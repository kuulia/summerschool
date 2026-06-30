<!--
SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->
# GPU programming exercises

## Sunday exercises

### Basics of HIP programming

- [Hello HIP API](02-kernels-hello-api)
- [Launching kernels](02-kernels-kernel-launch)
- [Kernel launch wrapper](02-kernels-kernel-launch-wrapper)
- [Using the API to query information](02-kernels-api-queries)
- [Catching API errors](02-kernels-api-errors)
- [Fill kernel](02-kernels-fill)
- [Taylor for](02-kernels-taylor-for)

## Monday exercises

### Streams, events, and synchronization

- [Basics of streams](04-streams-basics)
- [Asynchronous kernels](04-streams-asynckernel)
- [Asynchronous memory copy](04-streams-asyncmemcopy)
- [Timing GPU execution with events](04-streams-timings)
- [Bonus: Overlapping CPU and GPU execution](04-bonus-cpu-gpu-overlap)

### Memory management

- [Explicit vs. implicit memory management](05-memory-explicit-implicit)
- [Minimizing memory allocations](05-memory-singlecopy)
- [Using mempools](05-memory-mempools)

### Kernel optimizations

- [Investigate coalescing](06-optimization-coalescing)
- [Tracing with rocprof](06-optimization-trace)
- [Optimize histogram calculation](06-optimization-histogram)
- [Optimize reductions](06-optimization-reductions)

## Tuesday exercises

### OpenMP offload

- [Hello world](07a-omp-hello)
- [Calculating axpy](07b-omp-axpy)
- [Parallel sum](07c-omp-reduction-sum)
- [Interoperability with HIP](07d-omp-hip-interop)
- [Device functions](07e-omp-device-functions)
- [(Bonus) Solving 2D Poisson's equation with Jacobi iteration](07f-omp-poisson)
- [(Bonus) Heat equation](07g-omp-heat-equation)

### Multi-GPU programming

- [Hello world](09-multi-gpu-hello)
- [Ping-pong](09-multi-gpu-ping-pong)
- [Overlapping computation and communication](09-multi-gpu-overlap)
- [Bonus: Multi-GPU heat equation](09-multi-gpu-heat-equation)

## Bonus exercises

- [Basics of HIP: Debugging & fixing errors](02-kernels-kernel-errors)
- [Basics of HIP: 2D Copy with double for loop](02-kernels-copy2d)
- [Optimization: Matrix transpose](06-optimization-matrix-transpose)
- [Multi-GPU heat equation](09-multi-gpu-heat-equation)


