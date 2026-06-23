// SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <cstdio>
#include <string>
#include <vector>
#include "helper_functions.hpp"


int main(int argc, char* argv[]) {
    // Array size
    int n = 102400;
    if (argc > 1) {
        n = std::stoi(argv[1]);
    }
    printf("Array size n = %d\n", n);

    double alpha;
    std::vector<double> x(n), y(n);
    double *_x = x.data(), *_y = y.data();

    #pragma omp target data map(alloc: _x[0:n]) map(from: _y[0:n])
    {
        // Initialization
        alpha = 3.0;
        #pragma omp target teams distribute parallel for
        for (int i = 0; i < n; i++) {
            double frac = 1.0 / ((double) (n - 1));
            _x[i] = i * frac;
            _y[i] = i * frac * 100;
        }

        // Print input values
        printf("Input:\n");
        printf("a = %8.4f\n", alpha);
        #pragma omp target update from(_x[0:n]) from(_y[0:n])
        print_array("x", x);
        print_array("y", y);

        // Calculate axpy
        #pragma omp target teams distribute parallel for
        for (int i = 0; i < n; i++) {
            _y[i] += alpha * _x[i];
        }
    }

    // Print output values
    printf("Output:\n");
    print_array("y", y);

    return 0;
}
