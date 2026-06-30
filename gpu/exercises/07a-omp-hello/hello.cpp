/*
 * SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
 *
 * SPDX-License-Identifier: MIT
 */

#include <cmath>
#include <cstdio>
#include <omp.h>
#include <vector>

int main(void) {
  const int N = 100000;
  std::vector<float> result(N, 0.0f);
  float *d_result = result.data();

  printf("Hello from host!\n");

#pragma omp target teams distribute parallel for map(tofrom : d_result[0 : N])
  for (int i = 0; i < N; i++) {
    float acc = 0.0f;
    for (int j = 1; j < 10000; j++) {
      acc += std::sin(i) * std::cos(j);
    }
    d_result[i] = acc;
  }

  printf("result[0] = %f\n", result[0]);
  return 0;
}
