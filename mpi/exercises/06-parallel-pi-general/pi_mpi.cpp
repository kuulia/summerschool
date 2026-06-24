// SPDX-FileCopyrightText: 2019 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <cstdio>
#include <cmath>
#include <mpi.h>

constexpr long long n = 10000000;

int main(int argc, char** argv)
{
  MPI_Init(&argc, &argv);

  int rank, ntasks;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

  if (rank == 0) {
    printf("Computing approximation to pi with N=%lld\n", n);
    printf("Using %d MPI tasks\n", ntasks);
  }

  long long istart, istop;
  if (rank == 0) {
    istart = 1;
    istop = n / ntasks;
  } else {
    istart = rank * (n / ntasks) + 1;
    istop = (rank + 1) * (n / ntasks);
  }

  double t_start = MPI_Wtime();

  double partial = 0.0;
  for (long long i = istart; i <= istop; i++) {
    double x = (i - 0.5) / n;
    partial += 1.0 / (1.0 + x * x);
  }

  if (rank == 1) {
    MPI_Send(&partial, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  } else {
    double partial1;
    MPI_Recv(&partial1, 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    double pi = (partial + partial1) * 4.0 / n;
    double t_end = MPI_Wtime();
    printf("Approximate \t pi=%18.18f \n Exact \t\t pi=%10.18f)\n", pi, M_PI);
    printf("Time elapsed: %f seconds\n", t_end - t_start);
  }

  MPI_Finalize();
  return 0;
}