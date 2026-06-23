<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Discussion

## Tasks

1. Environment loaded as described.

2. Output is as described.

3. Output is as described.

4. The sbatch creates an output file `slurm-JOBID.out` with the following contents

       Code was compiled with Cray compiler
       Hello Nuuksio from slurm process id 0/1 on node nid002453

5. The output with `--nodes=2 --ntasks-per-node=4`:

       Code was compiled with Cray compiler
       Hello Nuuksio from slurm process id 0/8 on node nid002471
       Code was compiled with Cray compiler
       Hello Nuuksio from slurm process id 1/8 on node nid002471
       Code was compiled with Cray compiler
       Hello Nuuksio from slurm process id 3/8 on node nid002471
       Code was compiled with Cray compiler
       Hello Nuuksio from slurm process id 2/8 on node nid002471
       Code was compiled with Cray compiler
       Hello Nuuksio from slurm process id 4/8 on node nid002472
       Code was compiled with Cray compiler
       Hello Nuuksio from slurm process id 7/8 on node nid002472
       Code was compiled with Cray compiler
       Hello Nuuksio from slurm process id 6/8 on node nid002472
       Code was compiled with Cray compiler
       Hello Nuuksio from slurm process id 5/8 on node nid002472


## Bonus tasks: Different compiler environment on LUMI

1. Environment loaded as described.

2. Output:

       rm -vf *.x *.o
       removed 'prog.x'
       removed 'prog.o'
       removed 'util.o'

3. Output displays now different compiler information:

       Code was compiled with GNU compiler
       Hello Nuuksio from slurm process id 0/1 on node nid002465


## Bonus tasks: Run on Mahti

1. Updated Makefile: `Makefile_mahti`.

   Updated job script: `job_mahti.sh`.
