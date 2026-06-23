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

rm -f ../*.x *.x

CC="CC -O3 -Wall"
FT="ftn -O3"

#########################################################
# MPI only
#########################################################
for f in ../*.cpp; do
    $CC "$f" -o "${f%.cpp}.x"
done
for f in ../*.F90; do
    $FT "$f" -o "${f%.F90}-f.x"
done

for f in ../*.x; do
    srun -o $(basename "${f%.x}-ref.out") "$f"
done

#########################################################
# MPI+OpenMP
#########################################################
sed 's/int tag = tid;/int tag = 123;/g' multiple-p2p-thread-tags.cpp > multiple-p2p-random.cpp
sed 's/int tag = tid;/int tag = 123;/g' multiple-p2p-thread-tags.F90 > multiple-p2p-random.F90
for f in *.cpp; do
    $CC -fopenmp "$f" -o "${f%.cpp}.x"
done
for f in *.F90; do
    $FT -fopenmp "$f" -o "${f%.F90}-f.x"
done

for f in *.x; do
    srun -o "${f%.x}.out" "$f"
done
