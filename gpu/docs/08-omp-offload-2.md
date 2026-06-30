<!--
SPDX-FileCopyrightText: 2021 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title:  "OpenMP offload for GPU programming: Part 2"
event:  CSC Summer School in High-Performance Computing 2026
lang:   en
---

# Outline

- Asynchronous device execution in OpenMP offload
- Combining OpenMP offload with CUDA/HIP
- Device functions and variables

# Asynchronous execution {.section}

# Synchronous vs asynchronous execution

- By default, execution in the host continues only after the target region has finished
  - CPU is idling until the GPU has finished
- With the `nowait` clause, the host thread continues immediately after the work has been submitted to the device
  - Explicit synchronization needed on host with `taskwait`
- Benefits of asynchronous execution
  - Frees up the host CPU to perform other tasks while the device executes the offloaded region
  - Reduces the latency of kernel launches, which is especially useful for short kernels or pipelines or iterative workloads


# Example

:::{.column}
```c++
// Launch kernels asynchronously
#pragma omp target nowait
{  ...
   x[i] = a * u[i];
}

#pragma omp target nowait
{  ...
   y[i] = b * v[i];
}

// Wait all kernels to finish
#pragma omp taskwait
```
:::
:::{.column}
```fortranfree
! Launch kernels asynchronously
!$omp target nowait
   ...
   x(i) = a * u(i)
!$omp end target

!$omp target nowait
   ...
   y(i) = b * v(i)
!$omp end target

! Wait all kernels to finish
!$omp taskwait
```
:::


# Controlling execution order

- If the kernels are sufficiently small, the device might execute multiple of them simultaneously
  - Similarly to using multiple streams in CUDA/HIP
- The `depend` clause can be used to specify constraints on the execution order
  - Same way as with OpenMP tasks

# Example

:::{.column}
```c++
// Launch kernels asynchronously
#pragma omp target nowait depend(out: x[0:n])
{  ...
   x[i] = a * u[i];
}

#pragma omp target nowait
{  ...
   y[i] = b * v[i];
}

#pragma omp target nowait depend(in: x[0:n])
{  ...
   z[i] = c * x[i];
}

// Wait all kernels to finish
#pragma omp taskwait
```
:::
:::{.column}
```fortranfree
! Launch kernels asynchronously
!$omp target nowait depend(out: x(1:n))
   ...
   x(i) = a * u(i)
!$omp end target

!$omp target nowait
   ...
   y(i) = b * v(i)
!$omp end target

!$omp target nowait depend(in: x(1:n))
   ...
   z(i) = c * x(i)
!$omp end target

! Wait all kernels to finish
!$omp taskwait
```
:::


# Combining with OpenMP tasks

:::{.column}
```c++
#pragma omp parallel  // Create host threads
#pragma omp single
{
  #pragma omp target nowait depend(...)
  { ... }

  #pragma omp target nowait depend(...)
  { ... }

  #pragma omp task depend(...)
  { ... }

  // Wait all kernels and host tasks to finish
  #pragma omp taskwait

} // end of host threads
```
:::
:::{.column}
```fortranfree
!$omp parallel  ! Create host threads
!$omp single
  !$omp target nowait depend(...)
    ...
  !$omp end target

  !$omp target nowait depend(...)
    ...
  !$omp end target

  !$omp task depend(...)
    ...
  !$omp end task

  ! Wait all kernels and host tasks to finish
  !$omp taskwait
!$omp end single
!$omp end parallel
```
:::



# Combining OpenMP offload with CUDA/HIP {.section}

# Interoperability with CUDA/HIP and libraries

- OpenMP offload code can be integrated with CUDA/HIP kernels or GPU libraries
- Mixing OpenMP and CUDA/HIP, for example:
  - Use OpenMP for memory management
  - Introduce OpenMP in existing GPU code
  - Use CUDA/HIP for most critical kernels, otherwise OpenMP
