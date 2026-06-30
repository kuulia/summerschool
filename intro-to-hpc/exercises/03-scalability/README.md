<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Exercise: Scalability analysis

In this exercise we practise performing a scalability analysis of a simulation code.

We analyse the scalability provided three-dimensional heat equation solver under [heat-equation-3d](heat-equation-3d)
by running the code with different node counts and collect the data systematically in the table below:

| Nodes | Runtime (s) | Resource cost (Node-s) | Speedup | Parallel efficiency | Electricity cost (Wh) | Monetary cost (€) |
| ----: | ----------: | ---------------------: | ------: | ------------------: | ---------------------: | ----------------: |
|    1  |             |                        |         |                     |                        |                   |
|    2  |             |                        |         |                     |                        |                   |
|    4  |             |                        |         |                     |                        |                   |
|    8  |             |                        |         |                     |                        |                   |
|   16  |             |                        |         |                     |                        |                   |

The columns are calculated from the node count and the runtime as follows
($T_1$ is the runtime with a single node and $T_n$ is the runtime with $n$ nodes):

- Resource cost is defined as

```math
n \times T_n
```

- Speed up is defined as

```math
S_n = \frac{T_1}{T_n}
```

- Parallel efficiency is defined as

```math
E_n = \frac{T_1}{n T_n}
```

- For electricity cost, one can be estimate that a single CPU node consumes 600 W.
    - Note that this is only the power used by the CPUs, and does not include cooling, interconnect network etc.

- For monetary cost, use typically commercial HPC cloud pricing of 4 € / node hour for LUMI type CPUs

As a rule of thumb, parallel efficiencies higher than 75% are usually considered reasonable.
This corresponds to a speedup of 1.5 when doubling the number nodes or to a speedup of 3 when quadrupling and so on.

The [solution directory](solution/) contains discussion on the tasks below.


## Tasks: Scalability across nodes

1. Ensure that you have loaded the recommended CPU compilation environment.

2. Build the provided code using the [Makefile](heat-equation-3d/Makefile).
   For LUMI, the correct compiler is activated with:

       make PLATFORM=lumi

   For Mahti, the correct compiler is activated with:

       make PLATFORM=mahti

3. Run the program `heat3d` with a different number of **full nodes**.

   You can use the provided example `job.sh`.

   Note 1: The script requests full LUMI-C nodes by setting
   `--exclusive` and `--ntasks-per-node=128`.

   Note 2: The script sets output to `slurm-%x-%J.out`, where
   `%x` will be replaced by the job name and `%J` with the job id.
   This is useful in combination with the protip below.

   Protip: you don't need to edit the `job.sh` for different node counts,
   but you can override the sbatch parameters through equivalent command line options.
   For example:

       sbatch --job-name=n01 --nodes=1 job.sh
       sbatch --job-name=n02 --nodes=2 job.sh
       ...

4. Collect the obtained runtimes in the table above and calculate
   the resource cost, speedup, and parallel efficiency for each row.

5. Which node count would you suggest to use for running this code with the used input settings?
