! solution.f90 -- 2023 day 01 -- trebuchet -- troy brumley blametroi@gmail.com

! advent of code 2023 day 01 - you're getting launched from a trebuchet!
!
! you're getting launched at a target from a trebuchet, extract calibration
!  values from some rather messy input.

program solution

   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input

   implicit none

   integer, parameter            :: AOCIN = 10
   integer                       :: part_one, part_two  ! results
   character(len=max_aoc_reclen) :: rec

   character(len=*), dimension(0:9), parameter :: textual_digits = ["zero ", "one  ", "two  ", &
                                                                    "three", "four ", "five ", &
                                                                    "six  ", "seven", "eight", &
                                                                    "nine "]

   ! initialize

   rec = ""
   call open_aoc_input(AOCIN)
   part_one = 0; part_two = 0

   ! easy peasy

   do while (read_aoc_input(AOCIN, rec))
      if (len_trim(rec) < 1) cycle
      part_one = part_one + part_one_value(rec)
      part_two = part_two + part_two_value(rec)
   end do

   ! report and close

   print *
   print *, part_one, "part one"
   print *, part_two, "part two"
   print *

   call close_aoc_input(AOCIN)

contains

   ! simple predicates and converters

   function is_digit(c) result(res)
      implicit none
      character(len=1), intent(in) :: c
      logical                      :: res
      res = c <= "9" .and. c >= "0"
   end function is_digit

   function char_int(c) result(res)
      implicit none
      character(len=1), intent(in) :: c
      integer                      :: res
      res = -1
      if (is_digit(c)) res = ichar(c) - ichar("0")
   end function char_int

   ! the parse for part one of the problem is to extract all of the digit
   ! characters in order from the string and take the first digit as tens digit
   ! and the last digit as the units value. note that a digit can be both the
   ! first and the last digit in the string.

   function part_one_value(s) result(res)
      implicit none
      character(len=*), intent(in) :: s
      integer                      :: res
      integer                      :: i, first, last
      first = -1; last = -1
      do i = 1, len_trim(s)
         if (.not. is_digit(s(i:i))) cycle
         if (first == -1) first = char_int(s(i:i))
         last = char_int(s(i:i))
      end do
      res = first * 10 + last
   end function part_one_value

   ! the parse for part two introduces the wrinkle that digits could be
   ! spelled out (and overlap as in fiveight) in a string. otherwise
   ! it's the same idea as part one: value is first * 10 + last.

   function part_two_value(s) result(res)
      implicit none
      character(len=*), intent(in) :: s
      integer :: res
      integer                      :: i, j, k, first, last
      first = -1; last = -1
      nextchar: do i = 1, len_trim(s)

         if (is_digit(s(i:i))) then
            if (first == -1) first = char_int(s(i:i))
            last = char_int(s(i:i))
            cycle nextchar
         end if

         textdigit: do j = 0, 9
            k = len_trim(textual_digits(j))
            if (i + k - 1 > len_trim(s)) cycle textdigit
            if (s(i:i + k - 1) == textual_digits(j)) then
               if (first == -1) first = j
               last = j
               exit textdigit
            end if

         end do textdigit

      end do nextchar
      res = first * 10 + last
   end function part_two_value

end program solution
