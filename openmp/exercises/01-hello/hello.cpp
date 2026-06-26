// SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <cstdio>
#include <omp.h>
#include <unistd.h>

void hello(int thread_id) { printf("Hello from thread %d!\n", thread_id); }

int main() {
  printf("Hello world!\n");
  int thread_id = 42;
  printf("Hello from thread %d! (before loop)\n", thread_id);
#pragma omp parallel private(thread_id)
  {
    int num_threads = omp_get_num_threads();
    int thread_id = omp_get_thread_num();
#pragma omp single
    {
      printf("Entering the loop with %d threads\t(printout with omp single)\n",
             num_threads);
    }
    hello(thread_id);
  }
  printf("Hello from thread %d! (after loop)\n", thread_id);
  printf("\n\n\n");
  printf("Hello from thread %d! (before loop)\n", thread_id);
#pragma omp parallel private(thread_id)
  {
    int num_threads = omp_get_num_threads();
    int thread_id = omp_get_thread_num();
    if (thread_id == 0) {
      printf("Entering the loop with %d threads\t(printout with  if "
             "(thread_id===0))\n",
             num_threads);
    }
    hello(thread_id);
  }
  printf("Hello from thread %d! (after loop)\n", thread_id);
  return 0;
}
