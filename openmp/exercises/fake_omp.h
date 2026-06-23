/*
 * SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
 *
 * SPDX-License-Identifier: MIT
 */

// Stub implementations of OpenMP runtime routines for use when
// compiling without OpenMP:
//
//   #ifdef _OPENMP
//   #include <omp.h>
//   #else
//   #include "fake_omp.h"
//   #endif

#ifndef _OPENMP

#ifndef FAKE_OMP_H
#define FAKE_OMP_H

#include <time.h>

static inline double omp_get_wtime(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

static inline int omp_get_thread_num()  { return 0; }
static inline int omp_get_num_threads() { return 1; }

#endif
#endif
