<!--
SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Tools

Miscellaneous tools for the summerschool.

## Rsync script

The script [rsync_to_hpc.sh](rsync_to_hpc.sh) can be used for sending your local modifications to this repository to an
HPC system using [`rsync`](https://rsync.samba.org/). The purpose is to provide a streamlined code development workflow:
You can modify files (e.g. work on exercises) locally on your own laptop or workstation, and run the script to send your
changes to an HPC system for compilation and running.

In short:
- Your local `summerschool` repository becomes the "single source of truth". The script copies the state of your local
repository to an external HPC system.
- You write code using your favorite editor directly on your own machine, instead of modifying files on the HPC system
over SSH or a web interface. This way you are not affected by possible network or filesystem lag while coding.
- Running the script will send any modifications since the last sync to the HPC system. This is done using `rsync`,
which requires a working SSH setup. It should work without further configuration if you already have a working SSH
connection to the target system.

### Usage

The script needs to know your CSC username. Open the script and set `$REMOTE_USER` accordingly:
```bash
REMOTE_USER="myusername"
```
The script is configured to send your changes to a user-specific subdirectory under the summerschool project `scratch`
disk area:
- `REMOTE_ROOT="/scratch/project_462001452/$REMOTE_USER/rsync"` for LUMI
- `REMOTE_ROOT="/scratch/project_2019219/$REMOTE_USER/rsync"` for Mahti

You can run it as
```bash
./rsync_to_hpc.sh <target>
```
where `target` is either "lumi" or "mahti". You can also pass optional arguments that get forwarded to `rsync`, for
example a "dry run" (run without actually copying anything) could be done as
```bash
./rsync_to_hpc.sh --dry-run lumi
```
