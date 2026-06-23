<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Exercise: Calculating axpy with OpenMP offload

In this exercise we practise parallelizing an axpy code using OpenMP offload.

The axpy operation is a fundamental linear algebra operation defined as

$$
y_i \leftarrow \alpha x_i + y_i
$$

where $\alpha$ is a scalar and $x$ and $y$ are vectors of the same size.

A serial example code is provided: The code initializes the input values $\alpha$, $x$, and $y$,
performs axpy operation, and prints the output $y$.

The [solution directory](solution/) contains a model solution and discussion on the tasks below.

## Tasks

1. Study, compile, and run the provided code. You can provide the array size as a command line argument, e.g., `./axpy.x 1024`.

   Note: the code includes a separate `helper_functions.{hpp,F90}` file that provides the `print_array()`
   helper function. You don't need to study the contents of the helper file, but you can use it as such.

2. Offload the axpy loop to GPU by adding suitable OpenMP directives (see 'TODO' in the code).

   Note! Remember to add also suitable data mapping clauses.

   Extra note for C++! The arrays are of type `std::vector`, but mapping clause requires pointers.
   Create raw pointers like `_x = x.data()` and use them for the GPU execution.

3. Would the program work without data mapping clauses?

   Hint: enable runtime debugging and examine the data transfers done in different cases.

4. Offload also the array initialization loop to GPU and create a single structured
   data region covering both GPU kernels.

   Hint: use `target update` to ensure that correct arrays are printed on CPU
   after the initialization.

5. (Bonus) Enable compiler diagnostics and study the compiler output.

6. (Bonus) Replace the structured data region with an unstructured data region.


### Bonus tasks: Offload to CPU threads

1. Load the modules for CPU execution.

2. Compile your OpenMP-offload code to CPU threads.

   Cray compiler wrappers choose the offload target based on the loaded modules that set specific environment variables,
   so now with these modules loaded, the same compilation command will target CPU threads.

3. Run the program on a CPU partition with, e.g., 4 threads.
   Set an environment variable to display thread affinities:

       export OMP_DISPLAY_AFFINITY=true

   Is the code executing correctly using threads?
