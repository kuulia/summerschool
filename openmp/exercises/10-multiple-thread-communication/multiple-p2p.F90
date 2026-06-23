! SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

program multiple_p2p
  use mpi
  implicit none
  integer :: rank, ntasks, ierr, i
  integer :: tid, msg, tag

  call MPI_Init(ierr)
  call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)
  call MPI_Comm_size(MPI_COMM_WORLD, ntasks, ierr)

  tid = 0
  msg = -1
  tag = 123

  if (rank == 0) then
    msg = tid
    do i = 1, ntasks - 1
      call MPI_Send(msg, 1, MPI_INTEGER, i, tag, MPI_COMM_WORLD, ierr)
    end do
  else
    call MPI_Recv(msg, 1, MPI_INTEGER, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE, ierr)
    print '(A, I0, A, I0, A, I0)', "Rank ", rank, " thread ", tid, " received ", msg
  end if

  call MPI_Finalize(ierr)
end program multiple_p2p
