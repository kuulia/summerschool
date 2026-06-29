// SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

/*
 * This code in its current form uses the default stream
 * Task:
 * - Place kernel_{a,b,c} in separate streams and execute them asynchronously
 * - Validate that kernels are executing concurrently with `run_tue ... rocprof
 * --hip-trace ./<your->
 *   - Open chromium url chrome://tracing or https://ui.perfetto.dev, open file
 * "results.json"
 */

#include "error_checking.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

// GPU kernel definition
__global__ void kernel_a(float *a, int n) {
  int tid = threadIdx.x + blockIdx.x * blockDim.x;

  // Evaluate the trigonometric identity
  // sin^2(x) + cos^2(x) = 1
  // Very light kernel, one sin/cos evaluation per element
  if (tid < n) {
    float x = 0.001f * float(tid % 1000);
    float s = sinf(x);
    float c = cosf(x);

    a[tid] = s * s + c * c;
  }
}

__global__ void kernel_b(float *b, int n) {
  int tid = threadIdx.x + blockIdx.x * blockDim.x;

  // Heavy kernel: repeatedly updates x using sine, cosine, and arctangent
  // Converges to 1.313534
  if (tid < n) {
    float x = 0.001f * float(tid % 1000) + 1.0f;

    for (int i = 0; i < 200; ++i) {
      x = sinf(x) + cosf(x) + 0.1f * atanf(x);
    }

    b[tid] = x;
  }
}

__global__ void kernel_c(float *c, int n) {
  int tid = threadIdx.x + blockIdx.x * blockDim.x;

  if (tid < n) {
    float x = 0.001f * float(tid % 1000);

    // Fixed-point iteration for cos(x) = x.
    // Converges to ~0.739085
    // Medium
    for (int i = 0; i < 50; ++i) {
      x = cosf(x);
    }

    c[tid] = x;
  }
}

int main() {
  constexpr size_t N = 1 << 24; // ~64 MiB array

  constexpr int blocksize = 256;
  constexpr int gridsize = (N - 1 + blocksize) / blocksize;
  constexpr size_t N_bytes = N * sizeof(float);

  // Host & device pointers
  float *a;
  float *d_a;
  float *b;
  float *d_b;
  float *c;
  float *d_c;

  // Host allocations
  a = (float *)malloc(N_bytes);
  b = (float *)malloc(N_bytes);
  c = (float *)malloc(N_bytes);

  // Timing
  float t_kernel_a_ms;
  float t_kernel_b_ms;
  float t_kernel_c_ms;

  // create three separate streams
  hipStream_t stream_a, stream_b, stream_c;
  HIP_ERRCHK(hipStreamCreate(&stream_a));
  HIP_ERRCHK(hipStreamCreate(&stream_b));
  HIP_ERRCHK(hipStreamCreate(&stream_c));

  // Device allocations
  HIP_ERRCHK(hipMalloc((void **)&d_a, N_bytes));
  HIP_ERRCHK(hipMalloc((void **)&d_b, N_bytes));
  HIP_ERRCHK(hipMalloc((void **)&d_c, N_bytes));

  // warmup
  kernel_c<<<gridsize, blocksize>>>(d_a, N);
  HIP_ERRCHK(hipMemcpy(a, d_a, N_bytes / 100, hipMemcpyDefault));
  HIP_ERRCHK(hipDeviceSynchronize());

  // Execute kernels in sequence

  HIP_ERRCHK(hipMemcpyAsync(d_a, a, N_bytes, hipMemcpyHostToDevice, stream_a));
  HIP_ERRCHK(hipMemcpyAsync(d_b, b, N_bytes, hipMemcpyHostToDevice, stream_b));
  HIP_ERRCHK(hipMemcpyAsync(d_c, c, N_bytes, hipMemcpyHostToDevice, stream_c));

  hipEvent_t start_kernel_event_a, start_kernel_event_b, start_kernel_event_c;
  hipEvent_t stop_kernel_event_a, stop_kernel_event_b, stop_kernel_event_c;
  HIP_ERRCHK(hipEventCreate(&start_kernel_event_a));
  HIP_ERRCHK(hipEventCreate(&start_kernel_event_b));
  HIP_ERRCHK(hipEventCreate(&start_kernel_event_c));
  HIP_ERRCHK(hipEventCreate(&stop_kernel_event_a));
  HIP_ERRCHK(hipEventCreate(&stop_kernel_event_b));
  HIP_ERRCHK(hipEventCreate(&stop_kernel_event_c));
  HIP_ERRCHK(hipEventRecord(start_kernel_event_a, stream_a));
  // Launch each kernel in a different stream
  kernel_a<<<gridsize, blocksize, 0, stream_a>>>(d_a, N);
  HIP_ERRCHK(hipGetLastError());
  HIP_ERRCHK(hipEventRecord(start_kernel_event_a, stream_a));
  HIP_ERRCHK(hipEventElapsedTime(&t_kernel_a_ms, start_kernel_event_a,
                                 stop_kernel_event_a));
  printf("kernel_a time: %f ms\n", t_kernel_a_ms);

  kernel_b<<<gridsize, blocksize, 0, stream_b>>>(d_b, N);
  HIP_ERRCHK(hipGetLastError());

  kernel_c<<<gridsize, blocksize, 0, stream_c>>>(d_c, N);
  HIP_ERRCHK(hipGetLastError());

  // Copy results back (in the default stream with hipMemCpy)
  // #error synchronize the host with stream A, before copying d_A back
  HIP_ERRCHK(hipStreamSynchronize(stream_a));
  HIP_ERRCHK(hipMemcpyAsync(a, d_a, N_bytes, hipMemcpyDeviceToHost, stream_a));
  // #error synchronize the host with stream B, before copying d_B back
  HIP_ERRCHK(hipStreamSynchronize(stream_b));
  HIP_ERRCHK(hipMemcpyAsync(b, d_b, N_bytes, hipMemcpyDeviceToHost, stream_b));
  // #error synchronize the host with stream C, before copying d_C back
  HIP_ERRCHK(hipStreamSynchronize(stream_c));
  HIP_ERRCHK(hipMemcpyAsync(c, d_c, N_bytes, hipMemcpyDeviceToHost, stream_c));

  for (int i = 0; i < 10; ++i)
    printf("%f ", a[i]);
  printf("\n");

  for (int i = 0; i < 10; ++i)
    printf("%f ", b[i]);
  printf("\n");

  for (int i = 0; i < 10; ++i)
    printf("%f ", c[i]);
  printf("\n");
  // Free device and host memory allocations
  HIP_ERRCHK(hipFree(d_a));
  HIP_ERRCHK(hipFree(d_b));
  HIP_ERRCHK(hipFree(d_c));

  // destroy all streams
  HIP_ERRCHK(hipStreamDestroy(stream_a));
  HIP_ERRCHK(hipStreamDestroy(stream_b));
  HIP_ERRCHK(hipStreamDestroy(stream_c));

  free(a);
  free(b);
  free(c);
}
