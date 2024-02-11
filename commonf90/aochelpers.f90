! aochelpers.f90 -- converters and predicates for aoc -- troy brumley blametroi@gmail.com

! little bits of common code for advent of code

module aochelpers

   implicit none

   private

   public :: is_digit
   public :: char_int
   public :: is_char_in

contains

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

   ! "c" in [s]
   function is_char_in(c, s) result(res)
      implicit none
      character(len=1), intent(in) :: c
      character(len=*), intent(in) :: s
      logical                      :: res
      res = index(s, c) > 0
   end function is_char_in

end module aochelpers
