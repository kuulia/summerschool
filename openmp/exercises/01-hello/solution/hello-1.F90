! SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

program hello
  use omp_lib
  implicit none
  integer :: tid

  print '(A)', "Hello world!"
  !$omp parallel
  tid = omp_get_thread_num()
  print '(A, I0, A)', "Hello from thread ", tid, "!"
  !$omp end parallel

end program hello
