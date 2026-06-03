/*
 * SPDX-FileCopyrightText: 2024 CSC - IT Center for Science Ltd. <www.csc.fi>
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  printf("Hello\n");

  MPI_Finalize();
}
