#!/bin/bash
#SBATCH --job-name=test
#SBATCH --account=project_462001452
#SBATCH --reservation=SummerSchoolCPU
#SBATCH --partition=small
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=2
#SBATCH --cpus-per-task=2
#SBATCH --mem-per-cpu=1G
#SBATCH --time=00:05:00

# Set the number of threads based on cpus-per-task
export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK:-1}
export OMP_DISPLAY_AFFINITY=true
export OMP_AFFINITY_FORMAT="Process %P level %L thread %0.4n/%0.4N on node %H core %A"

# Place and bind threads to single hardware threads
# Comment the following lines if binding is not desired
export OMP_PLACES=sockets
export OMP_PROC_BIND=spread

# Run the program
srun ./hello.x
