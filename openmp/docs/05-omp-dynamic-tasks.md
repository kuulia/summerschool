<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title:  Dynamic load balancing and tasks
event:  CSC Summer School in High-Performance Computing 2026
lang:   en
---

# Outline

- Dynamic load balancing in OpenMP
- OpenMP tasks


# Dynamic load balancing {.section}

# Static vs dynamic scheduling of work

- The `for`/`do` constructs use by default _static_ scheduling
  - The iterations are distributed in chunks
  - For example, iterations 0-9 &rarr; thread 1, iterations 10-19 &rarr; thread 2, ...
- This is very efficient if each iteration is doing roughly the same amount of work
- For unbalanced load, it is also possible to request _dynamic_ scheduling
  - Each thread executes a chunk of iterations, then requests another chunk, until no chunks remain to be distributed
  - The cost is extra runtime overhead in the scheduling work

# Schedule clause

**`schedule(kind[, chunk_size])`**

- Here `kind` can be `static`, `dynamic`, or a few others, see the [specification](https://www.openmp.org/spec-html/5.1/openmpsu48.html)

<div class=column>
```c++
#pragma omp parallel
{
  #pragma omp for schedule(static)
  for (int i = 0; i < n; i++) {
    constant_work(i);
  }

  #pragma omp for schedule(dynamic, 4)
  for (int i = 0; i < n; i++) {
    varying_work(i);
  }
}
```
</div>
<div class=column>
```fortranfree
!$omp parallel
  !$omp do schedule(static)
  do i = 1, n
     constant_work(i)
  end do
  !$omp end do

  !$omp do schedule(dynamic, 4)
  do i = 1, n
     varying_work(i)
  end do
  !$omp end do
!$omp end parallel
```
</div>


# Task parallelisation {.section}

# Limitations of work distribution so far

- Number of iterations in a `for`/`do` loop must be constant
  - No early exits in the loops allowed
- OpenMP tasks enable parallelisation of irregular and dynamical patterns
  - While loops
  - Recursion


# Task in OpenMP

- A task has
  - Code to execute
  - Data environment
  - Internal control variables
- Tasks are added to a task queue, and executed then by any single thread
  - OpenMP runtime takes care of distributing tasks to threads
  - Execution may be deferred or started immediately after tasks is created
- Tasks are somewhat similar to GPU kernels launched from host


# OpenMP task construct

- Create a new task and add it to task queue
  - Store data and code to be executed
  - Task constructs can be arbitrarily nested

<div class=column>
```cpp
#pragma omp task [clause[[,] clause], ...]
{
  ...
}
```
</div>
<div class=column>
```fortranfree
!$omp task [clause[[,] clause], ...]
...
!$omp end task
```
</div>


# OpenMP task construct

- All threads that encounter the construct create a task
- Typical usage pattern is thus that a single thread creates the tasks

<div class=column>
```cpp
#pragma omp parallel
#pragma omp single
{
  int i = 0;
  while (i < 12) {
    #pragma omp task
    {
      printf("Task %d by thread %d\n", i,
             omp_get_thread_num());
    }
    i++;
  }
}
```
</div>
<div class=column>
```fortranfree
!$omp parallel private(i)
!$omp single
  i = 0
  do while (i < 12)
    !$omp task
      print *, "Task", i, "by thread", &
               omp_get_thread_num()
    !$omp end task
    i = i + 1
  end do
!$omp end single
!$omp end parallel
```
</div>


# OpenMP task construct

How many tasks does the following code create when executed with 4 threads?
<br>
`a) 6  b) 4  c) 24`

<div class=column>
```c
#pragma omp parallel
{
  int i=0;
  while (i < 6) {
    #pragma omp task
    {
      do_some_heavy_work();
    }
    i++;
  }
}

```
</div>
<div class=column>
```fortranfree
!$omp parallel
  i = 0
  do while (i < 6)
    !$omp task
      do_some_heavy_work();
    !$omp end task
    i = i + 1
  end do
!$omp end parallel
```
</div>


# Task execution model

- Tasks are executed by an arbitrary thread
    - Can be same or different thread that created the task
    - By default, tasks are executed in an arbitrary order
    - Each task is executed only once
- Synchronisation points
    - Implicit or explicit barriers
    - `#pragma omp taskwait / !$omp taskwait`
        - Encountering task suspends until child tasks complete


# Data environment of a task

- Tasks are created at one time, and executed at another
    - What data does the task see when executing?
- Variables that are `private` in the enclosing construct are made
  `firstprivate` and contain the data at the time of creation
- Variables that are `shared` in the enclosing construct contain the data at
  the time of execution
- Data scoping clauses (`shared`, `private`, `firstprivate`, `default`) can
  change the default behaviour


# Recursive algorithms with tasks

- A task can itself generate new tasks &rarr; useful for recursive algorithms
- Recursive (inefficient) algorithm for Fibonacci numbers:
  $F_0=0, \quad F_1=1, \quad F_n = F_{n-1} + F_{n-2}$

<div class=column>
```c
#pragma omp parallel
#pragma omp single
{
  fib(10);
}
```
</div>

<div class=column>
```cpp
int fib(int n) {
  int f1, f2;
  if (n < 2)
    return n;
  #pragma omp task shared(f1)
  f1 = fib(n-1);
  #pragma omp task shared(f2)
  f2 = fib(n-2);
  #pragma omp taskwait
  return f1+f2;
}
```
</div>


# Task dependencies

- The `depend` clause can be used to specify constraints on the task execution order
  - Allows fine-grained scheduling of tasks that share data. No need for `taskwait` after every task!
- Dependency (`in`, `out`, and `inout`) is associated with the memory address of a variable
  - Rule: `in` tasks must execute after any **previously created** `out` / `inout` tasks
  - Note! The dependency variables can also be dummy variables used solely for ordering the tasks

# Example: Task dependencies

```c
int a, b;

#pragma omp task depend(out: a)
a = -1;

#pragma omp task depend(in: a) // Guaranteed to run after the `out: a` task
b = 2 * a;
```


# Summary {.section}

# Summary

- OpenMP allows dynamic load balancing and work creation
- Dynamic scheduling of loop iterations
- Task construct to create parallel tasks at runtime

