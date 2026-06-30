#!/bin/bash
#SBATCH --job-name=test
#SBATCH --account=project_462001452
#SBATCH --partition=small-g
#SBATCH --reservation=SummerSchoolGPU
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=2
#SBATCH --cpus-per-task=4
#SBATCH --gpus-per-node=2
#SBATCH --time=00:01:00

# Enable GPU-aware MPI by uncommenting the line below
export MPICH_GPU_SUPPORT_ENABLED=1
export ROCR_VISIBLE_DEVICES=$SLURM_LOCALID
exec $*

# Run the program
module load LUMI/25.03 partition/G rocm/6.3.4
CC -xhip -O3 main.cpp -o main
srun ./main

