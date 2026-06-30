<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title:  Using MPI with OpenMP threads
event:  CSC Summer School in High-Performance Computing 2026
lang:   en
---

# Outline

- Using MPI+OpenMP

# Using MPI+OpenMP {.section}

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

# Hybrid programming: Launch threads (OpenMP) *within* processes (MPI)

<div class="column">
  - Shared memory programming inside a node, message passing between nodes
  - Matches well modern supercomputer hardware
  - Optimum MPI task per node ratio depends on the application and should always be experimented.
</div>

<div class="column">
![](img/supercomputer-node-hybrid.svg){.center width=80%}
</div>

# Potential advantages of the hybrid approach

- Fewer MPI processes for a given amount of cores
  - Improved load balance
  - All-to-all communication bottlenecks alleviated
  - Decreased memory consumption if implementation uses replicated data
- Additional parallelization levels may be available
- Possibility for dedicating threads to different tasks
  - e.g. a thread dedicated to communication or parallel I/O

# Thread support in MPI

![](img/mpi-thread-support.svg){.center width=80%}

- Modern MPI libraries support all threading levels

# Thread safe initialization

MPI_Init_thread(`required`{.input}, `provided`{.output})
  : Initializes the MPI execution environment
  : Note! C interface include command line arguments

<p>
- Thread safety levels `required` and `provided` are pre-defined integer constants:<br>
  `MPI_THREAD_SINGLE < MPI_THREAD_FUNNELED < `<br>`MPI_THREAD_SERIALIZED < MPI_THREAD_MULTIPLE`

# Example: Hybrid hello

<!-- Presentation suggestion: live coding for hybrid hello -->
<div class="column">
```c
#include <mpi.h>
#include <omp.h>

int main(int argc, char *argv[]) {
  int rank;
  int provided, required=MPI_THREAD_FUNNELED;
  MPI_Init_thread(&argc, &argv,
                  required, &provided);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  #pragma omp parallel
  {
    int thread_id = omp_get_thread_num();
    printf("I'm thread %d in process %d\n",
           thread_id, rank);
  }
  MPI_Finalize();
}
```
</div>
<div class="column">
```shell
$ cc -fopenmp hybrid-hello.c -o hybrid-hello
$ srun  --ntasks=2 --cpus-per-task=4
  ./hybrid-hello

I'm thread 0 in process 0
I'm thread 0 in process 1
I'm thread 2 in process 1
I'm thread 3 in process 1
I'm thread 1 in process 1
I'm thread 3 in process 0
I'm thread 1 in process 0
I'm thread 2 in process 0
```
</div>

# Checking the thread support level in the program

```cpp
MPI_Init_thread(&argc, &argv, required, &provided);

// Check that the MPI implementation supports the required level
if (provided < required) {
    printf("MPI does not support the required thread support level\n");
    MPI_Abort(MPI_COMM_WORLD, -1);
    return 0;
}
```

# Multiple thread communication

- Hybrid programming is relatively straightforward in cases where
  communication is done by only a single thread at a time
- In `MPI_THREAD_MULTIPLE` thread safety level all threads can make MPI calls independently
- When multiple threads communicate, the sending and receiving threads normally need to match
  - Use thread-specific tags or thread-specific communicators
- Note that using `MPI_THREAD_MULTIPLE` requires the MPI library to
  internally lock some data structures to avoid race conditions
  - May result in additional overhead in MPI calls



# Thread-specific tags

- In point-to-point communication the thread ID can be used to
  generate a tag that guides the messages to the correct thread

![](img/multiple-thread-communication.svg){.center width=60%}


# Thread-specific tags

- In point-to-point communication the thread ID can be used to
  generate a tag that guides the messages to the correct thread

```fortranfree
!$omp parallel private(tid, tidtag, ierr)
tid = omp_get_thread_num()
tidtag = 2**10 + tid

! mpi communication to the corresponding thread on another process
call mpi_sendrecv(senddata, n, mpi_real, pid, tidtag, &
                  recvdata, n, mpi_real, pid, tidtag, &
                  mpi_comm_world, stat, ierr)

!$omp end parallel
```

# Thread-specific communicators

```cpp
// Create thread-specific communicators
std::vector<MPI_Comm> mpi_comm_thread(omp_get_max_threads());
for (auto& comm : mpi_comm_thread) {
    MPI_Comm_dup(MPI_COMM_WORLD, &comm);
}

#pragma omp parallel
{
  ...
  MPI_Send(..., mpi_comm_thread[omp_get_thread_num()]);
  ...
}

// Free communicators
for (auto& comm : mpi_comm_thread) {
  MPI_Comm_free(&comm);
}
```


# Collective operations in the multiple mode

- MPI standard allows multiple threads to call collectives simultaneously
  - Programmer must ensure that the collective calls are correctly ordered
    if using the same communicator in the collective calls
  - Thread-specific communicators can be used also with collectives
- In most cases, even with `MPI_THREAD_MULTIPLE` it is often better to
  perform the collective communication from a single thread (usually the
  master thread)


# Summary {.section}

# Summary

- Multiple threads may make MPI calls simultaneously
- Use thread-specific tags and/or communicators
- For collectives it is often better to use a single thread for communication
