! solution.f90 -- 2022 day 9 -- mirage maintenance -- troy brumley blametroi@gmail.com

! advent of code 2023 day 9 -- mirage maintenance
!
! extrapolation of sensor readings using a triangular structure that reminds me
! of pascal's triangle. for part one, extrapolate forward. for part two,
! extrapolate backward. the easiest way to do this is to invert the rows of
! readings on the load.

program solution

   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input
   use aochelpers, only: str_int, str_ints

   implicit none

   integer, parameter            :: AOCIN = 10
   integer(kind=int64)           :: part_one, part_two  ! results
   character(len=max_aoc_reclen) :: rec

   integer, parameter :: MAXSENSORS = 200 ! 3 in small test set
   integer, parameter :: MAXREADINGS = 21 ! 6 in small set

   ! note that while sensors number from 1, the logs number from 0. the actual
   ! last slot is reserved for the new predicted reading.

   integer(kind=int64) :: sensorlogs(1:MAXSENSORS, 0:MAXREADINGS)
   integer             :: numsensors
   integer             :: numreadings
   integer(kind=int64) :: scratchpad(0:MAXREADINGS, 0:MAXREADINGS)

   ! initialize
   rec = ""
   call open_aoc_input(AOCIN)
   part_one = 0; part_two = 0

   call init
   call loadone
   part_one = dopartone()

   call init
   call loadtwo
   part_two = doparttwo()

   ! report and close
   print *
   print *, part_one, "part one"
   print *, part_two, "part two"
   print *

   call close_aoc_input(AOCIN)

contains

   ! initialize global variables.

   subroutine init
      implicit none

      rec = ""
      sensorlogs = 0
      scratchpad = 0
      numsensors = 0
      numreadings = 0
   end subroutine init

   ! read sensor logs and parse into the matrix

   subroutine loadone
      implicit none
      integer              :: i, j
      integer, allocatable :: nums(:)

      call rewind_aoc_input(AOCIN)
      i = 0
      do while (read_aoc_input(AOCIN, rec))
         if (len_trim(rec) < 1) cycle
         if (rec(1:1) == "#") cycle
         i = i + 1
         nums = str_ints(rec, " ")
         numreadings = size(nums)
         do j = 1, size(nums)
            sensorlogs(i, j - 1) = nums(j)
         end do
      end do
      numsensors = i
   end subroutine loadone

   ! load one but backwards

   subroutine loadtwo
      implicit none
      integer              :: i, j
      integer, allocatable :: nums(:)

      call rewind_aoc_input(AOCIN)
      i = 0
      numreadings = -1
      do while (read_aoc_input(AOCIN, rec))
         if (len_trim(rec) < 1) cycle
         if (rec(1:1) == "#") cycle
         i = i + 1
         nums = str_ints(rec, " ")
         numreadings = size(nums)
         ! switch directions on load for part two
         do j = 1, numreadings
            sensorlogs(i, j - 1) = nums(numreadings + 1 - j)
         end do
      end do
      numsensors = i
   end subroutine loadtwo

   ! to check our work, extrapolate the next reading for each
   ! sensor and report the sum of the extrapolations. }

   function dopartone() result(res)
      implicit none
      integer(kind=int64) :: res
      integer             :: i

      res = 0
      do i = 1, numsensors
         res = res + extrapolate(i)
      end do
   end function dopartone

   ! part two, ends up the same as part one

   function doparttwo() result(res)
      implicit none
      integer(kind=int64) :: res
      integer             :: i

      res = 0
      do i = 1, numsensors
         res = res + extrapolate(i)
      end do
   end function doparttwo

   ! determine the next figure in the series by using the differences of the
   ! prior readings. the extrapolation uses a pascal's triangle like structure
   ! for the extrapolation.

   function extrapolate(s) result(res)
      implicit none
      integer, intent(in) :: s ! sensor number
      integer(kind=int64) :: res
      integer             :: i, j

      ! load up scratchpad with current sensor

      scratchpad = 0
      do j = 0, numreadings
         scratchpad(0, j) = sensorlogs(s, j)
      end do

      ! calculate differences, always looks back to prior line.
      ! each row (i) needs one fewer column than the one above.
      ! the last column in the sensor log is 0 and that's the
      ! value we need to replace.

      i = 0
      do while (i < numreadings) ! not <=, last slot reserved

         ! if the prior row is all zero, we're done
         if (isscratchrowzero(i)) exit

         ! calculate this row from prior row , this = pripr + prior+1
         i = i + 1
         do j = 0, numreadings - i - 1
            scratchpad(i, j) = scratchpad(i - 1, j + 1) - scratchpad(i - 1, j)
         end do

      end do

      ! now work back up the pad replacing the zero difference at the
      ! end with the correct value. current row's tail + prior row's
      ! tail.

      do while (i /= 0)
         j = numreadings - i
         scratchpad(i - 1, j + 1) = scratchpad(i, j) + scratchpad(i - 1, j)
         i = i - 1
      end do
      res = scratchpad(0, numreadings)
   end function extrapolate

   ! is this whole row of scratch zero?
   ! and is there a better fortran way to do the check?

   function isscratchrowzero(r) result(res)
      implicit none
      integer, intent(in) :: r
      logical             :: res
      integer             :: i

      res = .false.
      do i = 0, MAXREADINGS
         if (scratchpad(r, i) /= 0) return
      end do
      res = .true.
   end function isscratchrowzero

end program solution

