#!/bin/bash

# SPDX-FileCopyrightText: 2026 Hossein Firooz (Aalto University) and CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

#SBATCH --account=project_462001452
#SBATCH --partition=small-g
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=7
#SBATCH --gpus-per-task=1
#SBATCH --mem=60G
#SBATCH --time=0:30:00
#SBATCH --output=run_model_parameters-%j.out
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

# Create Python venv and add fvcore package, if it doesn't already exist
if [ ! -f venv/bin/activate ]; then
    singularity run $SIF bash -c 'python -m venv venv --system-site-packages && source venv/bin/activate && pip install fvcore'
fi

srun singularity run $SIF bash -c 'source venv/bin/activate && python get_parameters.py'
