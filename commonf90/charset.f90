! charset.f90 -- a very simple implementation of 'set of characters' -- troy brumley blametroi@gmail.com

! pulling in an abstract types library would be overkill for what i'm doing.
! instead i'm rolling my own support for a set of characters (ascii 1-charset_limit) using
! an array of booleans. fortran has bit operations that can probably do this
! better, but this is simple and works well enough for now.

module charset

   implicit none

   private

   public :: size_of_set
   public :: clear_set
   public :: include_in_set
   public :: exclude_from_set
   public :: intersection_of_sets
   public :: union_of_sets
   public :: print_set

   public :: charset_limit

   integer, parameter :: charset_limit = 255

contains

   ! count s
   function size_of_set(s)
      implicit none
      logical, intent(in) :: s(charset_limit)
      integer             :: i, size_of_set
      size_of_set = 0
      do i = 1, charset_limit
         if (s(i)) size_of_set = size_of_set + 1
      end do
   end function size_of_set

   ! s = []
   subroutine clear_set(s)
      implicit none
      logical, intent(inout) :: s(charset_limit)
      s = .false.
   end subroutine clear_set

   ! s = s + [c]
   subroutine include_in_set(s, c)
      implicit none
      logical, intent(inout)       :: s(charset_limit)
      character(len=1), intent(in) :: c
      s(ichar(c)) = .true.
   end subroutine include_in_set

   ! s = s - [c]
   subroutine exclude_from_set(s, c)
      implicit none
      logical, intent(inout)       :: s(charset_limit)
      character(len=1), intent(in) :: c
      s(ichar(c)) = .false.
   end subroutine exclude_from_set

   ! res = s1 intersect s2
   subroutine intersection_of_sets(s1, s2, res)
      implicit none
      logical, intent(in)    :: s1(charset_limit), s2(charset_limit)
      logical, intent(inout) :: res(charset_limit)
      integer                :: i
      do i = 1, charset_limit
         res(i) = s1(i) .and. s2(i)
      end do
   end subroutine intersection_of_sets

   ! res = s1 union s2
   subroutine union_of_sets(s1, s2, res)
      implicit none
      logical, intent(in)    :: s1(charset_limit), s2(charset_limit)
      logical, intent(inout) :: res(charset_limit)
      integer                :: i
      do i = 1, charset_limit
         res(i) = s1(i) .or. s2(i)
      end do
   end subroutine union_of_sets

   ! debugging helper
   subroutine print_set(s)
      implicit none
      logical, intent(in)          :: s(charset_limit)
      character(len=charset_limit) :: str
      integer                      :: i
      str = ""
      do i = 1, charset_limit
         if (s(i)) str = trim(str)//achar(i)
      end do
      print *, str
   end subroutine print_set

end module charset
