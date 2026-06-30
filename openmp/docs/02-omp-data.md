<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title:  Data environment of threads
event:  CSC Summer School in High-Performance Computing 2026
lang:   en
---

# Outline

- Data environment in OpenMP
  - `shared`, `private`, `firstprivate`


# OpenMP data environment {.section}

# How do OpenMP threads interact?

- Threads share the same memory address space, so they can interact using _shared variables_
- Threads often need some _private variables_ in addition to the shared variables
  - For example the index variable of a multithreaded loop
- If multiple threads write simultaneously to the same shared variable, a **race condition** occurs
  - End results varies randomly depending on the thread execution order
  - <https://deadlockempire.github.io/>
- Visibility of different variables is defined using _data-sharing clauses_


# Race condition in hello world

<!-- Presentation suggestion: live coding, multiple runs with different outcome
     Note! Intel classic compiler optimizes somehow the race condition away
           with -O2 or higher, GNU and Clang-based compilers (including Intel
           OneAPI) show the race condition.
 -->

<div class=column>
```cpp
#include <cstdio>
#include <unistd.h>
#include <omp.h>

int main(int argc, char* argv[]) {
  int thread_id;
  #pragma omp parallel
  {
    thread_id = omp_get_thread_num();
    sleep(1);
    printf("Hello from thread %d\n", thread_id);
  }
}
```
</div>
<div class=column>
```fortranfree




program hello
  use omp_lib
  integer :: thread_id
  !$omp parallel
    thread_id = omp_get_thread_num()
    call sleep(1)
    print *, "Hello from thread", thread_id
  !$omp end parallel
end program hello
```
</div>

- The result varies between successive runs


# Data-sharing clauses

- **`private(var1, var2, ...)`**
  - Private variables are stored in the private stack of each thread
  - Undefined initial value
  - Undefined value after parallel region
- **`firstprivate(var1, var2, ...)`**
  - Same as `private`, but with an initial value that is the same as
    the original value defined outside the parallel region


# Data-sharing clauses

- **`shared(var1, var2, ...)`**
  - All threads can write to and read from a shared variable
- **`default(private/shared/none)`**
  - Sets default for variables to be shared, private or not defined
  - In C/C++, `default(private)` is not allowed
  - Setting `default(none)` can be useful for debugging as each variable has to be
    defined manually


# Default data-sharing behaviour

- Most variables are `shared` by default
  - Global variables are shared among threads
    - C: static variables, file scope variables
    - Fortran: save and module variables, common blocks
    - `threadprivate(list)` can be used to make a private copy
- Private by default:
  - Local variables of functions called from parallel region
  - Local variables declared within the parallel region (C/C++)
  - Loop variables
- Good programming practice: declare all variables either shared or private


# Race condition in hello world fixed

<div class=column>
```c
#include <stdio.h>
#include <omp.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  int thread_id;
  #pragma omp parallel private(thread_id)
  {
    thread_id = omp_get_thread_num();
    sleep(1);
    printf("Hello from thread %d\n", thread_id);
  }
}
```
</div>
<div class=column>
```fortranfree




program hello
  use omp_lib
  integer :: thread_id
  !$omp parallel private(thread_id)
    thread_id = omp_get_thread_num()
    call sleep(1)
    print *, "Hello from thread", thread_id
  !$omp end parallel
end program hello
```
</div>



# Summary {.section}

# Summary

- Visibility of variables in parallel region can be specified with
  data sharing clauses
  - **`private`** : each thread has their own variable
  - **`shared`** : all threads share the same variable
- Race conditions possible when writing to shared variables
  - Avoiding race conditions is key to correctly functioning OpenMP programs
