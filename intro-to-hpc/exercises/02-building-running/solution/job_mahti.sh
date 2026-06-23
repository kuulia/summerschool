#!/bin/bash
#SBATCH --job-name=test
#SBATCH --account=project_2019219
#SBATCH --partition=small
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=1G
#SBATCH --time=00:05:00

# Run the program
srun ./code/prog.x
