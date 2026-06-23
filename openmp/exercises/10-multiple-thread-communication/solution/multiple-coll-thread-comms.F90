! SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

program multiple_coll_thread_comms
  use mpi
  use omp_lib
  implicit none
  integer :: provided, rank, ntasks, ierr, i
  integer :: tid, msg
  integer, allocatable :: mpi_comm_thread(:)

  call MPI_Init_thread(MPI_THREAD_MULTIPLE, provided, ierr)

  if (provided < MPI_THREAD_MULTIPLE) then
    print '(A)', "MPI does not support MPI_THREAD_MULTIPLE"
    call MPI_Abort(MPI_COMM_WORLD, -1, ierr)
    stop
  end if

  call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)
  call MPI_Comm_size(MPI_COMM_WORLD, ntasks, ierr)

  ! Create thread-specific communicators
  allocate(mpi_comm_thread(omp_get_max_threads()))
  do i = 1, omp_get_max_threads()
    call MPI_Comm_dup(MPI_COMM_WORLD, mpi_comm_thread(i), ierr)
  end do

  !$omp parallel private(tid, msg)
  tid = omp_get_thread_num()
  msg = -1

  if (rank == 0) msg = tid

  call MPI_Bcast(msg, 1, MPI_INTEGER, 0, mpi_comm_thread(tid + 1), ierr)

  if (rank > 0) then
    print '(A, I0, A, I0, A, I0)', "Rank ", rank, " thread ", tid, " received ", msg
  end if
  !$omp end parallel

  ! Free communicators
  do i = 1, omp_get_max_threads()
    call MPI_Comm_free(mpi_comm_thread(i), ierr)
  end do
  deallocate(mpi_comm_thread)

  call MPI_Finalize(ierr)
end program multiple_coll_thread_comms
