#!/bin/bash

# SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

# Script for rsyncing projects from local to a remote HPC system,
# so that folder structure is preserved relative to a configurable "root" directory

if [[ "$1" != "lumi" && "$1" != "mahti" ]]; then
    echo "Usage: $0 [lumi|mahti]"
    exit 1
fi
TARGET_REMOTE="$1"

# --- Replace with your username ---
USER="laurinie"

# --- Local project root ---
LOCAL_ROOT="/home/$USER/training"

# --- Directories to sync (relative to LOCAL_ROOT). Will include subdirectories ---
SYNC_DIRS=(
    "summerschool"
)

# --- Remote config ---
if [[ "$TARGET_REMOTE" == "lumi" ]]; then
    REMOTE_HOST="lumi"
    REMOTE_ROOT="/scratch/project_462001452/$USER/rsync"
elif [[ "$TARGET_REMOTE" == "mahti" ]]; then
    REMOTE_HOST="mahti"
    REMOTE_ROOT="/scratch/project_2019219/$USER/gpaw_rsync"
fi

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Rsync common options:
# -a : Archive mode (preserves permissions, symlinks, etc.)
# -v : Verbose (prints which file is being synced)
# -z : Compress data during transfer
# --mkpath : Create missing directories on the remote
# --exclude : Ignore specified files/patterns
# --exclude-from : like --exclude but read the patterns from file

# See tools/.rsyncignore for our default excludes. For example, we don't rsync image files.
# Some more flags that you may find useful; add as necessary.
# -P : Show progress for each file
# --delete : Remove remote files that are not present locally
RSYNC_OPTS=(-avz --mkpath
    --exclude-from=".gitignore"
    --exclude-from="$SCRIPT_DIR/.rsyncignore"
)

# --- Sync each subdirectory ---
for SUBDIR in "${SYNC_DIRS[@]}"; do
    # Note: the trailing / is important for preserving directory structure
    LOCAL_PATH="$LOCAL_ROOT/$SUBDIR/"
    REMOTE_PATH="$REMOTE_HOST:$REMOTE_ROOT/$SUBDIR"

    echo ">>> Syncing $SUBDIR -> $REMOTE_HOST:$REMOTE_ROOT/$SUBDIR"

    rsync "${RSYNC_OPTS[@]}" "$LOCAL_PATH" "$REMOTE_PATH"

    if [[ $? -ne 0 ]]; then
        echo "ERROR: rsync failed for $SUBDIR" >&2
        exit 1
    fi
done

echo ">>> Done."
