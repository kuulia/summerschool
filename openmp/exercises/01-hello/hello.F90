! SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

program hello
  implicit none

  print '(A)', "Hello world!"
  !$omp parallel
  print '(A)', "Hello from thread!"
  !$omp end parallel

end program hello
