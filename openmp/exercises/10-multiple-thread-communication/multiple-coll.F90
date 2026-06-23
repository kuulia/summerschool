! SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

program multiple_coll
  use mpi
  implicit none
  integer :: rank, ntasks, ierr
  integer :: tid, msg

  call MPI_Init(ierr)
  call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)
  call MPI_Comm_size(MPI_COMM_WORLD, ntasks, ierr)

  tid = 0
  msg = -1

  if (rank == 0) msg = tid

  call MPI_Bcast(msg, 1, MPI_INTEGER, 0, MPI_COMM_WORLD, ierr)

  if (rank > 0) then
    print '(A, I0, A, I0, A, I0)', "Rank ", rank, " thread ", tid, " received ", msg
  end if

  call MPI_Finalize(ierr)
end program multiple_coll
