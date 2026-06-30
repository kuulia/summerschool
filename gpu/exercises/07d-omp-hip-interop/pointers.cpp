/*
 * SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdio>
#include <vector>

int main(int argc, char *argv[])
{
    const int n = 1024;
    std::vector<double> x(n);
    double *_x = x.data();

    printf("printing from host the address of x in host: %p\n", _x);

    #pragma omp target data map(to: _x[0:n])
    {
        #pragma omp target data use_device_ptr(_x)
        {
            printf("printing from host the address of x in dev:  %p\n", _x);
        }

        #pragma omp target
        {
            printf("printing from dev  the address of x in dev:  %p\n", _x);
        }
    }
}
