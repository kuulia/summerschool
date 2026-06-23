<!--
SPDX-FileCopyrightText: 2021 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title:  "OpenMP offload for GPU programming: Part 1"
event:  CSC Summer School in High-Performance Computing 2026
lang:   en
---

# Outline

- The idea of OpenMP offload
- OpenMP constructs for GPU programming
- Compiling and running OpenMP offload code


# The idea of OpenMP offload {.section}

# What is OpenMP offload?

- Set of OpenMP directives that instruct the compiler to parallelize and offload specific code regions to the GPU (or other devices)
- Part of OpenMP standard
  - Introduced in OpenMP 4.0 and refined in later standards
- The same code can be compiled and run on various systems
  - NVIDIA GPUs, AMD GPUs, Intel GPUs, FPGAs, ...
  - CPUs only (threading, vectorization)

# OpenMP vs. OpenACC

- OpenACC is similar compiler directive based approach for GPU programming
  - Open standard, NVIDIA major driver
- Why OpenMP and not OpenACC?
  - OpenMP is likely to have a more extensive platform and compiler support
  - Currently, OpenACC support in AMD GPUs is limited (supported only for Fortran on LUMI), but
    OpenACC can provide better performance on NVIDIA GPUs

# OpenMP vs. CUDA/HIP

- Why OpenMP and not CUDA/HIP?
  - Less code to start utilizing GPU acceleration
  - Simple things are possibly simpler
  - Same code can be compiled for CPU and GPU targets
- Why CUDA/HIP and not OpenMP?
  - Can access all the features of the GPU hardware
  - Better control and assurance that the code will work as intended
  - More optimization possibilities
- Note! It's possible to use **both** OpenMP and CUDA/HIP in an application
  - Example: the bulk of the code accelerated with OpenMP,
    performance-critical parts optimized with CUDA/HIP

# OpenMP execution and data model

- Execution and data models are similar to CUDA/HIP
  - Host (CPU) offloads kernels to the accelerator device (GPU)
  - OpenMP offers data transfer constructs between CPU and GPU memories
- Differences to CUDA/HIP
  - By default, host waits for the device execution to finish
    - Asynchronous execution is also possible &rarr; covered later
  - Data transfers between CPU and GPU take place implicitly if not explicitly declared

# Example: CUDA/HIP vs OpenMP offload

::::::{.columns}
:::{.column}
<small>
**HIP**

```cpp
__global__ void axpy(float alpha, float* x, float* y, int n) {
  int i = blockDim.x * blockId.x + threadId.x;
  if (i > n) {
    y[i] += alpha * x[i];
  }
}

int main() {
  // Host side preparations
  ...

  // Transfer data from host to device
  hipMemcpy(d_x, x, N_bytes, hipMemcpyDefault);
  hipMemcpy(d_y, y, N_bytes, hipMemcpyDefault);
  // Execute kernel on device
  axpy<<<griddim, blockdim>>(alpha, d_x, d_y, n)
  // Wait for device to finish
  hipDeviceSynchronize();
  // Transfer data from device to host
  hipMemcpy(y, d_y, N_bytes, hipMemcpyDefault);
  // Calculated y is now available on host
  ...
}
```
</small>
:::
:::{.column}
<small>
**OpenMP offload**

```cpp
int main() {
  // Host side preparations
  ...

  // Declare data mapping
  // - to: copy from host to device at the beginning
  // - from: copy from device to host at the end
  #pragma omp target data map(to:x[0:n]) map(tofrom:y[0:n])
  {
    // Execute kernel on device
    #pragma omp target teams distribute parallel for
    for (int i = 0; i < n; i++) {
      y[i] += alpha * x[i];
    }
    // Host waits device to finish implictly by default
  }

  // Calculated y is now available on host
  ...
}
```
</small>
:::
::::::


# OpenMP constructs for GPU programming {.section}

# OpenMP constructs for GPU programming

- Specifying device execution: `target`
- Launching kernels
  - Creating teams and threads on device: `teams`, `parallel`
  - Distributing work across the created teams and threads: `distribute`, `for`/`do`
- Controlling data movement
  - Data mapping: `map`
  - Structured data regions: `target data`
  - Unstructured data regions: `target enter data`, `target exit data`


# Target construct

- OpenMP `target` construct specifies a region to be executed on GPU
  - Initially, runs with a single thread
- By default, execution in the host continues only after target region is finished
  - Asynchronous execution is also possible
- May trigger implicit data movements between the host and the device

