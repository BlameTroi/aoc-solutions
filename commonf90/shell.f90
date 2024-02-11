! solution.f90 -- 2022 day ?? -- -- troy brumley blametroi@gmail.com

program solution

   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input
   !use intset, only: size_of_set, clear_set, include_in_set, exclude_from_set, intersection_of_sets, &
   use aochelpers, only: is_digit, char_int

   implicit none

   integer, parameter            :: AOCIN = 10
   integer(kind=int64)           :: part_one, part_two  ! results
   character(len=max_aoc_reclen) :: rec

   ! input file is argument 1
   rec = ""
   call open_aoc_input(AOCIN)

   ! initialize
   part_one = 0; part_two = 0


   ! report and close
   print *
   print *, part_one, "part one"
   print *, part_two, "part two"
   print *

   call close_aoc_input(AOCIN)

end program solution
