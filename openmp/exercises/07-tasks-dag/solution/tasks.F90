! SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

program tasks
  use omp_lib
  implicit none
  integer :: s, a, b, c, d, e, f, g
  real(8) :: t0, t1

  s = 0

  ! Start timing
  t0 = omp_get_wtime()

  print '(A, I0)', "Start with ", s
  !$omp parallel
  !$omp single
  !$omp task depend(in: s) depend(out: a)
  a = func_A(s)
  !$omp end task
  !$omp task depend(in: a) depend(out: b)
  b = func_B(a)
  !$omp end task
  !$omp task depend(in: a) depend(out: c)
  c = func_C(a)
  !$omp end task
  !$omp task depend(in: a) depend(out: d)
  d = func_D(a)
  !$omp end task
  !$omp task depend(in: b) depend(out: e)
  e = func_E(b)
  !$omp end task
  !$omp task depend(in: c, d) depend(out: f)
  f = func_F(c, d)
  !$omp end task
  !$omp task depend(in: e, f) depend(out: g)
  g = func_G(e, f)
  !$omp end task
  !$omp end single
  !$omp end parallel
  print '(A, I0)', "End with ", g

  ! End timing
  t1 = omp_get_wtime()

  print '(A, F0.3, A)', "Execution took ", (t1 - t0) * 1.0d3, " milliseconds"

contains

  integer function func_A(in)
    integer, intent(in) :: in
    integer :: out
    call sleep(1)
    out = in + 1
    print '(A, I0, A, I0)', "A: ", in, "->", out
    func_A = out
  end function func_A

  integer function func_B(in)
    integer, intent(in) :: in
    integer :: out
    call sleep(1)
    out = in + 2
    print '(A, I0, A, I0)', "B: ", in, "->", out
    func_B = out
  end function func_B

  integer function func_C(in)
    integer, intent(in) :: in
    integer :: out
    call sleep(1)
    out = in + 3
    print '(A, I0, A, I0)', "C: ", in, "->", out
    func_C = out
  end function func_C

  integer function func_D(in)
    integer, intent(in) :: in
    integer :: out
    call sleep(1)
    out = in + 4
    print '(A, I0, A, I0)', "D: ", in, "->", out
    func_D = out
  end function func_D

  integer function func_E(in)
    integer, intent(in) :: in
    integer :: out
    call sleep(1)
    out = in + 5
    print '(A, I0, A, I0)', "E: ", in, "->", out
    func_E = out
  end function func_E

  integer function func_F(in1, in2)
    integer, intent(in) :: in1, in2
    integer :: out
    call sleep(1)
    out = in1 + in2 + 6
    print '(A, I0, A, I0, A, I0)', "F: ", in1, ",", in2, "->", out
    func_F = out
  end function func_F

  integer function func_G(in1, in2)
    integer, intent(in) :: in1, in2
    integer :: out
    call sleep(1)
    out = in1 + in2 + 7
    print '(A, I0, A, I0, A, I0)', "G: ", in1, ",", in2, "->", out
    func_G = out
  end function func_G

end program tasks
