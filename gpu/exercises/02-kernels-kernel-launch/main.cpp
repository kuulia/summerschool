// SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <hip/hip_runtime.h>
#include <assert.h>
#include <stdio.h>

__global__ void hello(int32_t num_blocks, int32_t num_threads) {
    assert(num_blocks != 10);
    const auto tid = threadIdx.x + blockIdx.x * blockDim.x;
    printf("Hello world, this is kernel speaking! Thread %d[block %d, thread %d] out of %d is printing this message\n", tid,blockIdx.x,threadIdx.x, num_blocks * num_threads);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Give number of blocks and number of threads as arguments\n");
        printf("For example \"%s 1 8\"\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const int32_t num_blocks = std::atoi(argv[1]);
    const int32_t num_threads = std::atoi(argv[2]);

    printf("Launching with %d blocks and %d threads\n", num_blocks, num_threads);
    hello<<<num_blocks, num_threads>>>(num_blocks, num_threads);
    [[maybe_unused]] const auto result = hipDeviceSynchronize();

    return 0;
}

