// SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <cstdio>
#include <string>
#include <vector>
#include <hipblas/hipblas.h>
#include "helper_functions.hpp"


int main(int argc, char* argv[]) {
    // Array size
    int n = 102400;
    if (argc > 1) {
        n = std::stoi(argv[1]);
    }
    printf("Array size n = %d\n", n);

    // Create handle for hipblas
    hipblasHandle_t handle;
    hipblasCreate(&handle);
    hipblasSetPointerMode(handle, HIPBLAS_POINTER_MODE_HOST);

    double alpha;
    std::vector<double> x(n), y(n);
    double *_x = x.data(), *_y = y.data();

    // Initialization
    alpha = 3.0;
    for (int i = 0; i < n; i++) {
        double frac = 1.0 / ((double) (n - 1));
        x[i] = i * frac;
        y[i] = i * frac * 100;
    }

    // Print input values
    printf("Input:\n");
    printf("a = %8.4f\n", alpha);
    print_array("x", x);
    print_array("y", y);

    // Calculate axpy
    #pragma omp target data map(to: _x[0:n]) map(tofrom: _y[0:n])
    #pragma omp target data use_device_ptr(_x, _y)
    hipblasDaxpy(handle, n, &alpha, _x, 1, _y, 1);

    // Print output values
    printf("Output:\n");
    print_array("y", y);

    // Destroy hipblas handle
    hipblasDestroy(handle);

    return 0;
}
