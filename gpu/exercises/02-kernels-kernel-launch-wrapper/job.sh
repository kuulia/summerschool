#!/bin/bash
#SBATCH --job-name=test
#SBATCH --account=project_462001452
#SBATCH --partition=dev-g
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH --gpus-per-node=1
#SBATCH --time=00:01:00

# Enable GPU-aware MPI by uncommenting the line below
#export MPICH_GPU_SUPPORT_ENABLED=1

# Run the program
srun ./main 10 1025

