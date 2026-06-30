// SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <hip/hip_runtime.h>
#include <cmath>
#include <iostream>

#define N (1 << 20)
#define BLOCK_SIZE 256

// A compute-heavy function with transcendental math
__device__ double f1(double x)
{
    double acc = x;
    #pragma unroll 64
    for (int i = 0; i < 1024; ++i) {
        acc = sin(acc) * cos(acc) + sqrt(acc * acc + 1.0);
    }
    return acc;
}

// A different compute-heavy function (different instruction mix)
__device__ double f2(double x)
{
    double acc = x;
    #pragma unroll 64
    for (int i = 0; i < 1024; ++i) {
        acc = log(acc + 1.0) + exp(-acc) + acc * 0.5;
    }
    return acc;
}

__global__ void divergence_kernel(double* x, int n)
{
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid < n) {
        if (tid % 2 == 0) {
            x[tid] = f1((double)tid);
        } else {
            x[tid] = f2((double)tid);
        }
    }
}

__global__ void no_divergence_kernel(double* x, int n)
{
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid < n) {
        if (tid/64 % 2 == 0) {
            x[tid] = f1((double)tid);
        } else {
            x[tid] = f2((double)tid);
        }
    }
}

__global__ void singlebranch_kernel(double* x, int n)
{
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid < n) {
            x[tid] = f2((double)tid);
    }
}

__global__ void ternary_kernel(double* x, int n)
{
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid < n) {
        bool mask = (tid%2) == 0;
            x[tid] =   mask * f1((double)tid);
            x[tid] += (!mask) * f2((double)tid);
        }

}


int main()
{
    double* d_x;
    hipMalloc(&d_x, N * sizeof(double));

    dim3 block(BLOCK_SIZE);
    dim3 grid((N + block.x - 1) / block.x);

    divergence_kernel<<<grid, block>>>(d_x, N);
    hipDeviceSynchronize();

    no_divergence_kernel<<<grid, block>>>(d_x, N);
    hipDeviceSynchronize();

    singlebranch_kernel<<<grid, block>>>(d_x, N);
    hipDeviceSynchronize();

    ternary_kernel<<<grid, block>>>(d_x, N);
    hipDeviceSynchronize();

    hipFree(d_x);
    std::cout << "Kernel completed.\n";
    return 0;
}

