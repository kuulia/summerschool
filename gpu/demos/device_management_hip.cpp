// SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <cstdio>
#include <hip/hip_runtime.h>

int main(int argc, char *argv[])
{
    int count, device;

    hipGetDeviceCount(&count);
    hipGetDevice(&device);

    printf("Hello! I'm GPU %d out of %d GPUs in total.\n", device, count);

    hipSetDevice(count - 1);
    hipGetDevice(&device);
    printf("Now I'm GPU %d.\n", device);

    return 0;
}
