! SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

! Heat equation solver in 2D.

program heat_solve
  use heat
  use core
  use io
  use setup
  use utilities
#ifdef _OPENMP
  use omp_lib
#endif

  implicit none

  real(dp), parameter :: a = 0.5 ! Diffusion constant
  type(field) :: current, previous    ! Current and previus temperature fields

  real(dp) :: dt     ! Time step
  integer :: nsteps       ! Number of time steps
  integer, parameter :: image_interval = 100 ! Image output interval

  type(parallel_data) :: parallelization
  integer :: provided
  integer :: ierr

  integer :: iter, r

  real(dp) :: average_temp   !  Average temperature
  integer :: num_threads = 1
  integer, allocatable :: num_threads_rank(:)

  real(kind=dp) :: start, stop ! Timers

  call mpi_init_thread(MPI_THREAD_FUNNELED, provided, ierr)
  if (provided < MPI_THREAD_FUNNELED) then
    write(*,*) "MPI_THREAD_FUNNELED thread support level required"
    call mpi_abort(MPI_COMM_WORLD, 5, ierr)
  end if

  !$omp parallel
  !$omp single
#ifdef _OPENMP
  num_threads = omp_get_num_threads()
#endif
  !$omp end single
  !$omp end parallel

  call initialize(current, previous, nsteps, parallelization)

  ! Draw the picture of the initial state
  call write_field(current, 0, parallelization)

  average_temp = average(current, parallelization)
  if (parallelization % rank == 0) then
     allocate(num_threads_rank(parallelization%size))
     call mpi_gather(num_threads, 1, MPI_INTEGER, num_threads_rank, 1, MPI_INTEGER, &
          & 0, MPI_COMM_WORLD, ierr)
     write(*,'(A, I5, A, I5, A, I5)') 'Simulation grid: ', current%nx_full, ' x ', &
          & current%ny_full, ' time steps: ', nsteps
     write(*,'(A, I5)') 'MPI processes: ', parallelization%size
     do r = 0, parallelization%size - 1
        write(*,'(A, I0, A, I0)') 'Number of threads in MPI task ', r, ': ', num_threads_rank(r + 1)
     end do
     write(*,'(A,F9.6)') 'Average temperature at start: ', average_temp
     deallocate(num_threads_rank)
  else
     call mpi_gather(num_threads, 1, MPI_INTEGER, num_threads_rank, 1, MPI_INTEGER, &
          & 0, MPI_COMM_WORLD, ierr)
  end if

  ! Largest stable time step
  dt = current%dx**2 * current%dy**2 / &
       & (2.0 * a * (current%dx**2 + current%dy**2))

  ! Main iteration loop, save a picture every
  ! image_interval steps

  start =  mpi_wtime()

  do iter = 1, nsteps
     call exchange(previous, parallelization)
     call evolve(current, previous, a, dt)
     if (mod(iter, image_interval) == 0) then
        call write_field(current, iter, parallelization)
     end if
     call swap_fields(current, previous)
  end do

  stop = mpi_wtime()

  ! Average temperature for reference
  average_temp = average(previous, parallelization)

  if (parallelization % rank == 0) then
     write(*,'(A,F7.3,A)') 'Iteration took ', stop - start, ' seconds.'
     write(*,'(A,F9.6)') 'Average temperature: ',  average_temp
     if (command_argument_count() == 0) then
         write(*,'(A,F9.6)') 'Reference value with default arguments: ', 59.281239
     end if
  end if

  call finalize(current, previous)

  call mpi_finalize(ierr)

end program heat_solve
