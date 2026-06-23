<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Discussion

*Note for instructors: use `run*.sh` scripts to generate all the output files.*

## Tasks

1. Code compiles and runs without issues.

2. See `axpy.{cpp,F90}`.
   See [data](data/) directory for outputs.

3. See `axpy-nomap.{cpp,F90}` and `axpy-nomap-nopointer.cpp` for codes with missing
   data mapping clauses.
   See [data](data/) directory for outputs.

   The Fortran code still works as the Fortran array is aware of its size and hence
   OpenMP can do the data transfers correctly.

   The C++ codes do not work without explicit mapping as OpenMP sees only a pointer
   and cannot know how large data the pointer is referring to.

4. See `axpy-full.{cpp,F90}`.
   See [data](data/) directory for outputs.
   Note in particular the data transfers.

5. See `*.lst` files in [data](data/) directory.

6. See `axpy-full-unstructured.{cpp,F90}`.


### Bonus tasks: Offload to CPU threads

The code compiles and runs without issues on with CPU target too.
