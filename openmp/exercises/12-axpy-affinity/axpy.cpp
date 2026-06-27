// SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include "helper_functions.hpp"
#include <cstdio>
#include <iostream>
#include <omp.h>
#include <ostream>
#include <span>
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
  double *_x = (double *)malloc(n * sizeof(double));
  std::span<double> x(_x, n);
  double *_y = (double *)malloc(n * sizeof(double));
  std::span<double> y(_y, n);

  // Initialization
  alpha = 3.0;
#pragma omp parallel for schedule(static, 10)
  for (int i = 0; i < n; i++) {
    double frac = 1.0 / ((double)(n - 1));
    x[i] = i * frac;
    y[i] = i * frac * 100;
  }

  // Print input values
  printf("Input:\n");
  printf("a = %8.4f\n", alpha);
  print_array("x", x);
  print_array("y", y);

  // Start timing
  double t0 = omp_get_wtime();

// Calculate axpy
#pragma omp parallel for schedule(static, 20)
  for (int i = 0; i < n; i++) {
    y[i] += alpha * x[i];
  }

  // End timing
  double t1 = omp_get_wtime();

  // Print output values
  printf("Output:\n");
  print_array("y", y);
  printf("Calculation took %.3f milliseconds\n", (t1 - t0) * 1e3);

  return 0;
}
