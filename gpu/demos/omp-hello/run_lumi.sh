#!/bin/bash

# SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

#SBATCH --job-name=test
#SBATCH --partition=small-g
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH --gpus-per-node=1
#SBATCH --time=00:10:00

ml LUMI/25.03 partition/G rocm/6.3.4

set -xeuo pipefail

CC="CC -O3 -fopenmp -Wall"
FT="ftn -O3 -fopenmp"

for f in hello-*.cpp; do
    $CC "$f" -o "${f%.cpp}.x"
done

export CRAY_ACC_DEBUG=2
for f in hello-*.x; do
    srun -o "data/${f%.x}.out" "$f"
done
