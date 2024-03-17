! aochelpers.f90 -- converters and predicates for aoc -- troy brumley blametroi@gmail.com

! little bits of common code for advent of code

! repitition isn't the root of all evil, but it's evil adjacent. after a few
! repeats, factor utility code out here. as i learn more fortran and the
! available libraries, some of these can be replaced. others can be repackaged
! and improved in various ways.
!
! my general naming conventions are to prefix predicates with is_ rather than
! the more lispish suffix of p or ?, especially since ? isn't part of a legal
! name. i also think the is prefix is more intention revealing.
!
! converters are usually <source type>_<destination type>. if a type kind is
! specified, it suffixes: int for default integer, int64 for
! integer(kind=int64), etc.
!
! result types are obvious and the only thing to note is that multi-character
! string results are almost always allocated to the exact size required.
! int_str(123) returns "123" not "<space>123" nor are there any trailing spaces.
!
! generally arguments are intent(in). the only obvious exception is an index
! start for string scanning functions.

! todo: the exposed int64 is kinda ugly. an interface is the proper solution but
! i'm still waiting on reference materials and i want to see some examples as i
! work on learning before i commit. this is private code so ugly is ok for now.

module aochelpers

   use iso_fortran_env, only: int64, error_unit
   implicit none

   private

   public :: char_int                ! convert character to its ascii value
   public :: digit_int               ! convert character digit to 0-9
   public :: int64_str               ! return an int64 as a string, leading sign but no padding spaces
   public :: int_char                ! return a6 character for the passed ascii value
   public :: int_digit               ! return the character digit value for an integer 0-9
   public :: int_str                 ! return a default integer as a string, as in int64_str
   public :: is_char_in              ! is a character in a string, replace with index()
   public :: is_digit                ! is this character a decimal digit?
   public :: is_letter               ! is this character a letter of the alphabet
   public :: sanatize_int            ! remove non digit and non '-' from string for use by read()
   public :: sanatize_real           ! as in sanatize_int but allow characters that make up reals
   public :: split_str               ! split a string at separator characters
   public :: str_int                 ! extract an integer value from a position in a string
   public :: str_int64               ! extract an int64 value from a position in a string
   public :: str_ints                ! from string, with delimiter, into allocated array of ints

contains

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
   end function int64_str

   ! sanatize a string by removing all non integer digits and minus signs for
   ! use by read(string, "(*(i))") an extra blank is allocated to avoid end of
   ! string boundary errors
   !
   ! this is not a parse or regex, just a character filter.

   function sanatize_int(s) result(res)
      implicit none
      character(len=*), intent(in)  :: s
      character(len=:), allocatable :: res
      integer                       :: i
      character(len=*), parameter   :: LEGAL_CHARS = "0123456789-"

      allocate (character(len=len_trim(s) + 1) :: res)
      res = repeat(" ", len_trim(s) + 1)
      do i = 1, len_trim(s)
         if (index(LEGAL_CHARS, s(i:i)) /= 0) res(i:i) = s(i:i)
      end do
   end function sanatize_int

   ! sanatize a string by removing all non-real literal digits, signs, and
   ! exponentation. this should make the string easier to handle with read().
   ! an extra blank is appended to the result to avoid end of string boundary
   ! errors.
   !
   ! this is not a parse or regex, just a character filter.

   function sanatize_real(s) result(res)
      implicit none
      character(len=*), intent(in)  :: s
      character(len=:), allocatable :: res
      integer                       :: i
      character(len=*), parameter   :: LEGAL_CHARS = "0123456789-+.Ee"

      allocate (character(len=len_trim(s) + 1) :: res)
      res = repeat(" ", len_trim(s) + 1)
      do i = 1, len_trim(s)
         if (index(LEGAL_CHARS, s(i:i)) /= 0) res(i:i) = s(i:i)
      end do
   end function sanatize_real

   ! split a longer string into tokens at separators. all separators consumed
   ! with no meaningful error handling. it shouldn't crash, but garbage in will
   ! get garbage out.
   !
   ! bugs:
   !
   ! - fortran doesn't have zero length strings in the sense most people think
   !   of them, so some of the edge cases return one single blank.

   function split_str(str, sep) result(res)
      implicit none
      character(len=*), intent(in) :: str
      character(len=*), intent(in) :: sep
      character(len=:), dimension(:), allocatable :: res
      integer :: i, j, k
      logical :: in_word
      integer :: longest, count

      ! guard against empty string
      if (len_trim(str) == 0) then
         allocate (character(len=1) :: res(1))
         res(1) = ""
         return
      end if

      ! guard against empty separator, distinct from a single space
      if (len(sep) == 0) then
         allocate (character(len=len(str)) :: res(1))
         res(1) = str
         return
      end if

      ! count actual number and find longest
      longest = 0
      count = 0
      in_word = .false.
      j = 0
      do i = 1, len(str)
         if (index(sep, str(i:i)) /= 0) then
            if (in_word) then
               count = count + 1
               longest = max(longest, j)
               j = 0
               in_word = .false.
            end if
         else
            j = j + 1
            in_word = .true.
         end if
      end do
      ! don't forget the last word
      if (in_word) then
         count = count + 1
         longest = max(longest, j)
      end if

      ! handle no words found, single entry empty string
      if (count == 0) then
         allocate (character(len=1) :: res(1))
         res(1) = ""
         return
      end if

      ! copy into result
      allocate (character(len=longest) :: res(count))
      in_word = .false.
      j = 1
      k = 1
      res(j) = repeat(" ", longest)
      do i = 1, len(str)
         if (index(sep, str(i:i)) /= 0) then
            if (in_word) then
               k = 1
               j = j + 1
               res(j) = ""
               in_word = .false.
            end if
         else
            res(j) (k:k) = str(i:i)
            k = k + 1
            in_word = .true.
         end if
      end do

   end function split_str

end module aochelpers
