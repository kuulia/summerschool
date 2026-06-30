! SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

program lines
  use omp_lib
  implicit none
  character(len=32) :: arg
  integer :: unit, N, i, seed_val, seed_size
  integer, allocatable :: seed_arr(:)
  real(8) :: x1, y1, x2, y2, dx, dy, distance
  real(8) :: total_distance, average_distance
  real(8) :: r1, r2, r3, t0, t1

  ! Number of Monte Carlo samples
  N = 10000000
  call get_command_argument(1, arg)
  if (len_trim(arg) > 0) then
    read(arg, *) N
  end if
  print '(A, I0)', "Samples: ", N

  ! Seed
  call get_command_argument(2, arg)
  if (len_trim(arg) > 0) then
    read(arg, *) seed_val
  else
    ! Use /dev/urandom to generate default seed
    open(newunit=unit, file="/dev/urandom", &
         access="stream", form="unformatted", status="old")
    read(unit) seed_val
    close(unit)
  end if
  print '(A, I0)', "Seed: ", seed_val

  ! Get required seed size
  call random_seed(size=seed_size)
  allocate(seed_arr(seed_size))
  ! Fill seed array (not great; should be done better)
  seed_arr = seed_val
  ! Set seed
  call random_seed(put=seed_arr)
  deallocate(seed_arr)

  ! Start timing
  t0 = omp_get_wtime()

  total_distance = 0.0d0

  !$omp parallel reduction(+:total_distance) &
  !$omp   private(r1, r2, r3, x1, y1, x2, y2, dx, dy, distance)
  ! Print a few random values for debugging
  call random_number(r1)
  call random_number(r2)
  call random_number(r3)
  print '(A, I3, A, F6.4, " ", F6.4, " ", F6.4)', &
    "Thread ", omp_get_thread_num(), ": A few random values: ", r1, r2, r3

  ! Draw N random lines and calculate total distance
  !$omp do
  do i = 1, N
    call random_number(x1)
    call random_number(y1)
    call random_number(x2)
    call random_number(y2)
    dx = x1 - x2
    dy = y1 - y2
    distance = sqrt(dx*dx + dy*dy)
    total_distance = total_distance + distance
  end do
  !$omp end do
  !$omp end parallel

  ! Calculate average distance
  average_distance = total_distance / real(N, kind=8)

  ! End timing
  t1 = omp_get_wtime()

  print '(A, F0.6)', "Average distance: ", average_distance
  print '(A, F0.3, A)', "Calculation took ", (t1 - t0) * 1.0d3, " milliseconds"

end program lines
