#!/bin/bash
#SBATCH --job-name=hello
#SBATCH --partition=small-g
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH --gpus-per-node=1
#SBATCH --time=00:10:00

export CRAY_ACC_DEBUG=2
srun ./hello.x
