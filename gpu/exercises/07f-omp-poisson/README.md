<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Exercise: Solving 2D Poisson's equation with Jacobi iteration

In this exercise we practise the data mapping clauses across multiple GPU kernels.

Poisson's equation in 2D is

$$
\nabla^2 u(x, y) = f(x, y)
$$

where $$f$$ is a given function and $$u$$ is the function to be solved.

This equation can be discretized using finite differences as:

$$
\frac{u_{i+1,j} - 2u_{i,j} + u_{i-1,j}}{\Delta x^2} + \frac{u_{i,j+1} - 2u_{i,j} + u_{i,j-1}}{\Delta y^2} = f_{i,j}
$$

Assuming a uniform grid where $$\Delta x = \Delta y = h$$, this simplifies to:

$$
\frac{u_{i+1,j} + u_{i-1,j} + u_{i,j+1} + u_{i,j-1} - 4u_{i,j}}{h^2} = f_{i,j}
$$

Rearranging terms gives the standard five-point stencil:

$$
u_{i,j} = \frac{1}{4} \left( u_{i+1,j} + u_{i-1,j} + u_{i,j+1} + u_{i,j-1} - h^2 f_{i,j} \right)
$$

This discretized equation can be solved iteratively using various numerical methods, and in this exercise,
we solve it using Jacobi iteration that updates all grid points simultaneously using values from the previous iteration.

The algorithm uses the five-point stencil:

$$
u_{i,j}^{(k+1)} = \frac{1}{4} \left( u_{i+1,j}^{(k)} + u_{i-1,j}^{(k)} + u_{i,j+1}^{(k)} + u_{i,j-1}^{(k)} - h^2 f_{i,j} \right)
$$

where $$u_{i,j}^{(k)}$$ is the value of $$u$$ at grid point $$(i,j)$$ during the $$k$$-th iteration.

The algorithm comprises of the following steps:

1. Initialize the grid with an initial guess $$u_{i,j}^{(0)}$$ (zeros in the example code).
2. Iterate over all interior grid points and update $$u_{i,j}^{(k+1)}$$ using values from $$u^{(k)}$$.
3. Repeat until the solution converges, i.e., the difference between successive iterations is below a chosen tolerance,
   or the maximum number of iterations is reached.

The provided codes implement this algorithm. In addition to the main algorithm, the code
prints the convergence every 100th iteration and writes the field to a file every 1000th iteration.

We want to speed up this code by utilizing GPU!

The [solution directory](solution/) contains a model solution and discussion on the exercises below.


## Tasks

1. Study, compile, and run the provided code. You can provide input sizes as command line arguments:
   running the program for a 1024x1024 array, 2000 iterations, and 2 repetitions:

       ./poisson.x 1024 2000 2

2. Use OpenMP offload constructs to port the code to GPU.
   Ensure that the results remain correct.

   Here are some ideas to try out:
   - Offload the stencil update to GPU
   - Use data region to keep the data on GPU during the whole main loop (remember to update the data on CPU when needed)
   - Offload the convergence check to GPU
   - Overlap CPU and GPU execution by using asynchronous execution; in particular,
     ensure that GPU is busy all the time by overlapping the data writing with GPU execution

   Hints:
   - Profile the code while developing it to identify bottlenecks.
     The helper functions provide tracing marks to the `write_array` function that can be enabled with `-DTRACE -lroctx64` compilation flags.
   - Increase the system size as 1024x1024 is a pretty small size for a well-performing GPU code
