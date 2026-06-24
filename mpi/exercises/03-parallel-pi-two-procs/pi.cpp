// SPDX-FileCopyrightText: 2019 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <cstdio>
#include <cmath>
#include <chrono>

constexpr long long n = 10000000000LL;

int main(int argc, char** argv)
{
  printf("Computing approximation to pi with N=%lld\n", n);

  long long istart = 1;
  long long istop = n;

  auto t_start = std::chrono::high_resolution_clock::now();

  double pi = 0.0;
  for (long long i = istart; i <= istop; i++) {
    double x = (i - 0.5) / n;
    pi += 1.0 / (1.0 + x * x);
  }

  pi *= 4.0 / n;
  auto t_end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = t_end - t_start;

  printf("Approximate pi=%18.16f (exact pi=%10.8f)\n", pi, M_PI);
  printf("Time elapsed: %f seconds\n", elapsed.count());

  return 0;
}