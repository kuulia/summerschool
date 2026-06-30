! SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

program hello
  use omp_lib
  implicit none
  integer :: tid, nthreads

  print '(A)', "Hello world!"
  !$omp parallel
  tid = omp_get_thread_num()
  print '(A, I0, A)', "Hello from thread ", tid, "!"
  nthreads = omp_get_num_threads()
  !$omp end parallel
  print '(A, I0, A)', "There was ", nthreads, " threads in total!"

end program hello
