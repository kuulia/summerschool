! SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

program data_sharing
  use omp_lib
  implicit none
  integer :: var

  var = 42

  print '(A, I0)', "Main thread: initial var = ", var
  !$omp parallel shared(var)
  print '(A, I3, A, I0)', "Thread  ", omp_get_thread_num(), ": initial var = ", var
  var = omp_get_thread_num()
  print '(A, I3, A, I0)', "Thread  ", omp_get_thread_num(), ":   final var = ", var
  !$omp end parallel
  print '(A, I0)', "Main thread:   final var = ", var

end program data_sharing
