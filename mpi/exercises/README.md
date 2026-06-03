<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# MPI exercises

## Wednesday exercises

### Introduction to MPI

- [Hello world](01-hello-world/)

### Point-to-point communication I

- [Message exchange](02-message-exchange/)
- [Parallel pi with two processes](03-parallel-pi-two-procs/)

### Point-to-point communication II

- [Message length](04-message-length/)
- [Message chain](05-message-chain/)
- [Parallel pi with any number of processes](06-parallel-pi-general/)
- [Heat equation solver](heat-equation/): Tasks 1-2

### Non-blocking communication

- [Non-blocking message chain](07-message-chain-non-blocking/)
- [Heat equation solver](heat-equation/): Task 3

## Thursday exercises

### Collective operations

- [Broadcast and scatter](08-broadcast-scatter/)
- [(Bonus) Collective operations](09-collectives/)

### Collective reductions

- [Parallel pi with reduction](10-parallel-pi-reduction/)
- [Heat equation solver](heat-equation/): Task 4

### User-defined communicators

- [Communicators and collectives](11-communicator/)

### Further MPI topics

These are optional bonus exercises.

- Cartesian topology
  - [Cartesian grid process topology](12-cartesian-grid/)
  - [Message chain with Cartesian communicator](13-message-chain-cartesian/)
  - [Heat equation solver](heat-equation/): Task 5
- User-defined datatypes
  - [User-defined datatypes](14-datatypes/)
  - [Modifying extent](15-datatypes-extent/)
  - [Communicating struct](16-datatypes-struct/)
- Persistent communication
  - [Message chain with persistent communication](17-message-chain-persistent/)

### Parallel debugging

Bonus content: debugging MPI programs.
- [Debugging](debugging/)

### Bonus

- [Heat equation solver](heat-equation/): Remaining tasks