::::::{.columns}
:::{.column}
```c++
#pragma omp target
{
  // code executed in device
}
```
:::
:::{.column}
```fortranfree
!$omp target
  ! code executed in device
!$omp end target
```
:::
::::::

- Demo: `omp-hello`

# Launching kernels {.section}

# Constructs that *generate parallelism* vs *distribute work*

- Target construct does not create any parallelism, so additional constructs are needed
- OpenMP `teams` and `parallel` constructs *generate parallelism*
  - For example, `parallel` creates multiple threads (that do the same computation by default)
- OpenMP `distribute` and `for`/`do` constructs *distribute work* so that the created teams and threads do different work
  - For example, `for`/`do` assign different threads to different loop iterations (so that the computation work is distributed)
- Note the similarity to the OpenMP threading!


# Creating teams

- A `teams` construct creates a league of teams
  - Number of teams is implementation-dependent
  - Initially, there is only a single thread in each team
- No synchronization between teams is possible
- *Likely mapping* to CUDA/HIP and hardware
  - Team &rarr; thread block, runs on a streaming multiprocessor (SM) / on a compute unit (CU)
  - Note! This mapping is not enforced by OpenMP standard; implentation has freedom to decide


# Creating threads within a team

- Just having a league of teams is typically not enough to leverage all the parallelism available in the accelerator
- A `parallel` construct within a `teams` region creates threads within each team
  - Number of threads per team is implementation-dependent
  - With N teams and M threads per team there will be N x M threads in total
- Threads within a team can synchronize
- *Likely mapping* to CUDA/HIP and hardware
  - Thread &rarr; thread, runs on a core of SM / on a lane of a SIMD unit of CU
  - Note! This mapping is not enforced by OpenMP standard; implentation has freedom to decide


# Creating teams and threads

::::::{.columns}
:::{.column}
```c++
#pragma omp target
#pragma omp teams
#pragma omp parallel
{
  // code executed in device
}
```
:::
:::{.column}
```fortranfree
!$omp target
!$omp teams
!$omp parallel
  ! code executed in device
!$omp end parallel
!$omp end teams
!$omp end target
```
:::
::::::

- Useful API functions: `omp_get_team_num()`, `omp_get_thread_num()`, `omp_get_num_teams()`, `omp_get_num_threads()`

- Demo: `omp-hello`


# Distributing work across teams and threads

- The `teams` and `parallel` constructs create teams and threads, however, all the threads are still executing the same code
- The `distribute` construct distributes loop iterations over teams
- The `for` / `do` construct distributes loop iterations over threads
- Note! The `distribute` and `for` / `do` assume that the loop iterations are independent
  - Compiler does not check if that is the case; it's the programmer's responsibility


# Distributing work across teams and threads

::::::{.columns}
:::{.column}
```c++
#pragma omp target
#pragma omp teams
#pragma omp distribute
for (int i = 0; i < N; i++) {
  #pragma omp parallel
  #pragma omp for
  for (int j = 0; j < M; j++) {
    ...
  }
}
```
:::
:::{.column}
```fortranfree
!$omp target
!$omp teams
!$omp distribute
do i = 1, N
  !$omp parallel
  !$omp do
  do j = 1, M
    ...
  end do
  !$omp end do
  !$omp end parallel
end do
!$omp end distribute
!$omp end teams
!$omp end target
```
:::
::::::

- Demo: `omp-hello`


# Controlling the number of teams and threads

- By default, the number of teams and the number of threads is up to the implementation to decide
- The `num_teams` clause for `teams` construct and `num_threads` clause for `parallel` construct can be used to specify number of teams and threads
  - May improve performance in some cases
  - Performance is most likely not portable

::::::{.columns}
:::{.column}
```c++
#pragma omp target
#pragma omp teams num_teams(32)
#pragma omp parallel num_threads(128)
{
  // code executed in device
}
```
:::
:::{.column}
```fortranfree
!$omp target
!$omp teams num_teams(32)
!$omp parallel num_threads(128)
  ! code executed in device
!$omp end parallel
!$omp end teams
!$omp end target
```
:::
::::::


# Composite directives

- In many cases composite directives are convenient
  - Enables also parallelizing a single loop over both teams and threads

::::::{.columns}
:::{.column}
```c++
#pragma omp target
#pragma omp teams distribute parallel for
for (int i = 0; i < N; i++) {
  p[i] = v1[i] * v2[i]
}
```
:::
:::{.column}
```fortranfree
!$omp target
!$omp teams distribute parallel do
do i = 1, N
  p(i) = v1(i) * v2(i)
end do
!$omp end teams distribute parallel do
!$omp end target
```
:::
::::::


