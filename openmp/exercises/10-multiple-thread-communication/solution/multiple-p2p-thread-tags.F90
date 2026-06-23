! SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

program multiple_p2p_thread_tags
  use mpi
  use omp_lib
  implicit none
  integer :: provided, rank, ntasks, ierr, i
  integer :: tid, msg, tag

  call MPI_Init_thread(MPI_THREAD_MULTIPLE, provided, ierr)

  if (provided < MPI_THREAD_MULTIPLE) then
    print '(A)', "MPI does not support MPI_THREAD_MULTIPLE"
    call MPI_Abort(MPI_COMM_WORLD, -1, ierr)
    stop
  end if

  call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)
  call MPI_Comm_size(MPI_COMM_WORLD, ntasks, ierr)

  !$omp parallel private(tid, msg, tag, i)
  tid = omp_get_thread_num()
  msg = -1
  tag = tid

  if (rank == 0) then
    msg = tid
    do i = 1, ntasks - 1
      call MPI_Send(msg, 1, MPI_INTEGER, i, tag, MPI_COMM_WORLD, ierr)
    end do
  else
    call MPI_Recv(msg, 1, MPI_INTEGER, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE, ierr)
    print '(A, I0, A, I0, A, I0)', "Rank ", rank, " thread ", tid, " received ", msg
  end if
  !$omp end parallel

  call MPI_Finalize(ierr)
end program multiple_p2p_thread_tags
