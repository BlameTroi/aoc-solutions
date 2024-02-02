! a shell to fill in
program testbyter

   use iso_fortran_env, only: int64
   use iso_c_binding, only: c_int8_t
   use aocinput, only: open_aoc_input_byte, read_aoc_input_byte, close_aoc_input, rewind_aoc_input


   implicit none

   integer, parameter            :: AOCIN = 10
   integer                       :: i
   integer(kind=c_int8_t)        :: b
   character(len=1)              :: c

   ! input file is argument 1
   call open_aoc_input_byte(AOCIN)

   i = 0
   read_one: do

      if (.not. read_aoc_input_byte(AOCIN, b)) exit     ! read until end of file

      i = i + 1
      if (b < 32) then ! control character
         print *, i, b, "newline?"
      else
         print *, i, b, achar(b)
      end if

   end do read_one

   ! report and close
   print *

   call close_aoc_input(AOCIN)

end program testbyter
