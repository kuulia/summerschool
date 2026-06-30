<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title:  Process and thread affinity
event:  CSC Summer School in High-Performance Computing 2026
lang:   en
---

# Outline

- Process and thread affinity

# Process and thread affinity {.section}

# Non-uniform memory access

<div class=column>
- A node can have multiple sockets with memory attached to each socket
   - There can be memory hierarchy also within a socket
- Non-Uniform Memory Access (NUMA)
  - All memory within a node is accessible, but latencies and bandwidths vary
  - Query configuration with<br>`numactl --hardware`
</div>

<div class=column>
![](img/mahti-schematic.svg){.center width=100%}
</div>

# First touch policy

- Operating system typically optimizes memory allocations
  - `malloc()` does not allocate the memory directly, but at first memory access (write), the operating system physically allocates the corresponding page (first touch policy)
- To avoid performance loss: Initializion of data should be done on the core that will be later using this data

# NUMA aware initialization

<div class=column>
- No NUMA awareness

```c
// Initialize data
for (int i=0; i < N; i++)
   data[i] = ...
...
// Perform work
#pragma omp parallel for
for (int i=0; i < N; i++)
   process(data[i])
```
</div>

<div class=column>
![](img/first-touch-bad.svg){.center width=100%}
</div>

- All the data is physically on NUMA domain 0
- Slow access from cores on other NUMA domains

# NUMA aware initialization

<div class=column>
- With NUMA awareness

```c
// Initialize data
#pragma omp parallel for
for (int i=0; i < N; i++)
   data[i] = ...
...
// Perform work
#pragma omp parallel for
for (int i=0; i < N; i++)
   process(data[i])
```
</div>

<div class=column>
![](img/first-touch-good.svg){.center width=100%}
</div>

- Data is distributed to different physical NUMA domains
- Fast access when cores use local data

# Thread and process affinity

- Normally, operating system can run threads and processes in any logical core
- Operating system may even move the running process/thread from one core to another
    - Can be beneficial for load balancing
    - For HPC workloads often detrimental as private caches get
      invalidated and NUMA locality is lost
- User can control where tasks are run via affinity masks
    - Task can be *pinned* to a specific logical core or a set of logical cores


# Controlling process affinity

- Affinity for a *process* can be set with the `numactl` command
    - Limit the process to logical cores 0,3,7:
      <br><span style="padding-left:2em">
      `numactl --physcpubind=0,3,7 ./my_exe`
      </span>
    - Threads inherit the affinity of their parent process
- On LUMI, process affinity can be set by using SLURM options
    - `--distribution={nodes}:{sockets}:{cores}`
        - `{nodes|sockets|cores} ∈ [block, cyclic, plane=<size>, ...]`
    - e.g. `--distribution=block:cyclic:fcyclic`


# Controlling thread affinity

- Affinity of a thread can be set with OpenMP environment variables
    - `OMP_PROC_BIND=true/false|auto|spread|close|master`
        - e.g. `export OMP_PROC_BIND=spread`
    - `OMP_PLACES=threads|cores|sockets`
        - e.g. `export OMP_PLACES=cores`
- OpenMP runtime prints the affinity with `OMP_DISPLAY_AFFINITY=true`

- Useful online resources:
    - LUMI User Guide:<br>
      <span style="font-size:0.72em">
      <https://docs.lumi-supercomputer.eu/runjobs/scheduled-jobs/distribution-binding/>
      </span>
    - Frontier User Guide:<br>
      <span style="font-size:0.72em">
      <https://docs.olcf.ornl.gov/systems/frontier_user_guide.html#process-and-thread-mapping-examples>
      </span>


# Controlling affinity

```bash
export OMP_AFFINITY_FORMAT="Thread %0.3n affinity %A"
export OMP_DISPLAY_AFFINITY=true
./test
Thread 000 affinity 0-7
Thread 001 affinity 0-7
Thread 002 affinity 0-7
Thread 003 affinity 0-7
```

```bash
OMP_PLACES=cores ./test
Thread 000 affinity 0,4
Thread 001 affinity 1,5
Thread 002 affinity 2,6
Thread 003 affinity 3,7
```


# MPI+OpenMP thread affinity

<div class="column">
- MPI library must be aware of the underlying OpenMP for correct
  allocation of resources
    - Oversubscription of CPU cores may cause significant performance
      penalty
- Additional complexity from batch job schedulers
- Heavily dependent on the platform used!
</div>

<div class="column">
![](img/affinity.svg){.center width=70%}
</div>


# Slurm configuration at CSC

- Within a node, `--tasks-per-node` MPI tasks are spread
  `--cpus-per-task` apart
- Threads within a MPI tasks have the affinity mask for the
  corresponding `--cpus-per-task` cores
  ```bash
  export OMP_AFFINITY_FORMAT="Process %P thread %0.3n affinity %A"
  export OMP_DISPLAY_AFFINITY=true
  srun ... --tasks-per-node=2 --cpus-per-task=4 ./test
  Process 250545 thread 000 affinity 0-3
  ...
  Process 250546 thread 000 affinity 4-7
  ...
  ```
- Careful: Slurm configurations in other HPC centers can be very different


# Summary {.section}

# Summary

- NUMA (non-uniform memory access) aware initialization is important for multithreaded applications
- Performance of HPC applications is often improved when processes and
  threads are pinned to CPU cores
- MPI and batch system configurations may affect the affinity
    - Very system dependent, try to always investigate


