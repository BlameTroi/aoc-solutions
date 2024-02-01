
! solution.f90 -- 2022 day ?? -- -- Troy Brumley blametroi@gmail.com

! a shell to fill in
program solution

   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input

   implicit none

   integer, parameter            :: unitin = 10
   integer(kind=int64)           :: part_one, part_two  ! results
   integer(kind=int64)           :: i                   ! work
   integer(kind=int64)           :: reclen
   character(len=max_aoc_reclen) :: recin

   ! input file is argument 1
   call open_aoc_input(unitin)

   ! initialize
   part_one = 0; part_two = 0

   read_one: do

      if (.not. read_aoc_input(unitin, recin)) exit     ! read until end of file

      i = i + 1

      reclen = len_trim(recin)
      if (reclen < 1) cycle read_one

   end do read_one

   call rewind_aoc_input(unitin)

   read_two: do

      if (.not. read_aoc_input(unitin, recin)) exit     ! read until end of file

      i = i + 1

      reclen = len_trim(recin)
      if (reclen < 1) cycle read_two

   end do read_two

   ! report and close
   print *
   print *, part_one, "part one"
   print *, part_two, "part two"
   print *

   call close_aoc_input(unitin)

end program solution
