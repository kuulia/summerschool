! SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

program mandelbrot
  use omp_lib
  implicit none
  integer, parameter :: MAX_ITER = 255
  integer(kind=8), parameter :: BLOCK_SIZE = 4
  character(len=32) :: arg
  integer :: p, funit
  integer(kind=8) :: width, height, i, j
  real(8) :: xmin, xmax, ymin, ymax, dx, dy, t0, t1
  integer(kind=1), pointer, dimension(:,:) :: iter_counts

  ! Size of image as power of two
  p = 10
  call get_command_argument(1, arg)
  if (len_trim(arg) > 0) read(arg, *) p
  width = ishft(1_8, p)
  height = width
  print '(A, I0, A, I0)', "Image size: ", width, "x", height

  ! Define viewing window in complex plane
  xmin = -1.7d0; xmax = 0.7d0
  ymin = -1.2d0; ymax = 1.2d0

  ! Pixel spacing
  dx = (xmax - xmin) / (width - 1)
  dy = (ymax - ymin) / (height - 1)

  ! Allocate data array
  allocate(iter_counts(width, height))

  ! Start timing
  t0 = omp_get_wtime()

  !$omp parallel do collapse(2) schedule(dynamic)
  do j = 1, height, BLOCK_SIZE
    do i = 1, width, BLOCK_SIZE
      call compute_block(iter_counts, i, j, i + BLOCK_SIZE - 1, j + BLOCK_SIZE - 1, &
                         xmin, ymin, dx, dy)
    end do
  end do
  !$omp end parallel do

  ! End timing
  t1 = omp_get_wtime()
  print '(A, F0.3, A)', "Calculation took ", (t1 - t0) * 1.0d3, " milliseconds"

  ! Start timing
  t0 = omp_get_wtime()

  ! Binary output
  open(newunit=funit, file='image.bin', access='stream', form='unformatted', status='replace')
  write(funit) width
  write(funit) height
  write(funit) iter_counts
  close(funit)

  ! End timing
  t1 = omp_get_wtime()
  print '(A, F0.3, A)', "Writing file took ", (t1 - t0) * 1.0d3, " milliseconds"
  print '(A)', "Mandelbrot data saved to image.bin"

  deallocate(iter_counts)

contains

  integer(kind=1) function compute_point(x, y)
    real(8), intent(in) :: x, y
    real(8) :: zr, zi, zr_new
    integer :: iter
    zr = 0.0d0; zi = 0.0d0; iter = 0
    do while (zr*zr + zi*zi <= 4.0d0 .and. iter < MAX_ITER)
      zr_new = zr*zr - zi*zi + x
      zi = 2.0d0*zr*zi + y
      zr = zr_new
      iter = iter + 1
    end do
    compute_point = int(iter, kind=1)
  end function compute_point

  subroutine compute_block(iter_counts, i0, j0, i1, j1, xmin, ymin, dx, dy)
    integer(kind=1), intent(inout) :: iter_counts(:,:)
    integer(kind=8), intent(in) :: i0, j0, i1, j1
    real(8), intent(in) :: xmin, ymin, dx, dy
    integer(kind=8) :: i, j
    do j = j0, j1
      do i = i0, i1
        iter_counts(i, j) = compute_point(xmin + (i - 1) * dx, ymin + (j - 1) * dy)
      end do
    end do
  end subroutine compute_block

end program mandelbrot
