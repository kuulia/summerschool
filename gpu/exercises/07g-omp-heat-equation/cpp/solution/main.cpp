// SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

/* Heat equation solver in 2D. */

#include <string>
#include <iostream>
#include <stdio.h>
#include <iomanip>
#include <mpi.h>
#ifdef _OPENMP
#include <omp.h>
#endif

#include "heat.hpp"

int main(int argc, char **argv)
{
    int provided;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    if (provided < MPI_THREAD_FUNNELED) {
        printf("MPI_THREAD_FUNNELED thread support level required\n");
        MPI_Abort(MPI_COMM_WORLD, 5);
        return 5;
    }

    const int image_interval = 100;    // Image output interval

    ParallelData parallelization; // Parallelization info

    int nsteps;                 // Number of time steps
    int num_threads = 1;        // Number of threads
    Field current, previous;    // Current and previous temperature fields

    int num_devices = 0;
#ifdef _OPENMP
    num_devices = omp_get_num_devices();
#endif

    initialize(argc, argv, current, previous, nsteps, parallelization);

    // Output the initial field
    write_field(current, 0, parallelization);

    auto average_temp = average(current, parallelization);
    if (0 == parallelization.rank) {
        std::cout << "Simulation parameters: "
                  << "rows: " << current.nx_full << " columns: " << current.ny_full
                  << " time steps: " << nsteps << std::endl;
        std::cout << "Number of MPI tasks: " << parallelization.size << std::endl;
        std::cout << "Number of devices: " << num_devices << std::endl;
        std::cout << std::fixed << std::setprecision(6);
        std::cout << "Average temperature at start: " << average_temp << std::endl;
    } else {
        MPI_Gather(&num_threads, 1, MPI_INT, NULL, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }


    const double a = 0.5;     // Diffusion constant
    auto dx2 = current.dx * current.dx;
    auto dy2 = current.dy * current.dy;
    // Largest stable time step
    auto dt = dx2 * dy2 / (2.0 * a * (dx2 + dy2));

    //Get the start time stamp
    auto start_clock = MPI_Wtime();

    double *currdata = current.temperature.data();
    double *prevdata = previous.temperature.data();
    const int nx = current.nx;
    const int ny = current.ny;

#pragma omp target data map(tofrom: currdata[0:(nx+2)*(ny+2)], prevdata[0:(nx+2)*(ny+2)])
{
    // Time evolve
    for (int iter = 1; iter <= nsteps; iter++) {
        exchange(previous, parallelization);
        evolve(current, previous, a, dt);
        if (iter % image_interval == 0) {
            double *data = current.temperature.data();
            #pragma omp target update from(data[0:(nx+2)*(ny+2)])
            write_field(current, iter, parallelization);
        }
        // Swap current field so that it will be used
        // as previous for next iteration step
        std::swap(current, previous);
    }
}

    auto stop_clock = MPI_Wtime();

    // Average temperature for reference
    average_temp = average(previous, parallelization);

    if (0 == parallelization.rank) {
        std::cout << "Iteration took " << (stop_clock - start_clock)
                  << " seconds." << std::endl;
        std::cout << "Average temperature: " << average_temp << std::endl;
        if (1 == argc) {
            std::cout << "Reference value with default arguments: "
                      << 59.281239 << std::endl;
        }
    }

    // Output the final field
    write_field(previous, nsteps, parallelization);

    MPI_Finalize();

    return 0;
}
