// SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include "heat.hpp"
#include "matrix.hpp"
#include <iostream>
#include <stdio.h>
#include <mpi.h>
#include <omp.h>

void Field::setup(int nx_in, int ny_in, ParallelData parallel)
{
    nx_full = nx_in;
    ny_full = ny_in;

    nx = nx_full / parallel.size;
    if (nx * parallel.size != nx_full) {
        std::cout << "Cannot divide grid evenly to processors" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, -2);
    }
    ny = ny_full;

   // matrix includes also ghost layers
   temperature = Matrix<double> (nx + 2, ny + 2);

#ifdef _OPENMP
    MPI_Comm intranodecomm;
    int nodeRank, nodeProcs, devCount;

    MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, 0,  MPI_INFO_NULL, &intranodecomm);

    MPI_Comm_rank(intranodecomm, &nodeRank);
    MPI_Comm_size(intranodecomm, &nodeProcs);

    MPI_Comm_free(&intranodecomm);

    devCount = omp_get_num_devices();

    if (nodeProcs > devCount) {
        printf("Not enough GPUs (%d) for all processes (%d) in the node.\n",
               devCount, nodeProcs);
        fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, -2);
    }

    omp_set_default_device(nodeRank);
#endif
 

}

void Field::generate(ParallelData parallel) {

    // Radius of the source disc
    auto radius = nx_full / 6.0;
    for (int i = 0; i < nx + 2; i++) {
        for (int j = 0; j < ny + 2; j++) {
            // Distance of point i, j from the origin
            auto dx = i + parallel.rank * nx - nx_full / 2 + 1;
            auto dy = j - ny / 2 + 1;
            if (dx * dx + dy * dy < radius * radius) {
                temperature(i, j) = 5.0;
            } else {
                temperature(i, j) = 65.0;
            }
        }
    }

    // Boundary conditions
    for (int i = 0; i < nx + 2; i++) {
        // Left
        temperature(i, 0) = 20.0;
        // Right
        temperature(i, ny + 1) = 70.0;
    }

    // Top
    if (0 == parallel.rank) {
        for (int j = 0; j < ny + 2; j++) {
            temperature(0, j) = 85.0;
        }
    }
    // Bottom
    if (parallel.rank == parallel.size - 1) {
        for (int j = 0; j < ny + 2; j++) {
            temperature(nx + 1, j) = 5.0;
        }
    }
}