# Collapsing loops

- The `collapse(n)` clause can be used to combine *n* nested loops to a single large loop

::::::{.columns}
:::{.column}
```c++
#pragma omp target
#pragma omp teams distribute parallel for \
            collapse(2)
for (int i = 0; i < N; i++)
  for (int j = 0; j < M; j++) {
    ...
  }
```
:::
:::{.column}
```fortranfree
!$omp target
!$omp teams distribute parallel do &
!$omp     collapse(2)
do i = 1, N
  do j = 1, M
    ...
  end do
end do
!$omp teams distribute parallel do
!$omp end target
```
:::
::::::


# Reduction

- The OpenMP `reduction` clause works also on offload code
  - Generates code that does reduction efficiently across teams and threads

# Example: Reduction

::::::{.columns}
:::{.column}
```c++
int n=1000;
double sum=0.0;
double x[n*n], y[n*n];
// some code to initialise x and y

#pragma omp target map(tofrom: sum)
#pragma omp teams distribute reduction(+:sum)
for (int i = 0; i < n; i++) {
    #pragma omp parallel for reduction(+:sum)
    for (int j = 0; j < n; j++) {
        sum += x[i*n + j] * y[i*n + j];
    }
}
```
:::
:::{.column}
```fortranfree
integer :: n = 1000
real(8) :: sum = 0.0
real(8), dimension(n, n) :: x, y
! some code to initialise x and y

!$omp target map(tofrom: sum)
!$omp teams distribute reduction(+:sum)
do j = 1, n
  !$omp parallel do reduction(+:sum)
  do i = 1, n
      sum = sum + x(i,j) * y(i,j)
  end do
  !$omp end parallel do
end do
!$omp end teams distribute
!$omp end target
```
:::
::::::




# Controlling data movement {.section}

# OpenMP data environment

- Variables are implicitly mapped to a target region unless explicitly defined in a data clause
  - Scalars as *firstprivate*, static arrays copied to/from device
- When dealing with a GPU device with separate memory, **optimizing data movement** is often **essential** to achieve good performance
  - Map data explicitly instead of relying on implicit mapping

# Example: Implicit mapping

::::::{.columns}
:::{.column}
```c++
int N=1000;
double a=3.14;
double x[N], y[N];
// some code to initialise x and y

#pragma omp target
#pragma omp teams distribute parallel for
for (int i=0; i < N; i++) {
    y[i] += a * x[i];
}
```
:::
:::{.column}
```fortranfree
integer :: N
real(8) :: a
real(8), dimension(N) :: x, y
! some code to initialise x and y

!$omp target
!$omp teams distribute parallel do
do i = 1, N
  y(i) = a * x(i) + y(i)
end do
!$omp end teams distribute parallel do
!$omp end target
```
:::
::::::


- Implicit copy of `a`, `x`, and `y` to the device when the `target` region is entered and back when it is exited

# Explicit mapping

**`target map(type: var1, var2, ...)`**

- Define data mapping for the associated target region
- The mapping `type` is one of:
  - `to`: copy data to device upon entry to the region
  - `from`: copy data from device upon exit from the region
  - `tofrom`: copy data to device upon entry and back on exit
  - `alloc`: allocate on the device (no copy, uninitialised)
- Mapping arrays:
  - C: `data[first index : size]`
  - Fortran: `data(first index : inclusive last index)`


# Example: Explicit mapping

::::::{.columns}
:::{.column}
```c++
int n=1000;
double a=3.14;
double x[n], y[n];
// some code to initialise x and y

#pragma omp target map(to: x[0:n]) \
                   map(tofrom: y[0:n])
#pragma omp teams distribute parallel for
for (int i=0; i < n; i++) {
    y[i] += a * x[i];
}
```
:::
:::{.column}
```fortranfree
integer :: n
real(8) :: a
real(8), dimension(n) :: x, y
! some code to initialise x and y

!$omp target map(to: x(1:n)) &
!$omp        map(tofrom: y(1:n))
!$omp teams distribute parallel do
do i = 1, n
  y(i) = a * x(i) + y(i)
end do
!$omp end teams distribute parallel do
!$omp end target
```
:::
::::::

- Both `x` and `y` are copied the device, but only `y` is copied back to the host


# Structured data region

**`target data map(type: var1, var2, ...)`**

- Define data mapping for a structured block that may contain multiple target regions
  - Only maps data, one still needs to define a target region to execute code on the device
