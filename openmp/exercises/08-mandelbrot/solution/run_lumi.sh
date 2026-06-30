#!/bin/bash

# SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

#SBATCH --job-name=test
#SBATCH --partition=debug
#SBATCH --exclusive
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=128
#SBATCH --mem=0
#SBATCH --time=00:10:00

set -xeuo pipefail

rm -f ../*.x *.x

CC="CC -O3 -Wall"
FT="ftn -O3 -hipa0"

#########################################################
# Serial
#########################################################
for f in ../*.cpp; do
    $CC "$f" -o "${f%.cpp}.x"
done
for f in ../*.F90; do
    [[ $(basename "$f") == "fake_omp.F90" ]] && continue
    $FT "$f" -o "${f%.F90}-f.x"
done

for f in ../*.x; do
    p=$([[ $f == *adaptive* ]] && echo 14 || echo 12)
    srun --cpus-per-task=1 -o data/$(basename "${f%.x}-serial.out") "$f" $p
done


#########################################################
# OpenMP
#########################################################
sed 's/schedule(dynamic)/collapse(2) schedule(dynamic)/g' mandelbrot-direct.cpp  > mandelbrot-direct-collapse2.cpp
sed 's/schedule(dynamic)/schedule(static)/g' mandelbrot-blocked.cpp > mandelbrot-blocked-static.cpp
sed 's/schedule(dynamic)/collapse(2) schedule(dynamic)/g' mandelbrot-direct.F90  > mandelbrot-direct-collapse2.F90
sed 's/schedule(dynamic)/schedule(static)/g' mandelbrot-blocked.F90 > mandelbrot-blocked-static.F90
for f in *.cpp; do
    $CC -fopenmp "$f" -o "${f%.cpp}.x"
done
for f in *.F90; do
    $FT -fopenmp "$f" -o "${f%.F90}-f.x"
done

for f in *.x; do
    p=$([[ $f == *adaptive* ]] && echo 14 || echo 12)
    for c in 1 2 4 8 16 32; do
        for i in {1..2}; do
            srun --cpus-per-task=$c -o $(printf "data/%s-t%03d-run%02d.out" "${f%.x}" "$c" "$i") "$f" $p
        done
    done
done
