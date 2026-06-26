// SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include "helper_functions.hpp"
#include <cmath>
#include <cstdio>
#include <omp.h>
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

  // Initialization
  alpha = 3.0;
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

  // Calculate axpy
  std::vector<double> y_new(n);
  double start = omp_get_wtime();
#pragma omp parallel for
  for (int i = 0; i < n; i++) {
    y_new[i] = y[i] + alpha * std::sin(x[i]) * std::cos(x[i]);
  }

  y = y_new;

  double end = omp_get_wtime();
  printf("Work took %f seconds\n", end - start);
  // Print output values
  printf("Output:\n");
  print_array("y", y);

  return 0;
}
