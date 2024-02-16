! aochelpers.f90 -- converters and predicates for aoc -- troy brumley blametroi@gmail.com

! little bits of common code for advent of code

! repitition isn't the root of all evil, but it's evil adjacent. after a few repeats,
! factor utility code out here. as i learn more fortran and the available libraries,
! some of these can be replaced. others can be repackaged and improved in various
! ways.
!
! my general naming conventions are to prefix predicates with is rather than the more
! lispish suffix of p or ?, especially since ? isn't part of a legal name. i also think
! the is prefix is more intention revealing.
!
! converters are usually <source type>_<destination type>. if a type kind is specified,
! it suffixes: int for default integer, int64 for integer(kind=int64), etc.
!
! result types are obvious and the only thing to note is that multi-character
! string results are allocated to the exact size required. int_str(123) returns
! "123" not "<space>123" nor are there any trailing spaces.
!
! generally arguments are intent(in). the only obvious exception is an index start for
! string scanning functions.

! todo: the exposed int64 is kinda ugly. an interface is the proper solution but i'm
! still waiting on reference materials and i want to see some examples as i work on
! learning before i commit. this is private code so ugly is ok for now.

module aochelpers

   use iso_fortran_env, only: int64, error_unit
   implicit none

   private

   public :: char_int
   public :: digit_int
   public :: int64_str
   public :: int_char
   public :: int_digit
   public :: int_str
   public :: is_char_in
   public :: is_digit
   public :: is_letter
   public :: str_int
   public :: str_int64
   public :: str_ints               ! from string, with delimiter, into allocated array of ints

contains

   ! c in ["0".."9"]?

   function is_digit(c) result(res)
      implicit none
      character(len=*), intent(in) :: c
      logical                      :: res
      res = .false.
      if (len(c) == 0) return
      res = c <= "9" .and. c >= "0"
   end function is_digit

   ! c in ["a".."z", "A".."Z"]?
   ! note: this is ascii centric

   function is_letter(c) result(res)
      implicit none
      character(len=*), intent(in) :: c
      logical                      :: res
      res = .false.
      if (len(c) == 0) return
      res = (c >= "a" .and. c <= "z") .or. (c >= "A" .and. c <= "Z")
   end function is_letter

   ! "c" in [s]

   function is_char_in(c, s) result(res)
      implicit none
      character(len=*), intent(in) :: c
      character(len=*), intent(in) :: s
      logical                      :: res
      res = .false.
      if (len(c) == 0 .or. len(s) == 0) return
      res = index(s, c) > 0
   end function is_char_in

   ! if c is a digit, return its value, otherwise -1.

   function digit_int(c) result(res)
      implicit none
      character(len=1), intent(in) :: c
      integer                      :: res
      res = -1
      if (len(c) == 0) return
      if (is_digit(c)) res = ichar(c) - ichar("0")
   end function digit_int

   ! for symmetry, a wrapper around ichar. not really needed.

   function char_int(c) result(res)
      implicit none
      character(len=*), intent(in) :: c
      integer                      :: res
      res = -1
      if (len(c) == 0) return
      res = ichar(c)
   end function char_int

   ! if i in [0..255], return as an ascii character, otherwise "?"

   function int_char(i) result(res)
      implicit none
      integer, intent(in)   :: i
      character(len=1)      :: res
      res = "?"
      if (i < 256 .and. i > -1) res = achar(i)
   end function int_char

   ! if i in [0..9], return as an ascii digit character, otherwise ?

   function int_digit(i) result(res)
      implicit none
      integer, intent(in)   :: i
      character(len=1)      :: res
      res = "?"
      if (i < 10 .and. i > -1) res = achar(ichar("0") + i)
   end function int_digit

   ! scan string s from p and return an integer value, p is updated to point past
   ! the last digit read

   function str_int(s, p) result(res)
      ! todo: signs
      implicit none
      character(len=*), intent(in) :: s
      integer, intent(inout)       :: p
      integer                      :: res
      res = 0
      if (len(s) == 0) return
      if (p < 1 .or. p > len(s)) return
      do while (p <= len(s))
         if (.not. is_digit(s(p:p))) return
         res = res * 10 + digit_int(s(p:p))
         p = p + 1
      end do
   end function str_int

   ! as in str_int but returns kind=int64

   function str_int64(s, p) result(res)
      ! todo: signs
      implicit none
      character(len=*), intent(in) :: s
      integer, intent(inout)       :: p
      integer(kind=int64)          :: res
      res = 0
      if (len(s) == 0) return
      if (p < 1 .or. p > len(s)) return
      do while (p <= len(s))
         if (.not. is_digit(s(p:p))) return
         res = res * 10 + digit_int(s(p:p))
         p = p + 1
      end do
   end function str_int64

   ! lifted from so and modified a bit, original at
   ! https://stackoverflow.com/questions/30006834/reading-a-file-of-lists-of-integers-in-fortran
   function str_ints(str, sep) result(res)
      character(len=*), intent(in) :: str
      character(len=1), intent(in) :: sep
      integer, allocatable         :: res(:)
      character(len=len(str))      :: tmp
      integer                      :: i, n
      tmp = str
      n = 0
      do i = 1, len_trim(tmp)
         if (tmp(i:i) == sep) then
            n = n + 1
            tmp(i:i) = ","
         end if
      end do
      allocate (res(n + 1))
      read (tmp, *) res
   end function str_ints
   ! return a decimal string representation of integer i in a string
   ! just large enough to hold the digits.

   function int_str(i) result(res)
      ! todo: signs
      implicit none
      integer, intent(in)           :: i
      character(len=:), allocatable :: res
      logical                       :: neg
      integer                       :: n
      character(len=64)             :: s
      s = ""
      n = abs(i)
      neg = i < 0
      do while (n > 0)
         s = int_digit(mod(int(n), 10))//trim(s)
         n = n / 10
      end do
      if (len_trim(s) < 1) s = "0"
      if (neg) s = "-"//trim(s)
      allocate (character(len=len_trim(s)) :: res)
      res = trim(s)
   end function int_str

   ! as in int_sr but the integer is of kind=int64

   function int64_str(i) result(res)
      ! todo: signs
      implicit none
      integer(kind=int64), intent(in) :: i
      character(len=:), allocatable   :: res
      integer(kind=int64)             :: n
      logical                         :: neg
      character(len=64)               :: s
      s = ""
      n = abs(i)
      neg = i < 0
      do while (n > 0)
         s = int_digit(int(mod(n, 10)))//trim(s)
         n = n / 10
      end do
      if (len_trim(s) < 1) s = "0"
      if (neg) s = "-"//trim(s)
      allocate (character(len=len_trim(s)) :: res)
      res = trim(s)
   end function int64_str

end module aochelpers
