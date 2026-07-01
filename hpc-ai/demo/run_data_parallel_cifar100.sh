# SPDX-FileCopyrightText: 2026 Hossein Firooz (Aalto University) and CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

#!/bin/bash
#SBATCH --job-name=data_parallel_cifar100
#SBATCH --account=project_462001452
#SBATCH --partition=small-g
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=14
#SBATCH --gpus-per-node=2
#SBATCH --mem-per-gpu=60G
#SBATCH --time=0:30:00
#SBATCH --output=data_parallel_cifar100-%j.out
#SBATCH --reservation=SummerSchoolAI

module load Local-LAIF lumi-aif-singularity-bindings

# To use the latest
#SIF=/appl/local/laifs/containers/lumi-multitorch-latest.sif

# To use a specific release of the container
SIF=/appl/local/laifs/containers/lumi-multitorch-u24r70f21m50t210-20260513_121430/lumi-multitorch-full-u24r70f21m50t210-20260513_121430.sif

# Settings for user-specific MIOpen caches, otherwise there might be
# conflicts when multiple users are on the same nodes
export MIOPEN_USER_DB_PATH=/tmp/${USER}-miopen-cache
export MIOPEN_CUSTOM_CACHE_DIR=$MIOPEN_USER_DB_PATH

srun singularity run $SIF python train_data_parallel_cifar100.py 
