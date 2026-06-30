! SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

program hello
  use omp_lib
  implicit none
  integer :: rank, tid

  rank = 0

  !$omp parallel
  tid = omp_get_thread_num()
  print '(A, I0, A, I0, A)', "Hello from thread ", tid, " in process ", rank, "!"
  !$omp end parallel

end program hello
