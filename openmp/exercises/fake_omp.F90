! SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

! Stub implementations of OpenMP runtime routines for use when
! compiling without OpenMP:
!
!   #ifdef _OPENMP
!     use omp_lib
!   #else
!     use fake_omp
!   #endif

#ifndef _OPENMP

#ifndef FAKE_OMP_F90
#define FAKE_OMP_F90

module fake_omp
  implicit none

  contains

  real(8) function omp_get_wtime()
    integer(kind=8) :: count, count_rate
    call system_clock(count, count_rate)
    omp_get_wtime = real(count, kind=8) / real(count_rate, kind=8)
  end function omp_get_wtime

  integer function omp_get_thread_num()
    omp_get_thread_num = 0
  end function omp_get_thread_num

  integer function omp_get_num_threads()
    omp_get_num_threads = 1
  end function omp_get_num_threads

end module fake_omp

#endif
#endif
