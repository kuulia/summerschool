<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Exercise: Hello world

In this exercise we practise the first steps of compiling and running an OpenMP code and using common library functions.

A minimal example code is provided: The program first prints a hello message in serial, and then
each thread prints another hello message in parallel.

The [solution directory](solution/) contains a model solution and discussion on the exercises below.


## Task: Compiling and running

1. Compile the C++ version:

       CC -O3 -fopenmp -Wall hello.cpp -o hello.x

   or the Fortran version:

       ftn -O3 -fopenmp hello.F90 -o hello.x

2. Use the provided job script `job.sh` to run the program on a single CPU core:

       sbatch job.sh

   and then on 4 cores:

       sbatch --cpus-per-task=4 job.sh

   How many lines of output you get?

3. You can control the number of threads explicitly with `OMP_NUM_THREADS` environment variable.
   Edit the job script to fix the number of threads to a fixed value of 2:

       export OMP_NUM_THREADS=2

   Submit the updated job on 4 cores:

       sbatch --cpus-per-task=4 job.sh

   How many lines of output you get?

4. Try out different numbers of threads. Do you get expected outputs?


## Task: Library functions

1. Modify the program so that each thread prints their thread id.
   Use `omp_get_thread_num()` library function.

2. Modify the program further so that the total number of threads is printed once.
   Use `omp_get_num_threads()` library function.


## Bonus task: Conditional compilation

1. Try to compile the code from previous task without OpenMP support. Why does it not work?

2. Use `_OPENMP` preprocessor macro to guard all OpenMP constructs so that code works still as a serial code.
