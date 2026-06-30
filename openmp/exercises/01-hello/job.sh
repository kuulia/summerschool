#!/bin/bash
#SBATCH --job-name=test
#SBATCH --account=project_462001452
#SBATCH --reservation=SummerSchoolCPU
#SBATCH --partition=small
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=1G
#SBATCH --time=00:05:00

# Set number of threads for OpenMP. Usually we want this to match the
# --cpus-per-task Slurm option, which gets stored in the SLURM_CPUS_PER_TASK
# environment variable. The following bash syntax sets OMP_NUM_THREADS to 1
# if SLURM_CPUS_PER_TASK was not defined (ie. --cpus-per-task was not given)
export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK:-1}

# Run the program
srun ./hello.x
