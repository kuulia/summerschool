// SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

// Main solver routines for heat equation solver

#include <mpi.h>

#include "heat.hpp"

// Exchange the boundary values
void exchange(Field& field, const ParallelData parallel)
{
    double *sbuf, *rbuf;
    double *data = field.temperature.data();
    int nx = field.nx;
    int ny = field.ny;

#pragma omp target data use_device_ptr(data)
{
    // Send to up, receive from down
    sbuf = data + ny + 2;
    rbuf = data + (ny + 2) * (nx + 1);
    MPI_Sendrecv(sbuf, field.ny + 2, MPI_DOUBLE,
                 parallel.nup, 11,
                 rbuf, field.ny + 2, MPI_DOUBLE,
                 parallel.ndown, 11, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // Send to down, receive from up
    sbuf = data + nx * (ny + 2);
    rbuf = data;
    MPI_Sendrecv(sbuf, field.ny + 2, MPI_DOUBLE,
                 parallel.ndown, 12,
                 rbuf, field.ny + 2, MPI_DOUBLE,
                 parallel.nup, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}
}

// Update the temperature values using five-point stencil */
void evolve(Field& curr, Field& prev, const double a, const double dt)
{

  // Compilers do not necessarily optimize division to multiplication, so make it explicit
  auto inv_dx2 = 1.0 / (prev.dx * prev.dx);
  auto inv_dy2 = 1.0 / (prev.dy * prev.dy);

  // Help the compiler avoid being confused by the structs
  double *currdata = curr.temperature.data();
  double *prevdata = prev.temperature.data();

  int nx = curr.nx;
  int ny = curr.ny;

  // Determine the temperature field at next time step
  // As we have fixed boundary conditions, the outermost gridpoints
  // are not updated.
  #pragma omp target teams distribute parallel for collapse(2)
  for (int i = 1; i < nx + 1; i++) {
    for (int j = 1; j < ny + 1; j++) {
      int ind = i * (ny + 2) + j;
      int ip = (i + 1) * (ny + 2) + j;
      int im = (i - 1) * (ny + 2) + j;
      int jp = i * (ny + 2) + j + 1;
      int jm = i * (ny + 2) + j - 1;
      currdata[ind] = prevdata[ind] + a*dt*
	    ((prevdata[ip] - 2.0*prevdata[ind] + prevdata[im]) * inv_dx2 +
	     (prevdata[jp] - 2.0*prevdata[ind] + prevdata[jm]) * inv_dy2);
    }
  }

}
