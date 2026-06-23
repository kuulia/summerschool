// SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#pragma once

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <ratio>
#include <omp.h>
#include <vector>

template <typename T> struct MeasuredLoop {
    template <typename F>
    void loop(F f, size_t n) {
        // clang-format off
        #pragma omp parallel
        {
            const auto nthreads = omp_get_num_threads();
            const auto tid = omp_get_thread_num();

            const size_t local_n = n / nthreads;
            const size_t local_start = tid * local_n;
            const size_t local_end = (tid == nthreads - 1) ? n : local_start + local_n;

            for (size_t i = local_start; i < local_end; i++) {
                f(i);
            }
        }
        // clang-format on
    }

    template <typename F>
    size_t with_overhead(F f, size_t n) {
        const auto start = std::chrono::high_resolution_clock::now();
        loop(f, n);
        const auto end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double, std::nano> elapsed = end - start;

        return elapsed.count();
    }

    template <typename F>
    size_t without_overhead(F f, size_t n) {
        std::vector<size_t> elapsed(omp_get_max_threads(), 0);

        // clang-format off
        #pragma omp parallel
        {
            const auto nthreads = omp_get_num_threads();
            const auto tid = omp_get_thread_num();

            const size_t local_n = n / nthreads;
            const size_t local_start = tid * local_n;
            const size_t local_end = (tid == nthreads - 1) ? n : local_start + local_n;

            #pragma omp barrier

            const auto start = omp_get_wtime();
            for (size_t i = local_start; i < local_end; i++) {
                f(i);
            }
            const auto end = omp_get_wtime();
            elapsed[tid] = (end - start) * 1e9;
        }
        // clang-format on

        return *std::max_element(elapsed.begin(), elapsed.end());
    }
};

inline void *allocate(size_t num_bytes) {
    return malloc(num_bytes);
}

inline void deallocate(void *p) { free(p); }
