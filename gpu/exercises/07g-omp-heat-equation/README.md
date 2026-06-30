<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

## Heat equation solver with MPI + OpenMP offload

In this exercise, your task is to parallelize the two dimensional heat equation with MPI + OpenMP offload.
See previous sections for a general description of the heat equation solver.

The provided initial code is the hybrid MPI + OpenMP code from the OpenMP section.
You are welcome to keep working on your own heat equation in this exercise.

1. Add OpenMP offload constructs into the main computational routine `evolve()`
   in [cpp/core.cpp](cpp/core.cpp) or [fortran/core.F90](fortran/core.F90).
2. Use data region to ensure that the data remains on GPU.
3. Assign MPI tasks to devices
4. Pass device pointers to MPI routines

To build the code, please use the provided `Makefile` (by typing `make`).
