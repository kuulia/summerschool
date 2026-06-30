! SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

program hello
  use mpi
  use omp_lib
  implicit none
  integer :: rank, tid, provided, required, ierr

  rank = 0
  required = MPI_THREAD_FUNNELED

  call MPI_Init_thread(required, provided, ierr)
  call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)

  !$omp parallel
  tid = omp_get_thread_num()
  print '(A, I0, A, I0, A)', "Hello from thread ", tid, " in process ", rank, "!"
  !$omp end parallel

  call MPI_Finalize(ierr)

end program hello
