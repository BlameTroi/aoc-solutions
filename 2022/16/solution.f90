! solution.f90 -- 2022 day 16 -- proboscidea volcanium -- troy brumley blametroi@gmail.com

! advent of code 2022 -- day 16: proboscidea volcanium -- elephants in the volcano! under pressure!
!
program solution

   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input
   use aochelpers, only: is_digit, char_int

   implicit none

   integer, parameter            :: AOCIN = 10
   integer(kind=int64)           :: part_one, part_two  ! results
   character(len=max_aoc_reclen) :: rec

   ! initialize
   part_one = 0; part_two = 0
   rec = ""
   call open_aoc_input(AOCIN)

   call init

   call load

   part_one = do_part_one()

   part_two = do_part_two()

   ! report and close
   write (*, *)
   write (*, "('part one: ', i0)") part_one
   write (*, "('part two: ', i0)") part_two
   write (*, *)

   call close_aoc_input(AOCIN)

   stop

contains

   subroutine init
      implicit none
   end subroutine init

   subroutine load
      implicit none
   end subroutine load

   function do_part_one() result(res)
      implicit none
      integer(kind=int64) :: res
      res = -1
   end function do_part_one

   function do_part_two() result(res)
      implicit none
      integer(kind=int64) :: res
      res = -1
   end function do_part_two

end program solution
