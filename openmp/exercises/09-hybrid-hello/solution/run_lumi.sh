#!/bin/bash

# SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

#SBATCH --job-name=test
#SBATCH --partition=debug
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=3
#SBATCH --cpus-per-task=4
#SBATCH --time=00:10:00

set -xeuo pipefail

CC="CC -O3 -Wall"
FT="ftn -O3"

$CC -fopenmp hello-1.cpp -o hello.x
srun -o data/hello-1.out ./hello.x

$CC -fopenmp hello-2.cpp -o hello.x
srun -o data/hello-2.out ./hello.x


$FT -fopenmp hello-1.F90 -o hello.x
srun -o data/hello-f-1.out ./hello.x

$FT -fopenmp hello-2.F90 -o hello.x
srun -o data/hello-f-2.out ./hello.x
