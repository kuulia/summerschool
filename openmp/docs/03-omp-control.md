<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title:  Thread execution control
event:  CSC Summer School in High-Performance Computing 2026
lang:   en
---

# Outline

- Controlling thread execution in OpenMP
  - `barrier`, `masked`/`master`, `single`, `critical`, `atomic`


# OpenMP execution control {.section}

# Execution controls

- We have thus far used mainly `for`/`do` construct
- OpenMP provides many more clauses for controlling the execution of code blocks
- Sometimes a part of a parallel region should be executed only
  once (possibly by a specific thread) or by all threads but one at a time
  - I/O, initializations, updating global values, etc.


# Execution control constructs

**`omp barrier`**

- When a thread reaches a barrier it only continues after all the threads in
  the same thread team have reached it
  - Every thread must reach the barrier, or none at all
  - The sequence of work-sharing regions and barrier regions must be the same for all threads
- There is an implicit barrier at the end of many constructs
  - `for`/`do`, `parallel`, `single`, `workshare`
  - Add `nowait` clause to remove the implicit barrier


# Execution control constructs: a specific thread

<div class=column>
**`omp masked [ filter(id) ]`**

- Specifies a region that should be executed only by the thread of given id (0 by default without filter)
- No implicit barrier at the end, *i.e.*, other threads do not wait
</div>


<div class=column>
**`omp master`**

- Like `masked`, but only for the thread id 0
- Deprecated in OpenMP 5.1 and replaced with `masked`
</div>

# Execution control constructs: any single thread

**`omp single [ nowait ]`**

- Specifies that a region should be executed only by a single (arbitrary) thread
- Implicit barrier at the end, *i.e.*, other threads wait
- Adding `nowait` removes the implicit barrier


# Examples

<div class=column>
```c++
#pragma omp parallel
{
  #pragma omp masked filter(0)
  {
    // This line is executed by thread 0 only
    printf("Hello masked\n");
  }

  #pragma omp single
  {
    // This line is executed by any single thread
    printf("Hello single\n");
  } // implicit barrier!

  // This line is executed by all threads
  printf("Hello all\n");
}
```
</div>

<div class=column>
```fortranfree
!$omp parallel

  !$omp masked filter(0)
    ! This line is executed by thread 0 only
    print *, "Hello masked"
  !$omp end masked

  !$omp single
    ! This line is executed by any single thread
    print *, "Hello single"
  !$omp end single
  ! implicit barrier!

  ! This line is executed by all threads
  print *, "Hello all"

!$omp end parallel
```
</div>





# Execution control constructs: one thread at a time

<div class=column>
**`omp critical`**

- A section that is executed by only one thread at a time
- No implicit barrier at the end
</div>

<div class=column>
**`omp atomic`**

- Like `critical`, but strictly limited construct to update of a single value
- Only guarantees atomic update, does not protect function calls
- Can be faster on hardware platforms that support atomic updates
</div>


# Manual work distribution

- Thread id can be used for distributing work manually (similar to MPI rank)

<div class=column>
```cpp
#pragma omp parallel
{
  #pragma omp for
  for (int i = 0; i < N; i++) {
    work(i);
  }
}
```
</div>

<div class=column>
```cpp
#pragma omp parallel
{
  int n_threads = omp_get_num_threads();
  int chunk = (N + n_threads - 1) / n_threads;
  int start = omp_get_thread_num() * chunk;
  int end = min(start + chunk, N);
  for (int i = start; i < end; i++) {
    work(i);
  }
}
```
</div>

- By default, the `for` clause generates similar static distribution of work, but the chuck size is implementation-dependent


# Summary {.section}

# Summary

- OpenMP has many construct for execution control required for different algorithm patters
  - `barrier`
  - `masked`/`master` and `single`
  - `critical` and `atomic`
