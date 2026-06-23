<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# General instructions for LUMI

## Logging in via ssh

You can access the [LUMI](https://docs.lumi-supercomputer.eu/) supercomputer with ssh:

    ssh <username>@lumi.csc.fi

If you don't have the ssh key in the default location, you need to give a path to the key file:

    ssh -i <path-to-private-key> <username>@lumi.csc.fi

See [wiki](../../wiki/Setting-up-CSC-account-and-SSH) for further details.

## First-time setup

All the exercises should be carried out in the scratch disk area.
This scratch area is shared between all the project members, so create a personal working directory there:

    mkdir -p /scratch/project_462001452/$USER
    cd /scratch/project_462001452/$USER

and clone the summer school git repository there:

    git clone https://github.com/csc-training/summerschool.git /scratch/project_462001452/$USER/summerschool

Now, `/scratch/project_462001452/$USER/summerschool` is your own clone of the summer school repository on LUMI
and you can modify files there without causing conflicts with other summer school participants.

After this initial setup, the files remain on the system and in the subsequent terminal sessions you can go
directly to your summershool clone:

    cd /scratch/project_462001452/$USER/summerschool

<details>
<summary>Optional: Working with your own git fork</summary>

**We don't have time to teach git during the summer school, so this is recommended only if you are already somewhat familiar with git.**

It is recommended to fork the summer school repository on github and clone your own fork to LUMI instead.
This allows you to push your exercise solutions to a branch on your own fork.

In order to push commits to your own fork on LUMI, you can use your existing key on your laptop by enabling ssh agent forwarding (`ssh -A <username>@lumi.csc.fi`) *or* to add an SSH public key generated on LUMI to your github account.

Note that the default editor for commit messages is *vim*, if you prefer something else you can add, e.g.,

    export EDITOR=nano

to the file `$HOME/.bashrc`.

</details>

## Editing code

You can use terminal-based code editors for modifying source code directly on LUMI, e.g. when working on exercises.
For instance, using `nano`:
    nano prog.F90

(`^` in nano's shortcuts refer to **Ctrl** key, *i.e.* in order to save the file and exit the editor press `Ctrl+X`)
Other available editors include *emacs* and *vim*.

Another valid approach is to do all your code editing locally on your own laptop and clone the changes to LUMI for
compilation and running. We provide a utility script, based on `rsync`, for simplifying this process:
- [The script itself](tools/rsync_to_hpc.sh)
- [Usage and documentation](tools/README.md)


## Web interface

- LUMI web interface can be accessed through <https://www.lumi.csc.fi>


## Compiling

### CPU programming

LUMI has several programming environments.

For CPU programming use:
```bash
module load LUMI/25.03 partition/C
```

#### MPI

Compilation of MPI programs in C, C++, and Fortran:
```bash
cc -O3 -Wall prog.c -o prog.x
CC -O3 -Wall prog.cpp -o prog.x
ftn -O3 prog.F90 -o prog.x
```

The wrapper commands include automatically all the flags needed for building MPI programs.

#### MPI+OpenMP and pure OpenMP (threading with CPUs)

Both pure OpenMP and hybrid MPI+OpenMP programs can be compiled with the same wrappers
by including `-fopenmp` flag:
```bash
cc -fopenmp -O3 -Wall prog.c -o prog.x
CC -fopenmp -O3 -Wall prog.cpp -o prog.x
ftn -fopenmp -O3 prog.F90 -o prog.x
```

#### HDF5

In order to use HDF5 in CSC supercomputers, you need the load the HDF5 module with MPI I/O support.
The appropriate module in Lumi:
```bash
module load cray-hdf5-parallel
```

No special flags are needed for compiling and linking, the compiler wrappers take care of them automatically when this module is loaded.


### GPU programming

LUMI has several programming environments.

For GPU programming use:
```bash
module load LUMI/25.03 partition/G rocm/6.3.4
```

#### HIP and MPI+HIP

Compilation of HIP and multi-GPU MPI+HIP programs:
```bash
CC -xhip -O3 prog.cpp -o prog.x
```

### OpenMP offload and MPI+OpenMP offload

The compilation command is the same as in the CPU case:
```bash
cc -fopenmp -O3 -Wall prog.c -o prog.x
CC -fopenmp -O3 -Wall prog.cpp -o prog.x
ftn -fopenmp -O3 prog.F90 -o prog.x
```

**Note!** This will generate OpenMP offload code when the appropriate
GPU modules are loaded (in particular `craype-accel-amd-gfx90a` that is
loaded by `partition/G`).
It's important to ensure that GPU code is generated as the compilation
command is the same in CPU and GPU cases.

To obtain compiler diagnostics:

```bash
cc -fopenmp -O3 -Wall -fsave-loopmark prog.c -o prog.x
CC -fopenmp -O3 -Wall -fsave-loopmark prog.cpp -o prog.x
ftn -fopenmp -O3 -hmsgs -hlist=m prog.F90 -o prog.x
```

See [HPE Cray Clang C and C++ Quick Reference (17.0.1)](https://support.hpe.com/hpesc/public/docDisplay?docId=dp00004439en_us)
and [ftn man pages](https://cpe.ext.hpe.com/docs/24.03/cce/man1/crayftn.1.html)
for further information.

Alternatively to Cray compilers above, the C and C++ codes can also be compiled with AMD Clang:
```bash
amdclang -fopenmp -O3 --offload-arch=gfx90a prog.c -o prog.x
amdclang++ -fopenmp -O3 --offload-arch=gfx90a prog.cpp -o prog.x
```


## Running

Programs need to be executed via the batch job system:
```bash
sbatch job.sh
```
The output of the job will be in the file `slurm-JOBID.out`. You can check the status of your jobs with `squeue --me` and kill possible hanging applications with `scancel JOBID`.

The file job script `job.sh` contains both the resource request (comment header lines starting with `#SBATCH`)
and the file is executed as a bash script in the allocation (lines starting with `#` are comments and ignored
during bash execution). Examples are provided below.

Note that you can override any of the options define in the `#SBATCH` header as command line options, for example:
```bash
sbatch --time=00:10:00 job.sh
```

### Slurm reservations

We have dedicated slurm reservations for the summer school:
1. `--reservation=SummerSchoolCPU`: Valid from 2026-06-23T10:00:00 until 2026-06-27T20:00:00 and gives access up to 4 nodes in `small` partition.
2. `--reservation=SummerSchoolGPU`: Valid from 2026-06-28T09:00:00 to 2026-06-30T20:00:00 and gives access up to 4 nodes in `small-g` partition.
3. `--reservation=SummerSchoolAI`: Valid from 2026-07-01T09:00:00 to 2026-07-01T16:00:00 and gives access up to 10 nodes in `small-g` partition.

The example job script below document how to use these reservations.
If your job is not within these requirements, you can remove the reservation line so that you can access
all the general partitions available on LUMI.


### CPU jobs

Example `job.sh` for running MPI+OpenMP program reserving 1 node, 4 tasks per node, and 2 CPU core per task, as well as 1 GB RAM per core, i.e., 8 CPU cores and 8 GB RAM within one node in total:

```bash
#!/bin/bash
#SBATCH --job-name=test
#SBATCH --account=project_462001452
#SBATCH --reservation=SummerSchoolCPU
#SBATCH --partition=small
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=4
#SBATCH --cpus-per-task=2
#SBATCH --mem-per-cpu=1G
#SBATCH --time=00:05:00

# Set the number of threads based on cpus-per-task
export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK:-1}

# Place and bind threads to single hardware threads
# Comment the following lines if binding is not desired
export OMP_PLACES=cores
export OMP_PROC_BIND=spread

# Run the program
srun ./prog.x
```

Rules of thumb for choosing the resources based on the parallelization type:
- MPI only: `--ntasks-per-node=<number_of_mpi_tasks>` and `--cpus-per-task=1`
- OpenMP only: `--ntasks-per-node=1` and `--cpus-per-task=<number_of_threads>`
- MPI+OpenMP: `--ntasks-per-node=<number_of_mpi_tasks>` and `--cpus-per-task=<number_of_threads_per_mpi_task>`

Note that other ways might be reasonable in some cases too.
Some of such cases will be discussed in the exercises.


### GPU jobs

Example `job.sh` for running a GPU program reserving 1 GPU (= 1 GCD of the AMD MI250X GPU):

```bash
#!/bin/bash
#SBATCH --job-name=test
#SBATCH --account=project_462001452
#SBATCH --reservation=SummerSchoolGPU
#SBATCH --partition=small-g
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH --gpus-per-node=1
#SBATCH --time=00:05:00

# Enable GPU-aware MPI by uncommenting the line below
#export MPICH_GPU_SUPPORT_ENABLED=1

# Run the program
srun ./prog.x
```

Note that this script allocates also 4 CPU cores per task.
These extra CPU cores are especially useful for OpenMP runtime.

For multi-GPU jobs using MPI:
- Change the number of MPI tasks and GPUs per node: `--ntasks-per-node=<number_of_mpi_tasks>` and `--gpus-per-node=<number_of_gpus>`
- Uncomment `export MPICH_GPU_SUPPORT_ENABLED=1` to enable GPU-aware MPI


### Interactive jobs

Requesting an allocation:

```bash
salloc --account=project_462001452 --partition=small --nodes=1 --ntasks-per-node=1 --cpus-per-task=4 --time=00:30:00
```

Once the allocation is ready, you'll get a new shell on the login node.
In this shell, `srun` will launch jobs within the allocation:
```bash
srun --ntasks-per-node=2 --cpus-per-task=2 ./prog.x
```


### Interactive shell on a compute node

Create an interactive shell on a compute node:

```bash
srun --job-name=cn-shell --account=project_462001452 --partition=small --nodes=1 --ntasks-per-node=1 --cpus-per-task=2 --time=01:00:00 --pty ./tools/launch-cn-shell.sh
```

The script [launch-cn-shell.sh](tools/launch-cn-shell.sh) launches a bash shell
with the environment cleaned so that submitting slurm jobs works from the shell
in the same way as from the login node.


## Resources

- [LUMI documentation](https://docs.lumi-supercomputer.eu/)
