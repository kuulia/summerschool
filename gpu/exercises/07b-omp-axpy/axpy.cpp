// SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include "helper_functions.hpp"
#include <cstdio>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
  // Array size
  int n = 102400;
  if (argc > 1) {
    n = std::stoi(argv[1]);
  }
  printf("Array size n = %d\n", n);

  double alpha;
  std::vector<double> x(n), y(n);
  double *_x = x.data();
  double *_y = x.data();

#pragma omp target data map(to : _x[0 : n]) map(tofrom : _y[0 : n])
  {
    // Initialization
    alpha = 3.0;
    for (int i = 0; i < n; i++) {
      double frac = 1.0 / ((double)(n - 1));
      _x[i] = i * frac;
      _y[i] = i * frac * 100;
    }

    // Print input values
    printf("Input:\n");
    printf("a = %8.4f\n", alpha);
    print_array("x", x);
    print_array("y", y);

    // Calculate axpy
    // TODO: Add OpenMP directives for GPU execution
#pragma omp target
#pragma omp teams distribute parallel for
    for (int i = 0; i < n; i++) {
      _y[i] += alpha * _x[i];
    }
  }

  // Print output values
  printf("Output:\n");
  print_array("y", y);

  return 0;
}
