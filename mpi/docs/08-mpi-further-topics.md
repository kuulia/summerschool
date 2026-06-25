<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title:  Further MPI topics
event:  CSC Summer School in High-Performance Computing 2026
lang:   en
---

# Recap / quiz

Menti!

# Outline

- Process topologies
  - Cartesian communicator
- User-defined datatypes
- Additional communication modes
  - Persistent communication
  - One-sided communication
- Recent additions to the MPI standard

# Process topologies {.section}

# Process topologies

- MPI topology mechanism adds additional information about the communication pattern to a communicator
- MPI topologies are virtual, *i.e.* they do not have necessarily relation to the physical structure of the supercomputer
- MPI topology may assist the library and the runtime system in optimizations
  - In most implementations main advantage is, however, better programmability
- Topologies are defined by creating special user defined communicators

# Example: Cartesian communicator

- Cartesian topology represents a regular multidimensional grid

```c
int ndims = 2;
int dims[ndims] = {0};
int periods[ndims] = {0};
int coords[ndims] = {0};

MPI_Dims_create(size, ndims, dims);
MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, periods, 1, &comm);
MPI_Comm_rank(comm, &rank);
MPI_Cart_coords(comm, rank, ndims, coords)
```

# User-defined datatypes {.section}

# User-defined datatypes

- User-defined datatypes enable communication of non-contiguous or heterogeneous data with single MPI communication operations
  - Improves code readability and portability
  - Allows optimizations by MPI at runtime
- MPI provides constructors for several specific types

# Example: Sending rows of matrix in Fortran

```fortranfree
integer, parameter :: n=2, m=3
real, dimension(n,m) :: a
type(mpi_datatype) :: rowtype
! create a derived type
call mpi_type_vector(m, 1, n, mpi_real, rowtype, ierr)
call mpi_type_commit(rowtype, ierr)
! send a row
call mpi_send(a, 1, rowtype, dest, tag, comm, ierr)
! free the type after it is not needed
call mpi_type_free(rowtype, ierr)
```

<p>
![](img/fortran-array-layout.png){.center width=50%}


# Additional communication modes {.section}

# Persistent communication

- Often a communication with same argument list is repeatedly executed
- Persistent communication enables opening a "communication port" for the repeated communication pattern
- This can provide optimization opportunities for MPI library
- Setup with `MPI_Send_init(...)` / `MPI_Recv_init(...)`, perform with `MPI_Start(...)` (nonblocking)

# One-sided communication

- In one-sided communication only a single process calls data movement functions - remote memory access (RMA)
  - Compare to "regular" communication where sends and receives of different processes need to match
- Always non-blocking
- Certain algorithms can be easier to implement with one-sided communication
- Can potentially reduce synchronization overheads and provide better performance especially on recent hardware


# Recent additions to the MPI standard {.section}

# Evolution of the MPI standard

- MPI is an evolving standard:
  - Version 5.0 in 2025, 4.0 in 2021, 3.0 in 2015
- New versions bring new features
- However, HPC systems usually lag behind and may provide older implementations only
  - LUMI default MPI follows version 3.1, but there is an unofficial installation that supports 4.1

# Long-count variants, since MPI-4.0

- Traditional `MPI_Send`, `MPI_Recv` have a subtle limit on how big messages it can handle: the `count` parameter is of (signed) `int` type!
  - `int` is 32-bit on practically all platforms => maximum message length is $2^{31} - 1 = 2,147,483,647$ elements
  - ~2 GB if sending `MPI_BYTE`, ~16 GB if sending `MPI_DOUBLE`
  - Workaround: divide very long messages into many smaller ones
- Modern MPI has separate **long-count variants:** `count` is of `MPI_Count` type (at least 64-bit)
  - Instead of `MPI_Send`, `MPI_Recv` etc. you would use `MPI_Send_c`, `MPI_Recv_c`, ...

# ABI compatibility, since MPI-5.0

- ABI = Application Binary Interface
- Standard now requires that different MPI implementations are ABI-compatible
- Program compiled and linked for OpenMPI will Just Work (tm) also with MPICH

# Summary {.section}

# Summary

- MPI is large standard and it has hundreds of procedures for different purposes
- MPI is an evolving standard and new features are introduced from time to time
  - Use with care: HPC systems may only provide older implementations
- See our extra material (slides, exercises)
