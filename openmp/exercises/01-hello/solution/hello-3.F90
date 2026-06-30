! SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

program hello
#ifdef _OPENMP
  use omp_lib
#endif
  implicit none
  integer :: tid, nthreads

  print '(A)', "Hello world!"
  !$omp parallel
  tid = omp_get_thread_num()
  print '(A, I0, A)', "Hello from thread ", tid, "!"
  nthreads = omp_get_num_threads()
  !$omp end parallel
  print '(A, I0, A)', "There was ", nthreads, " threads in total!"

#ifndef _OPENMP
contains
  integer function omp_get_thread_num()
    omp_get_thread_num = 0
  end function omp_get_thread_num

  integer function omp_get_num_threads()
    omp_get_num_threads = 1
  end function omp_get_num_threads
#endif

end program hello
