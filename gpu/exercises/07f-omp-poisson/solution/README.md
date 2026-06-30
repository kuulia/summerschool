<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Discussion

## Tasks

1. The serial code with `./poisson.x 1024 2000 3` runs in 0.996 seconds on LUMI-C.
   A larger case `./poisson.x 4096 2000 3` takes 113.478 seconds.

2. See `poisson.{c,F90}` that implements all the listed ideas.
   Now `./poisson.x 4096 2000 3` runs in 0.933 seconds on LUMI-G, more than 100x speedup.
