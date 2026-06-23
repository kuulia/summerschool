<!--
SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Ping-pong with multiple GPUs and MPI

Implement a simple ping-pong test for GPU-to-GPU communication using:
a) indirect communication via the host, and b) direct communication with
GPU-aware MPI.

The ping-pong test consists of the following steps:
  1. Send a vector from one GPU to another
  2. The receiving GPU should increment all elements of the vector by one
  3. Send the vector back to the original GPU

For reference, there is also a CPU-to-CPU implementation in the skeleton
code ([ping-pong.cpp](ping-pong.cpp)). Timing of all tests is also included to
compare the execution times.

Run the ping-pong test with one node, two GPUs per node and two tasks per node.
