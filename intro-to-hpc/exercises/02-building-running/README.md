<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Exercise: Building and running on a supercomputer

In this exercise we practise editing files, loading modules, compiling code, and submitting jobs.

An example minimal code project is provided in the directory `code`.

The [solution directory](solution/) contains discussion on the tasks below.

## Tasks

1. Use `module` command to preprare the environment for compiling CPU code,
   following the general instructions for [LUMI](../../../README_LUMI.md).

2. Build the example code in the code directory with `make`:

       cd code
       make

   and observe the output:

       CC -O3 -Wall -c prog.cpp
       CC -O3 -Wall -c util.cpp
       CC -O3 -Wall prog.o util.o -o prog.x

   These are the commands that `make` executed in order to compile and link the code.

3. Modify the file `util.cpp` by replacing `Hello world` with `Hello Nuuksio`.
   You can edit the file using, e.g., `nano` or any other editor of your choice.

   Run make again and observe the output:

       CC -O3 -Wall -c util.cpp
       CC -O3 -Wall prog.o util.o -o prog.x

   Note that file `prog.cpp` was not compiled again, because it hadn't changed since the previous compilation.
   This is the central idea of make and it can save a lot of time when developing larger code bases.

4. Run the resulting executable `prog.x` via batch system by using the example batch job script `job.sh`:

       sbatch job.sh

5. Run the executable with different number of nodes and tasks per node. You can do this by editing `job.sh`, but here is a protip:
   You can override the sbatch parameters through equivalent command line options. For example:

       sbatch --nodes=2 --ntasks-per-node=4 job.sh


## Bonus tasks: Different compiler environment on LUMI

1. Swap the default Cray compiler environment to GNU compiler environment:

       module load PrgEnv-gnu

2. Clean the existing to compilation to ensure that the whole project is recompiled:

       make clean

3. Recompile the project using `make` and submit the job using `sbatch` as above.
   Do you observe any differences in the output?


## Bonus tasks: Run on Mahti

1. Repeat the tasks above for Mahti, following
   the general instructions for [Mahti](../../../README_Mahti.md).

   Note that you will need to modify the Makefile for the correct
   compiler command for Mahti and adapt the job script for Mahti.
