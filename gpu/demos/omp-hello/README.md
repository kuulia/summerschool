<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Demos

## Hello

1. Use `module` command to preprare the environment for compiling GPU code,
   following the general instructions for [LUMI](../../../README_LUMI.md).

   Compile the code:

       CC -O3 -fopenmp -Wall hello.cpp -o hello.x

   Run the program:

       sbatch job.sh

2. Compile and run the different `hello-*.cpp` by `sbatch run_lumi.sh`.