- Calling GPU libraries: cublas/hipblas, cufft/hipfft, ...
- Using MPI: GPU-aware MPI libraries can do GPU-to-GPU memory transfer without going through host

# Device data interoperability

- OpenMP includes methods to access the device data pointers on the host side
- Device data pointers can be used to interoperate with libraries and
  other programming techniques available for accelerator devices

# Data constructs: `use_device_ptr` and `use_device_addr`

- **`target data use_device_ptr(var1, var2, ...)`**
  - Within the construct, all the listed pointer variables correspond to the device addresses
  - Use in C for pointers or in Fortran for `c_ptr`

- **`target data use_device_addr(var1, var2, ...)`**
  - Similar construct for non-pointer variables
  - Use for arrays in Fortran

- See detailed description in the OpenMP standard

# Example: Calling CUDA/HIP kernel

```cpp
#pragma omp target data map(to: x[0:n]) \
                        map(tofrom: y[0:n])
{
  #pragma omp target data use_device_ptr(x, y)
  {
    // Execute kernel on device
    axpy<<<griddim, blockdim>>(alpha, x, y, n)
  }
  // Wait for device to finish
  hipDeviceSynchronize();
}
// Calculated y is now available on host
...

```

- The `iso_c_binding` module can be used to build a Fortran interface to such C library

# Example: Calling hipblas

```cpp
#pragma omp target data map(to: x[0:n]) \
                        map(tofrom: y[0:n])
{
  #pragma omp target data use_device_ptr(x, y)
  {
    // Execute kernel on device
    hipblasDaxpy(handle, n, &alpha, x, 1, y, 1);
  }
  // Wait for device to finish
  hipDeviceSynchronize();
}
// Calculated y is now available on host
...
```

- The `iso_c_binding` module can be used to build a Fortran interface to such C library


# Alternative for `use_device_ptr`: `omp_get_mapped_ptr`

- If host pointer `x` has been mapped to device with OpenMP,
  we can get the corresponding device pointer `d_x` by
  ```cpp
  void* d_x = omp_get_mapped_ptr(x, omp_get_device_num())
  ```

# Existing device pointers: `is_device_ptr`

- If `d_x` is already a device pointer (e.g., allocated outside OpenMP),
  we can instruct OpenMP to use that pointer instead doing mapping:
  ```cpp
  #pragma omp target ... is_device_ptr(d_x)
  ```

# Device functions and variables {.section}

# Device function and variables

- The `declare target` tells the compiler that certain functions or variables should be compiled for the device (not just the host CPU)
  - Analogous to `__device__` in CUDA/HIP

# Example: Device functions in C++

::::::{.columns}
:::{.column}
`main.cpp`:

```cpp
#pragma omp target
#pragma omp teams distribute parallel for
for (int i = 0; i < n; i++) {
  z[i] = fun(x[i], y[i]);
}
```
:::
:::{.column}
`kernels.cpp`:

```cpp
#pragma omp declare target
double fun(double x, double y) {
  return x + y;
}
#pragma omp end declare target
```
:::
::::::

# Example: Device functions in Fortran

::::::{.columns}
:::{.column}
`main.F90`:

```fortranfree
!$omp target
!$omp teams distribute parallel do
do i = 1, n
  z(i) = fun(x(i), y(i))
end do
!$omp end teams distribute parallel do
!$omp end target
```
:::
:::{.column}
`kernels.F90`:

```fortranfree
module kernels
  implicit none
contains

  pure function fun(x, y) result(res)
    real(8), intent(in) :: x, y
    real(8) :: res
    !$omp declare target

    res = x + y
  end function fun

end module kernels
```
:::
::::::




# Summary {.section}
# Summary

- Asynchronous device execution is possible, but requires explicit synchronization
- OpenMP programs can work in conjunction with CUDA/HIP kernels and GPU libraries
- Key constructs for interoperability:
  - `use_device_ptr`: get the device pointer for a data mapping managed by OpenMP
  - `is_device_ptr`: use a non-OpenMP-managed device pointer for the device execution