- The mapping `type` is like in `target map`

# Example: Data mapping over multiple target regions

::::::{.columns}
:::{.column}
```c++
#pragma omp target data map(to: x[0:n])
{
    #pragma omp target map(tofrom: y[0:n])
    #pragma omp teams distribute parallel for
    for (int i=0; i < n; i++)
        y[i] += a * x[i];

    #pragma omp target map(tofrom: z[0:n])
    #pragma omp teams distribute parallel for
    for (int i=0; i < n; i++)
        z[i] += b * x[i];
}
```
:::
:::{.column}
```fortranfree
!$omp target data map(to: x(1:n))
  !$omp target map(tofrom: y(1:n))
  !$omp teams distribute parallel do
  do i = 1, n
    y(i) = a * x(i) + y(i)
  end do
  !$omp end teams distribute parallel do
  !$omp end target

  !$omp target map(tofrom: z(1:n))
  !$omp teams distribute parallel do
  do i = 1, n
    z(i) = b * x(i) + z(i)
  end do
  !$omp end teams distribute parallel do
  !$omp end target
!$omp end target data
```
:::
::::::


# Update

**`target update type(var1, var2, ...)`**

- Update a variable within a data region
  - A single line executable directive
- The `type` is one of:
  - `to`: copy data to device
  - `from`: copy data from device
- Useful for producing snapshots of the device variables on the host or for updating variables on the device


# Example: Update within a data region

::::::{.columns}
:::{.column}
```c++
#pragma omp target data map(to: x[0:n])
{
    #pragma omp target map(tofrom: y[0:n])
    #pragma omp parallel for
    for (int i=0; i < n; i++)
        y[i] += a * x[i];

    // ... some host code that modifies x ...
    #pragma omp target update to(x[0:n])

    #pragma omp target map(tofrom: z[0:n])
    #pragma omp parallel for
    for (int i=0; i < n; i++)
        z[i] += b * x[i];
}
```
:::
:::{.column}
```fortranfree
!$omp target data map(to: x(1:n))
  !$omp target map(tofrom: y(1:n))
  !$omp teams distribute parallel do
  do i = 1, n
    y(i) = a * x(i) + y(i)
  end do
  !$omp end teams distribute parallel do
  !$omp end target

  ! ... some host code that modifies x ...
  !$omp target update to(x(1:n))

  !$omp target map(tofrom: z(1:n))
  !$omp teams distribute parallel do
  do i = 1, n
    z(i) = b * x(i) + z(i)
  end do
...
```
:::
::::::



# Unstructured data regions

**`target enter data map(type: var1, var2, ...)`**

- The mapping `type` is one of:
  - `alloc`: allocate on the device
  - `to`: copy data to device

**`target exit data map(type: var1, var2, ...)`**

- The mapping `type` is one of:
  - `delete`: deallocate on the device
  - `from`: copy data from device

- Unstructured data regions enable handling cases where memory allocation and deallocation is done in a different scope


# Example: Unstructured data region

```c++
class Vector {
    Vector(int n) : len(n) {
        v = new double[len];
        #pragma omp target enter data map(alloc: v[0:len])
    }
    ~Vector() {
        #pragma omp target exit data map(delete: v[0:len])
        delete[] v;
    }
    double v;
    int len;
};
```


# Compiling and running {.section}

# Compiling an OpenMP offload program

- In addition to normal OpenMP options (*i.e.* `-fopenmp`), one needs to typically specify offload target (NVIDIA GPU, AMD GPU, ...)
- Compiler can also provide diagnostics
  - If and how loops were parallelized, what kind of data copies were added, ...
- See exercises

# Running an OpenMP offload program

- The compiled code can be executed as usual on supercomputer
- Often both CPU and GPU backends are compiled!
  - Set `OMP_TARGET_OFFLOAD=MANDATORY` before running to ensure that the code runs on GPU
- There are also *implementation-dependent* environment variables for runtime debug information
  - `CRAY_ACC_DEBUG`, `LIBOMPTARGET_INFO`, `NVCOMPILER_ACC_NOTIFY`, ...
- See exercises


# Summary {.section}
# Summary

- OpenMP provides directive-based programming of accelerators
  - C, C++, and Fortran supported
- Host--device model
  - Host offloads computations to the device
  - Device execution is synchronous with host by default in OpenMP offload
- Host and device may have separate memories
  - Host controls copying to/from the device
  - Memory transfers can happen also implictly in OpenMP offload
