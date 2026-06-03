<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

## Message chain

See [the earlier message chain exercise](../message-chain) for the description of the message chain.

### Message chain with persistent communication

1. Implement the program using persistent communication, *i.e.*
   `MPI_Send_init`, `MPI_Recv_init`, `MPI_Start` and `MPI_Wait`.
   You may start from scratch, or use the skeleton code or
   your solution from [the earlier message chain exercise](../message-chain)
   as a starting point.

2. Write a version that uses `MPI_Startall` and `MPI_Waitall` instead of doing multiple calls to `MPI_Start` and `MPI_Wait`.
