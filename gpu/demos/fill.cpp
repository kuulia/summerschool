// SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <hip/hip_runtime.h>
#include <stdio.h>

__global__ void fill_(int n, double *x, double a)
{
    int tid = threadIdx.x + blockIdx.x * blockDim.x;
    int stride = gridDim.x * blockDim.x;

    for (; tid < n; tid += stride) {
        x[tid] = a;
    }
}

int main(void)
{
    const int n = 10000;
    double a = 3.4;
    double x[n];
    double *x_;

    // allocate device memory
    hipMalloc(&x_, sizeof(double) * n);

    // launch kernel
    dim3 blocks(32);
    dim3 threads(256);
    fill_<<<blocks, threads>>>(n, x_, a);

    // copy data to the host and print
    hipMemcpy(x, x_, sizeof(double) * n, hipMemcpyDeviceToHost);
    printf("%f %f %f %f ... %f %f\n",
            x[0], x[1], x[2], x[3], x[n-2], x[n-1]);

    return 0;
}
