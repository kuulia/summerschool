<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Exercise: Interoperability with HIP

In this exercise we practice calling external GPU libraries from OpenMP offload.

The issue of calling external libraries boils down to the question how to get
the correct device pointer so that it can be passed to the library call.
OpenMP provides a clause for this: `use_device_ptr` / `use_device_addr`.

As the GPU libraries have often C interface, the Fortran case is a bit more
involved and requires the use of `iso_c_binding`.
Example bindings are provided.

The [solution directory](solution/) contains a model solution and discussion on the exercises below.

## Tasks: Examine the array locations

1. The code 'pointers' prints the address of an array
   in the host context and in the device context using `use_device_ptr` / `use_device_addr`.

   Compile and run the code on GPU and study the output.

2. Compile and run the code for CPU target and study the output.


## Tasks: Call axpy using hipblas

1. The code 'axpy' tries to call axpy from hipblas, but it's broken.

   Compile the code with

       CC -fopenmp -O3 -Wall axpy.cpp -lhipblas -o axpy.x
       ftn -fopenmp -O3 axpy.F90 -lhipblas -o axpy.x

   and run it on GPU. What is the error you get?

2. In order to make the hipblas call work, we need to fix the data passed to the call
   (see 'TODO' in the code).

   Fix the code, using 'pointers' code as an example.
