#!/bin/bash
#SBATCH --job-name=scalability
#SBATCH --account=project_462001452
#SBATCH --partition=standard
#SBATCH --exclusive
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=128
#SBATCH --cpus-per-task=1
#SBATCH --mem=0
#SBATCH --time=00:10:00
#SBATCH --output=slurm-%x-%J.out

# Run the program
srun ./heat-equation-3d/build/heat3d 960 960 960 300
