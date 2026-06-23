! SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

program multiple_coll_single_comm
  use mpi
  use omp_lib
  implicit none
  integer :: provided, rank, ntasks, ierr, i
  integer :: tid, msg

  call MPI_Init_thread(MPI_THREAD_MULTIPLE, provided, ierr)

  if (provided < MPI_THREAD_MULTIPLE) then
    print '(A)', "MPI does not support MPI_THREAD_MULTIPLE"
    call MPI_Abort(MPI_COMM_WORLD, -1, ierr)
    stop
  end if

  call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)
  call MPI_Comm_size(MPI_COMM_WORLD, ntasks, ierr)

  !$omp parallel private(tid, msg, i)
  tid = omp_get_thread_num()
  msg = -1

  if (rank == 0) msg = tid

  ! Call broadcast with threads in order (once per thread; first thread 0, then thread 1, 2, ...)
  !$omp do ordered schedule(static, 1)
  do i = 1, omp_get_num_threads()
    !$omp ordered
    call MPI_Bcast(msg, 1, MPI_INTEGER, 0, MPI_COMM_WORLD, ierr)
    !$omp end ordered
  end do
  !$omp end do

  if (rank > 0) then
    print '(A, I0, A, I0, A, I0)', "Rank ", rank, " thread ", tid, " received ", msg
  end if
  !$omp end parallel

  call MPI_Finalize(ierr)
end program multiple_coll_single_comm
