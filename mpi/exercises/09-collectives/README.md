<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

## Collective operations

In this exercise we experiment with common collective-communication patterns.

Write a program for four MPI tasks. Each task should have a data vector with
the values initialised as follows:

![](img/sendbuffer.png)

In addition, each task has a receive buffer for eight elements and the
values in the buffer are initialized to -1.

Implement communication that sends and receives values from the data
vectors to the receive buffers so that the receive buffers will have the following values.
**Each case is doable with a single collective routine**.
You can start from scratch or use the skeleton code
[skeleton.cpp](skeleton.cpp) or [skeleton.F90](skeleton.F90).

### Case 1

![](img/bcast.png)

### Case 2

![](img/scatter.png)

### Case 3

![](img/gatherv.png)

### Case 4

![](img/alltoall.png)

