// SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <algorithm>
#include <array>
#include <cassert>
#include <charconv>
#include <cmath>
#include <type_traits>
#include <vector>

#if defined(HIP_TAYLOR)
#include "hip_taylor.hpp"
#define GPU
#elif defined(OMP_TAYLOR)
#include "omp_taylor.hpp"
#elif defined(SERIAL_TAYLOR)
#include "serial_taylor.hpp"
// Computation is optimized away without this on serial runs
static volatile int dummy = 0;
#else
static_assert(false, "Define one of HIP_TAYLOR, OMP_TAYLOR or SERIAL_TAYLOR "
                     "when compiling this file");
#endif

int main(int argc, char **argv) {
    // Parse the number of terms in the Taylor sum
    if (argc != 2) {
        std::fprintf(stderr,
                     "Give the number of terms in the Taylor series as an"
                     "input argument\n");
        std::fprintf(stderr, "E.g.: %s 10\n", argv[0]);

        return EXIT_FAILURE;
    }

    size_t N = 0;
    const auto [ptr, ec] =
        std::from_chars(argv[1], argv[1] + std::strlen(argv[1]), N);
    if (ec != std::errc()) {
        std::fprintf(stderr,
                     "Failed to convert first input argument to size_t. Given "
                     "argument: %s\n",
                     argv[1]);

        return EXIT_FAILURE;
    }

    using T = float;

    // Run for these vector sizes
    static constexpr std::array ns{
        1ul << 6,  1ul << 9,  1ul << 12, 1ul << 15, 1ul << 18,
        1ul << 21, 1ul << 24, 1ul << 27, 1ul << 30,
    };
    static constexpr size_t max_n = *std::max_element(ns.begin(), ns.end());
    static constexpr size_t num_bytes = sizeof(T) * max_n;

    // Compute Taylor series for values between minimum and maximum
    const T minimum = 1.0f;
    const T maximum = 10.0f;

    // Evaluated at middle
    const T a = (maximum - minimum) / static_cast<T>(2.0f);
    const T exa = std::exp(a);

    auto taylor = [exa, a, N](auto *x, auto *y, size_t i) {
        auto sum = static_cast<std::remove_pointer_t<decltype(x)>>(1.0);
        if (N > 0) {
            const auto xi = x[i] - a;
            sum += xi;
            if (N > 1) {
                auto xn = xi;
                decltype(sum) factorial = 1.0;

                for (size_t n = 2; n <= N; n++) {
                    xn *= xi;
                    factorial *= static_cast<decltype(factorial)>(n);
                    sum += xn / factorial;
                }
            }
        }
        y[i] = exa * sum;
    };

    auto init = [minimum, maximum](auto *x, auto *y, size_t i, size_t size) {
        const auto width = maximum - minimum;
        x[i] = minimum + i * width / size;
        y[i] = 0.0;
    };

    // Check the Taylor expansion for correctness, but only if using enough
    // terms, as with few terms the sum is a bad approximation
    if (N > 8) {
        std::vector<T> x(100);
        std::vector<T> y(100);

        for (auto i = 0ul; i < x.size(); i++) {
            init(x.data(), y.data(), i, x.size());
            taylor(x.data(), y.data(), i);
            const auto ex = std::exp(x[i]);

            const auto rel_diff = std::abs(y[i] - ex) / ex;
            const bool within_tolerance = rel_diff < static_cast<T>(0.01f);

            if (not within_tolerance) {
                std::printf("Taylor expansion not within tolerance: x: %f, y: "
                            "%f, e^x: %f relative difference: %f\n",
                            x[i], y[i], ex, rel_diff / x[i]);
            }
            assert(within_tolerance);
        }
    }

    // Each version defines this structure
    // and allocation/deallocation functions
    MeasuredLoop<T> loop = {};
    T *x = static_cast<T *>(allocate(num_bytes));
    T *y = static_cast<T *>(allocate(num_bytes));

    for (size_t n : ns) {
        auto init_xyn = [init, x, y, n](size_t i) { init(x, y, i, n); };
        auto f_xy = [taylor, x, y](size_t i) { taylor(x, y, i); };

        // Init once per n, so the x values are correct
        loop.loop(init_xyn, n);

        static constexpr auto n_iter = 20ul;

        size_t avg_with_overhead = 0ul;
        for (auto iteration = 0ul; iteration < n_iter; iteration++) {
            const auto with_overhead = loop.with_overhead(f_xy, n);
            if (iteration > 0) {
                avg_with_overhead += with_overhead;
            }
        }

        size_t avg = 0ul;
        for (auto iteration = 0ul; iteration < n_iter; iteration++) {
            const auto without_overhead = loop.without_overhead(f_xy, n);
            if (iteration > 0) {
                avg += without_overhead;
            }
        }

#ifdef SERIAL_TAYLOR
        // Computation is optimized away without this on serial runs
        dummy = y[16];
#endif

        std::fprintf(stdout, "%ld, %ld, %ld\n", n, (avg / (n_iter - 1)),
                     avg_with_overhead / (n_iter - 1));
    }

    deallocate(x);
    deallocate(y);

    return 0;
}
