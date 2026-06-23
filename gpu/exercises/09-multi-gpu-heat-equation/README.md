<!--
SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Multi-GPU heat equation with HIP

Port the `evolve()` function code in the `core.cpp` source file to a GPU.

In order to make use of the ported code, you also need to manage device memory
somehow. You may use implicit or explicit memory management.

For explicit memory management consider the following tasks.

- (`setup.cpp`) `initialize(...)`: Initialize also `field`s (defined in
  `heat.h`) whose data is in GPU: First initialize on host and then make carbon
  copies to device. You most likely need to add two `field*` input arguments to
  the `initialize(...)` function.
- (`setup.cpp`) Write a finalizer function for device and call that for fields
  whose data is on device. 
- (`main.cpp`) Copy the data on device to host prior to writing the image to
  disk and computing average temperature. You may also compute the average on
  device, but that's extra of this bonus exercise.
