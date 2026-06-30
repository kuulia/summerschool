// SPDX-FileCopyrightText: 2021 CSC - IT Center for Science Ltd. <www.csc.fi>
// SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

/* Main solver routines for heat equation solver */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <mpi.h>
#include <hip/hip_runtime.h>

#include "heat.h"

/* Exchange the boundary values */
void exchange(field *temperature, parallel_data *parallel)
{
    double *data;  
    double *sbuf_up, *sbuf_down, *rbuf_up, *rbuf_down;

    data = temperature->data;

    // Send to the up, receive from down
    sbuf_up = data + temperature->ny + 2; // upper data
    rbuf_down = data + (temperature->nx + 1) * (temperature->ny + 2); // lower halo

    MPI_Sendrecv(sbuf_up, temperature->ny + 2, MPI_DOUBLE,
                 parallel->nup, 11,
                 rbuf_down, temperature->ny + 2, MPI_DOUBLE, 
                 parallel->ndown, 11, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // Send to the down, receive from up
    sbuf_down = data + temperature->nx * (temperature->ny + 2); // lower data
    rbuf_up = data; // upper halo

    MPI_Sendrecv(sbuf_down, temperature->ny + 2, MPI_DOUBLE, 
                 parallel->ndown, 12,
                 rbuf_up, temperature->ny + 2, MPI_DOUBLE,
                 parallel->nup, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

}

__global__ void evolve_gpu_kernel(double *curr, double *prev, double a, double dt, double idx2, double idy2, int nx, int ny) {
        const int tid_x = blockDim.x*blockIdx.x + threadIdx.x;
        const int tid_y = blockDim.y*blockIdx.y + threadIdx.y;

        const int stride_x = gridDim.x*blockDim.x;
        const int stride_y = gridDim.y*blockDim.y;

        for (int i = tid_y + 1; i < nx + 1; i += stride_y) {
                for (int j = tid_x + 1; j < ny + 1; j += stride_x) {
                        int ind = i * (ny + 2) + j;
                        int ip = (i + 1) * (ny + 2) + j;
                        int im = (i - 1) * (ny + 2) + j;
                        int jp = i * (ny + 2) + j + 1;
                        int jm = i * (ny + 2) + j - 1;
                        curr[ind] = prev[ind] + a * dt *
                                ((prev[ip] -2.0 * prev[ind] + prev[im]) * idx2 +
                                (prev[jp] - 2.0 * prev[ind] + prev[jm]) * idy2);
                }
        }
}

/* Update the temperature values using five-point stencil */
void evolve(field *curr, field *prev, double a, double dt)
{
        double idx2, idy2;
        int nx, ny;
        double *currdata, *prevdata;


        dim3 grid(32,32,1);
        dim3 block(8,8,1);

        currdata = curr->data;
        prevdata = prev->data;
        nx = curr->nx;
        ny = curr->ny;

        /* Determine the temperature field at next time step
         * As we have fixed boundary conditions, the outermost gridpoints
         * are not updated. */
        idx2 = 1/(prev->dx * prev->dx);
        idy2 = 1/(prev->dy * prev->dy);

        hipLaunchKernelGGL(evolve_gpu_kernel, grid, block, 0, 0, 
                        currdata, prevdata, a, dt, idx2, idy2, nx, ny);
        hipStreamSynchronize(0);
}
