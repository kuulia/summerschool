<!--
SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Discussion


## Task: Parallelize with OpenMP threads

1. Running the code with default arguments uses a random seed:

       Samples: 10000000
       Seed: 1740322028
       Thread   0: A few random values: 0.9783 0.5619 0.2925
       Average distance: 0.521524
       Calculation took 233.130 milliseconds

   The output with a fixed seed (0):

       Samples: 10000000
       Seed: 0
       Thread   0: A few random values: 0.8162 0.1267 0.3123
       Average distance: 0.521324
       Calculation took 234.036 milliseconds


2. **C++**

   See `lines-wrong.cpp` for a first attempt.
   Possible output with four threads:

       Samples: 10000000
       Seed: 0
       Thread   0: A few random values: 0.7972 0.7322 0.8610
       Thread   2: A few random values: 0.7972 0.7322 0.8610
       Thread   3: A few random values: 0.7972 0.7322 0.8610
       Thread   1: A few random values: 0.8434 0.7972 0.7322
       Average distance: 0.521259
       Calculation took 97.578 milliseconds

   We see that there is an issue with the random number sampling
   as multiple threads are getting the same random values.

   The issue is that the threads are sharing the same random number
   generator.

   For a working solution, see `lines.cpp`. Output:

       Samples: 10000000
       Seeds: 0 and thread number
       Thread   0: A few random values: 0.7252 0.8002 0.3484
       Thread   1: A few random values: 0.1049 0.3741 0.9227
       Thread   2: A few random values: 0.7151 0.7520 0.1685
       Thread   3: A few random values: 0.2976 0.6741 0.5666
       Average distance: 0.521422
       Calculation took 65.180 milliseconds

   The code creates a private random number generator for each thread
   and uses both the global seed and the thread number to seed
   the thread-private generators.

   **Note:** While this resolves the immediate issue of identical sequences,
   it is not a perfect solution. The resulting random number streams
   are not guaranteed to be fully independent. Subtle correlations
   may exist between the streams. For demanding applications
   this could impact the statistical quality of the result.
   In such cases, more robust parallel random number generation techniques
   should be used.

   **Note:** This approach makes the final result depend on
   the number of threads. If reproducibility is required
   (e.g. identical results regardless of thread count),
   additional care is needed, typically at the cost of increased
   implementation complexity or reduced performance.

   **Fortran**

   See `lines.F90`.

   **Note:** [OpenMP compliance](https://www.openmp.org/spec-html/5.2/openmpse6.html#x27-260001.6)
   requires that Fortran's `random_number` is thread safe,
   so the C++ race condition does not occur.

   However, the specification leaves it open how the thread safety is implemented,
   and also the performance depends on the implementation.
   With GCC 11, we get expected speed-up when using threads,
   but with Cray Fortran 19, the execution becomes only slower with threads.

   To achieve good performance and high quality sampling,
   proper parallel number generation should be used.
