// SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#pragma once

#include <chrono>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <deque>
#include <ratio>

template <typename T> struct MeasuredLoop {
    std::deque<size_t> cached_elapsed = {};
    size_t call_index = 0;

    template <typename F>
    void loop(F f, size_t n) {
        for (size_t i = 0; i < n; i++) {
            f(i);
        }
    }

    template <typename F>
    size_t with_overhead(F f, size_t n) {
        const auto start = std::chrono::high_resolution_clock::now();
        loop(f, n);
        const auto end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double, std::nano> elapsed = end - start;
        cached_elapsed.push_back(elapsed.count());
        call_index++;

        return cached_elapsed.back();
    }

    template <typename F> size_t without_overhead(F, size_t) {
        // Serial single thread version has no overhead, so just returning the
        // cached values
        const auto elapsed = cached_elapsed[cached_elapsed.size() - call_index];
        call_index--;
        return elapsed;
    }
};

inline void *allocate(size_t num_bytes) {
    return malloc(num_bytes);
}

inline void deallocate(void *p) { free(p); }
