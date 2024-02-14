! solution.f90 -- 2022 day 06 -- wait for it -- troy brumley blametroi@gmail.com

! advent of code 2023 day 6 -- wait for it
!
! the goal is to determine how many outcomes of a 'race' exceed a particular
! distance. given the nature of the problem and data, simple arrays are
! appropriate data structures.
!
! The formula for part one is a quadratic equation and the solution can be found
! using forgotten albebra but the data is small enough to iterate so part one
! will be done that way.
!
! for part two the iteration count gets rather large, but the memory and
! processing time all fit within my constraints.

program solution

   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input
   !use intset, only: size_of_set, clear_set, include_in_set, exclude_from_set, intersection_of_sets, &
   use aochelpers, only: is_digit, digit_int, str_int64

   implicit none

   integer, parameter            :: AOCIN = 10
   integer(kind=int64)           :: part_one, part_two  ! results
   character(len=max_aoc_reclen) :: rec

   integer, parameter             :: MAXRACES = 4 ! 3 in test
   integer(kind=int64), parameter :: MAXTIMES = 56977793 ! 71530 in test
   character(len=*), parameter    :: CTIME = "Time:"
   character(len=*), parameter    :: CDISTANCE = "Distance:"

   integer(kind=int64), dimension(1:MAXRACES) :: times
   integer(kind=int64), dimension(1:MAXRACES) :: distances
   integer(kind=int64), dimension(1:MAXRACES) :: options
   integer(kind=int64), dimension(1:MAXRACES, 0:MAXTIMES) :: outcomes
   integer(kind=int64)                        :: numtimes, numraces

   ! initialize
   part_one = 0; part_two = 0
   rec = ""
   call open_aoc_input(AOCIN)
   call init

   part_one = dopartone()

   part_two = doparttwo()

   ! report and close
   print *
   print *, part_one, "part one"
   print *, part_two, "part two"
   print *

   call close_aoc_input(AOCIN)

contains

   subroutine init
      implicit none
      integer :: i, j
      do i = 1, MAXRACES
         times(i) = 0
         distances(i) = 0
         do j = 0, MAXTIMES
            outcomes(i, j) = 0
         end do
         options(i) = 0
      end do
      numraces = 0
   end subroutine init

   ! read race times and distances under part one rules. these come on two
   ! separate records so times first then distance.

   subroutine loadone
      implicit none
      integer :: i, p

      call rewind_aoc_input(AOCIN)

      if (.not. read_aoc_input(AOCIN, rec)) then
         print *, "*** error *** unexpected eof."
         stop 1
      end if
      if (rec(1:len(CTIME)) /= CTIME) then
         print *, "*** error *** missing time record."
         stop 1
      end if
      i = 0
      p = len(CTIME) + 1 ! first blank
      do while (p < len_trim(rec))
         i = i + 1
         do while (rec(p:p) == " ")
            p = p + 1
         end do
         times(i) = str_int64(rec, p)
      end do

      if (.not. read_aoc_input(AOCIN, rec)) then
         print *, "*** error *** unexpected eof."
         stop 1
      end if
      if (rec(1:len(CDISTANCE)) /= CDISTANCE) then
         print *, "*** error *** missing distance record."
         stop 1
      end if
      numtimes = i

      i = 0
      p = len(CDISTANCE) + 1 ! first blank
      do while (p < len_trim(rec))
         i = i + 1
         do while (rec(p:p) == " ")
            p = p + 1
         end do
         distances(i) = str_int64(rec, p)
      end do
      numraces = i

   end subroutine loadone

   ! for part one, calculate distances travelled given the wait and velocity
   ! rules. count the number of these distances that exceed the target distance
   ! from input.

   function churn(r) result(res)
      implicit none
      integer(kind=int64), intent(in) :: r
      integer(kind=int64)             :: res
      integer(kind=int64)             :: w

      res = 0
      do w = 0, times(r)
         outcomes(r, w) = (times(r) * w) - w * w
         if (outcomes(r, w) > distances(r)) then
            options(r) = options(r) + 1
         end if
      end do
      res = options(r)
   end function churn

   ! for each race, how many wait times produce greater distances than the
   ! record from the input? multiply those counts as the answer for part one.

   function dopartone() result(res)
      implicit none
      integer(kind=int64) :: res
      integer(kind=int64) :: r

      call init
      call loadone
      res = 1
      do r = 1, numraces
         res = res * churn(r)
      end do
   end function dopartone

   ! read race times and distances with the view of the data for part two: that
   ! the spaces are bogus and there is only one race.

   subroutine loadtwo
      implicit none
      integer :: p

      call rewind_aoc_input(AOCIN)

      if (.not. read_aoc_input(AOCIN, rec)) then
         print *, "*** error *** unexpected eof."
         stop 1
      end if
      if (rec(1:len(CTIME)) /= CTIME) then
         print *, "*** error *** missing time record."
         stop 1
      end if
      times(1) = 0
      p = len(CTIME) + 1 ! first blank
      do while (p < len_trim(rec))
         p = p + 1
         if (.not. is_digit(rec(p:p))) cycle
         times(1) = times(1) * 10 + digit_int(rec(p:p))
      end do

      if (.not. read_aoc_input(AOCIN, rec)) then
         print *, "*** error *** unexpected eof."
         stop 1
      end if
      if (rec(1:len(CDISTANCE)) /= CDISTANCE) then
         print *, "*** error *** missing distance record."
         stop 1
      end if
      distances(1) = 0
      p = len(CDISTANCE) + 1 ! first blank
      do while (p < len_trim(rec))
         p = p + 1
         if (.not. is_digit(rec(p:p))) cycle
         distances(1) = distances(1) * 10 + digit_int(rec(p:p))
      end do

   end subroutine loadtwo

   ! for part two, it turns out there is only one race and record. the spaces
   ! between the input are typos. solving for this new single race will involve
   ! so many iterations that i'll have to remember how to find roots of a
   ! quadriatic equation. but first, the parabola is regular and at right angles
   ! to the x axis so i'll fix partone to do only half the iterations.
   !
   ! ok, so with 64 bit integers and current hardware, iteration is fast enough
   ! on yesterday's problem and today's.
   !
   ! leaving the better algorithm work for another time.

   function doparttwo() result(res)
      implicit none
      integer(kind=int64) :: res
      integer(kind=int64), parameter :: highlander = 1

      call init
      call loadtwo
      res = churn(highlander)

   end function doparttwo

end program solution
