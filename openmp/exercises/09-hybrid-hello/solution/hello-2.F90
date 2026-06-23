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

  ! Check that the MPI implementation supports the required level
  if (provided < required) then
    print '(A)', "MPI does not support required thread support level"
    call MPI_Abort(MPI_COMM_WORLD, -1, ierr)
    stop
  end if

  call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)

  !$omp parallel
  tid = omp_get_thread_num()
  print '(A, I0, A, I0, A)', "Hello from thread ", tid, " in process ", rank, "!"
  !$omp end parallel

  if (rank == 0) then
    print '(/, A, I0)', "Provided thread support level: ", provided
    print '(A, I0, A)', "  ", MPI_THREAD_SINGLE, " - MPI_THREAD_SINGLE"
    print '(A, I0, A)', "  ", MPI_THREAD_FUNNELED, " - MPI_THREAD_FUNNELED"
    print '(A, I0, A)', "  ", MPI_THREAD_SERIALIZED, " - MPI_THREAD_SERIALIZED"
    print '(A, I0, A)', "  ", MPI_THREAD_MULTIPLE, " - MPI_THREAD_MULTIPLE"
  end if

  call MPI_Finalize(ierr)

end program hello
