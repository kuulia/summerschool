// SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <stdlib.h>

void print_compiler()
{
#if defined(__cray__)
    printf("Code was compiled with Cray compiler\n");
#elif defined(__clang__)
    printf("Code was compiled with Clang compiler (other than Cray)\n");
#elif defined(__GNUC__)
    printf("Code was compiled with GNU compiler\n");
#else
    printf("Code was compiled with an unknown compiler\n");
#endif
}

void print_hello()
{
    // Read environment variables
    char *s;
    s = getenv("SLURM_PROCID");
    int id = s ? atoi(s) : 0;
    s = getenv("SLURM_NTASKS");
    int ntasks = s ? atoi(s) : 1;
    char *node = getenv("SLURMD_NODENAME");

    printf("Hello world from slurm process id %d/%d on node %s\n", id, ntasks, node);
}
