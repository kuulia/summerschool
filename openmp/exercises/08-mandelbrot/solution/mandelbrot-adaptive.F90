! SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

program mandelbrot
  use omp_lib
  implicit none
  integer, parameter :: MAX_ITER = 255
  integer(kind=8), parameter :: MIN_BLOCK_SIZE = 4
  character(len=32) :: arg
  integer :: p, funit
  integer(kind=8) :: width, height
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

  !$omp parallel
  !$omp single
  call compute_adaptive(iter_counts, 1_8, 1_8, width, height, xmin, ymin, dx, dy)
  !$omp end single
  !$omp end parallel

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

  subroutine fill_block(iter_counts, i0, j0, i1, j1, value)
    integer(kind=1), intent(inout) :: iter_counts(:,:)
    integer(kind=8), intent(in) :: i0, j0, i1, j1
    integer(kind=1), intent(in) :: value
    integer(kind=8) :: i, j
    do j = j0, j1
      do i = i0, i1
        iter_counts(i, j) = value
      end do
    end do
  end subroutine fill_block

  recursive subroutine compute_adaptive(iter_counts, i0, j0, i1, j1, xmin, ymin, dx, dy)
    integer(kind=1), pointer, dimension(:,:) :: iter_counts
    integer(kind=8), intent(in) :: i0, j0, i1, j1
    real(8), intent(in) :: xmin, ymin, dx, dy
    integer(kind=8) :: w, h, im, jm
    real(8) :: x0, x1, xm, y0, y1, ym
    integer(kind=1) :: c00, c10, c01, c11, cmm

    ! Compute small block directly
    w = i1 - i0 + 1
    h = j1 - j0 + 1
    if (w <= MIN_BLOCK_SIZE .and. h <= MIN_BLOCK_SIZE) then
      call compute_block(iter_counts, i0, j0, i1, j1, xmin, ymin, dx, dy)
      return
    end if

    ! Sample corners and center
    x0 = xmin + (i0 - 1) * dx;  x1 = xmin + (i1 - 1) * dx;  xm = 0.5d0 * (x0 + x1)
    y0 = ymin + (j0 - 1) * dy;  y1 = ymin + (j1 - 1) * dy;  ym = 0.5d0 * (y0 + y1)

    c00 = compute_point(x0, y0)
    c10 = compute_point(x1, y0)
    c01 = compute_point(x0, y1)
    c11 = compute_point(x1, y1)
    cmm = compute_point(xm, ym)

    ! Fill the block uniformly if all samples are the same
    if (c00 == c10 .and. c00 == c01 .and. c00 == c11 .and. c00 == cmm) then
      call fill_block(iter_counts, i0, j0, i1, j1, c00)
      return
    end if

    ! Subdivide the block otherwise
    im = (i0 + i1) / 2
    jm = (j0 + j1) / 2
    !$omp task
    call compute_adaptive(iter_counts, i0, j0, im, jm, xmin, ymin, dx, dy)
    !$omp end task
    !$omp task
    call compute_adaptive(iter_counts, im + 1, j0, i1, jm, xmin, ymin, dx, dy)
    !$omp end task
    !$omp task
    call compute_adaptive(iter_counts, i0, jm + 1, im, j1, xmin, ymin, dx, dy)
    !$omp end task
    !$omp task
    call compute_adaptive(iter_counts, im + 1, jm + 1, i1, j1, xmin, ymin, dx, dy)
    !$omp end task

  end subroutine compute_adaptive

end program mandelbrot
