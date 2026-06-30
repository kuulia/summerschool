<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title:  Introduction to OpenMP
event:  CSC Summer School in High-Performance Computing 2026
lang:   en
---

# Outline

- The idea of OpenMP
- OpenMP in practice: Creating, compiling, and running
- Three components of OpenMP: directives, runtime library functions, and environment variables

# Introduction to OpenMP {.section}

# What is OpenMP?

- OpenMP is a standard for **multithreaded, shared-memory parallelization**, first version (1.0) published in 1997, latest (6.0) in 2024
  - <https://www.openmp.org/specifications/>
- The standard is implemented by different compilers for C, C++, and Fortran
  - [GCC](https://gcc.gnu.org/) 15 supports all of OpenMP 4.5, most of 5.0, 5.1, and 5.2, and the first 6.0 features for C, C++, and Fortran
  - [Clang](https://www.llvm.org/) 21 supports all of OpenMP 4.5, almost all of 5.0 and most of 5.1 and 5.2 features for C and C++
  - See [this list](https://www.openmp.org/resources/openmp-compilers-tools/) for more

# Why would you want to learn OpenMP?

- OpenMP enables one to parallelize one part of the program at a time
  - Get some speedup with a limited programming effort
  - Efficient and well scaling code still requires work
- Serial and OpenMP versions of the code can easily coexist
- Some problems are easier to parallelize with OpenMP than with MPI (e.g., dynamic load balancing)
- OpenMP-parallelized program can be run on a laptop or a single node of a supercomputer
  - A modern node has 100+ CPU cores $\to$ a lot of compute power for shared-memory parallelization

# Why would you want to learn OpenMP?

- OpenMP can be seamlessly integrated with MPI for hybrid MPI+OpenMP programming
  - Use MPI between nodes and OpenMP within a node
  - OpenMP can improve scaling and reduce memory usage in contrast to plain MPI


# Parallel programming models

 ![](img/processes-threads.svg){.center width=80%}
<br>
<div class=column>
**MPI: Processes**

- Independent execution units
- Have their **own** memory space
- MPI launches N processes at application startup
- Works over multiple nodes
</div>
<div class=column>

**OpenMP: Threads**

- Threads **share** memory space
- Threads are created and destroyed (parallel regions)
- Limited to a single node
</div>


# OpenMP in practice {.section}

# Programming OpenMP

- OpenMP is mainly programmed through compiler directives, i.e., language extensions
  - Also runtime library routines exist
- OpenMP directives start with `#pragma omp` / `!$omp`, followed by the directive name (e.g., `parallel`) and optional clauses

<div class=column>
```c++
#pragma omp parallel [clauses]
{
  // This block is executed with
  // multiple threads in parallel
}
```
</div>

<div class=column>
```fortranfree
!$omp parallel [clauses]
  ! This block is executed with
  ! multiple threads in parallel

!$omp end parallel
```
</div>

- Directives are ignored when the code is compiled without OpenMP support $\to$ usual serial code

# Compiling an OpenMP program

- OpenMP is enabled during compilation with a command line option
  - GNU, Clang, Intel, Cray: `-fopenmp`
  - Intel legacy: `-qopenmp`
  - NVIDIA: `-mp`

- Demo: `hello.{cpp,F90}`


# Launching an OpenMP program

- An OpenMP program can be executed as a normal executable
- By default, the number of threads is decided by the runtime
  - Default is often the number of CPU cores available for the process
  - Can be controlled by environment variables or by additional clauses (see the next slide)
  - Other environment variables exists too for controlling the execution


# Controlling the number of threads

- The `OMP_NUM_THREADS` environment variable can be used to set the number of threads launched in the `parallel` region
  - Example: set `export OMP_NUM_THREADS=4` before executing the program
- Alternatively, the `num_threads` clause can be used to fix the number of threads within the code

<div class=column>
```c++
#pragma omp parallel num_threads(4)
{
  ...
}
```
</div>
<div class=column>
```fortranfree
!$omp parallel num_threads(4)
  ...

!$omp end parallel
```
</div>


# Three components of OpenMP {.section}

# Three components of OpenMP

- Compiler directives
- Runtime library routines
- Environment variables

# Constructs that *generate parallelism* vs *distribute work*

- OpenMP `parallel` construct **generates parallelism**, that is, creates multiple threads (that execute the same code)
  - Barrier at the end of the block
  - Also other constructs that generate parallelism exist
- OpenMP `for`/`do` constructs **distribute work**, that is, assign different threads to different loop iterations (so that the computation work is distributed)
  - Construct must reside inside a parallel region
  - Implicit barrier at the end of the block
  - Also other constructs that distribute work exist


# Constructs that *generate parallelism* vs *distribute work*

<div class=column>
```c++
#pragma omp parallel
{
  // This line is executed by all threads
  printf("Thread created\n");

  #pragma omp for
  for (int i = 0; i < n; i++) {
    // Loop iterations are executed by
    // different threads
    print("Thread running iteration %d\n", i);
  }


  // This line is executed by all threads
  printf("Thread loop done\n");
}
```
</div>

<div class=column>
```fortranfree
!$omp parallel

  ! This line is executed by all threads
  print *, "Thread created"

  !$omp do
  do i = 1, n
    ! Loop iterations are executed by
    ! different threads
    print *, "Thread running iteration", i
  end do
  !$omp end do

  ! This line is executed by all threads
  print *, "Thread loop done"
!$omp end parallel
```
</div>


# Composite directives

- In many cases composite directives are convenient

::::::{.columns}
:::{.column}
```c++
#pragma omp parallel for
for (int i = 0; i < n; i++) {
  ...

}
```
:::
:::{.column}
```fortranfree
!$omp parallel do
do i = 1, n
  ...
end do
!$omp end parallel do
```
:::
::::::


# Collapsing loops

- The `collapse(n)` clause can be used to combine *n* nested loops to a single large loop

::::::{.columns}
:::{.column}
```c++
#pragma omp parallel for collapse(2)
for (int i = 0; i < n; i++) {
  for (int j = 0; j < m; j++) {
    ...
  }
}
```
:::
:::{.column}
```fortranfree
!$omp parallel do collapse(2)
do i = 1, n
  do j = 1, m
    ...
  end do
end do
!$omp end parallel do
```
:::
::::::


# Fortran: `workshare` directive

- Fortran has a syntax for performing array operations without explicit loops
- The `workshare` directive divides the execution of the enclosed structured block into separate units of work, each of which is executed only once

```fortranfree
real :: a(n,n), b(n,n), c(n,n) d(n,n)
...
!$omp parallel
  !$omp workshare
  c = a * b
  d = a + b
  !$omp end workshare
!$omp end parallel
```

# OpenMP runtime library routines

- OpenMP provides runtime library routines in header / module:
  - C/C++: `#include <omp.h>`
  - Fortran: `use omp_lib`
- Some useful routines:
  - `omp_get_thread_num()` returns the thread id of the calling thread
  - `omp_get_num_threads()` returns the total number of threads
  - `omp_get_wtime()` returns the elapsed wall clock time in seconds


# OpenMP conditional compilation

- The `_OPENMP` macro is defined when OpenMP is enabled
- Use the macro to compile different code with and without OpenMP:

```cpp
#ifdef _OPENMP
  // This code is compiled when OpenMP is enabled (e.g. OpenMP library calls)
#else
  // This code is compiled when OpenMP is not enabled
#endif
```

# OpenMP environment variables

- OpenMP standard defines a set of environment variables
- The environment variables are set before the program execution and they are read during the program start-up
  - Changing the variables during the execution has no effect


# Some useful environment variables

| Variable                | Action                                              |
| ----------------------- | --------------------------------------------------- |
| `OMP_NUM_THREADS`       | Set the umber of threads to use                     |
| `OMP_DISPLAY_ENV`       | Print the OpenMP environment info to stderr         |
| `OMP_PROC_BIND`         | Bind threads to CPUs                                |
| `OMP_PLACES`            | Specify the placement of threads                    |
| `OMP_DISPLAY_AFFINITY`  | Print the thread affinities to stderr               |

- The last three are covered in detail when affinity are discussed

# Summary {.section}

# Summary

- OpenMP is programmed mainly through compiler directives
  - The directives are ignored when code is build without OpenMP
- Threads are created in `parallel` regions
- Loops can be parallelized with a `for`/`do` construct
- Library routines allow queruing the number of threads and thread ids
- Execution can be controlled with environment variables
