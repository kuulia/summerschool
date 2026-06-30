<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Discussion

## Task: Fix the race condition

1. Output:

       Array size: 100000
       Sum: 0.000000
       Calculation took 312.749 milliseconds

2. See `sum.{cpp,F90}`. Output is now correct:

       Array size: 100000
       Sum: 1.812028
       Calculation took 329.269 milliseconds
