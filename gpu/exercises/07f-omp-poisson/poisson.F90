! SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

#ifndef _OPENMP
#include "fake_omp.F90"
#endif

#include "helper_functions.F90"

subroutine run(n, niter, normmax)
#ifdef _OPENMP
  use omp_lib
#else
  use fake_omp
#endif
  use helper_functions
  implicit none
  integer, intent(in) :: n, niter
  real(8), intent(in) :: normmax
  integer(kind=8) :: nx, ny
  integer :: i, j, it, write_flag
  real(8), pointer :: f(:,:), u(:,:), unew(:,:), tmp(:,:)
  real(8) :: h2, t0, t1, norm2max, norm2, diff
  character(len=20) :: filename

  print '(A, I0, A, I0)', &
    "Using N = ", n, ", niter = ", niter
  nx = n
  ny = n
  h2 = 1.0
  norm2max = normmax * normmax

  allocate(f(ny, nx))
  allocate(u(ny, nx))
  allocate(unew(ny, nx))

  ! Initialize arrays
  call create_input(f)
  u = 0.0
  unew = 0.0

  ! Write initial arrays
  write(filename, '(A,I6.6,A)') 'u', 0, '.bin'
  call write_array(filename, u)
  call write_array('f.bin', f)

  ! Iterate
  t0 = omp_get_wtime()

  do it = 1, niter

    ! Stencil update
    do j = 2, nx - 1
      do i = 2, ny - 1
        unew(i,j) = 0.25 * (u(i+1,j) + u(i-1,j) + u(i,j+1) + u(i,j-1) - h2 * f(i,j))
      end do
    end do

    ! Swap the arrays
    tmp => u
    u => unew
    unew => tmp

    ! Check converge
    if (mod(it, 100) == 0) then
      norm2 = 0.0d0
      do j = 2, nx - 1
        do i = 2, ny - 1
          diff = u(i,j) - unew(i,j)
          norm2 = norm2 + diff * diff
        end do
      end do

      print '(I6.6, A, F0.6)', &
        it, ": ", norm2

      if (norm2 < norm2max) then
        print '(A)', "Converged"
        exit
      end if
    end if

    ! Write data
    if (mod(it, 1000) == 0) then
      write(filename, '(A,I6.6,A)') 'u', it, '.bin'
      call write_array(filename, u)
    end if

  end do

  t1 = omp_get_wtime()

  ! Write final result
  i = ny / 2
  j = nx / 2
  print '(A, I0, A, I0, A, F0.6)', &
    "u[", i, ",", j, "] = ", u(i,j)
  print '(A, F0.3, A)', &
    "Time spent: ", t1 - t0, " s"
  call write_array("u_end.bin", u)

  deallocate(unew)
  deallocate(u)
  deallocate(f)
end subroutine run


program main
  use iso_fortran_env, only: output_unit
  implicit none
  integer :: n, niter, nrep, i, iostat
  real(8) :: normmax
  character(len=32) :: arg

  ! Default values
  n = 1024
  niter = 2000
  nrep = 1
  normmax = 10.0

  ! Command-line argument parsing
  call get_command_argument(1, arg)
  if (len_trim(arg) > 0) then
    read(arg, *, iostat=iostat) n
    if (iostat /= 0 .or. n < 1) then
      print *, 'Size needs to be greater than zero.'
      stop 1
    end if
  end if
  call get_command_argument(2, arg)
  if (len_trim(arg) > 0) then
    read(arg, *, iostat=iostat) niter
    if (iostat /= 0 .or. niter < 1) then
      print *, "Number of iterations needs to be greater than zero."
      stop 1
    end if
  end if
  call get_command_argument(3, arg)
  if (len_trim(arg) > 0) then
    read(arg, *, iostat=iostat) nrep
    if (iostat /= 0 .or. nrep < 1) then
      print *, "Number of repetitions needs to be greater than zero."
      stop 1
    end if
  end if
  call get_command_argument(4, arg)
  if (len_trim(arg) > 0) then
    read(arg, *, iostat=iostat) normmax
    if (iostat /= 0 .or. normmax <= 0) then
      print *, "Max norm needs to be greater than zero."
      stop 1
    end if
  end if

  do i = 0, nrep - 1
    print '(A, I0)', &
      "RUN ", i
    call run(n, niter, normmax)
    call flush(output_unit)
  end do
end program main
