! intset.f90 -- a very simple implementation of 'set of integers' -- troy brumley blametroi@gmail.com

! pulling in an abstract types library would be overkill for what i'm doing.
! instead i'm rolling my own support for a set of integers (0-intset_limit)
! using an array of booleans. fortran has bit operations that can probably do
! this better, but this is simple and works well enough for now.

module intset

   use iso_fortran_env, only: int64

   implicit none

   private

   public :: size_of_set
   public :: clear_set
   public :: include_in_set
   public :: exclude_from_set
   public :: intersection_of_sets
   public :: union_of_sets
   public :: contains_set

   public :: intset_limit

   integer, parameter :: intset_limit = 999

contains

   ! count s
   function size_of_set(s)
      implicit none
      logical, intent(in) :: s(0:intset_limit)
      integer             :: i, size_of_set
      size_of_set = 0
      do i = 0, intset_limit
         if (s(i)) size_of_set = size_of_set + 1
      end do
   end function size_of_set

   ! s = []
   subroutine clear_set(s)
      implicit none
      logical, intent(inout) :: s(0:intset_limit)
      s = .false.
   end subroutine clear_set

   ! s = s + [i]
   subroutine include_in_set(s, i)
      implicit none
      logical, intent(inout)          :: s(0:intset_limit)
      integer(kind=int64), intent(in) :: i
      s(i) = .true.
   end subroutine include_in_set

   ! s = s - [i]
   subroutine exclude_from_set(s, i)
      implicit none
      logical, intent(inout)          :: s(0:intset_limit)
      integer(kind=int64), intent(in) :: i
      s(i) = .false.
   end subroutine exclude_from_set

   ! res = s1 intersect s2
   subroutine intersection_of_sets(s1, s2, res)
      implicit none
      logical, intent(in)    :: s1(0:intset_limit), s2(0:intset_limit)
      logical, intent(inout) :: res(0:intset_limit)
      integer(kind=int64)    :: i
      do i = 0, intset_limit
         res(i) = s1(i) .and. s2(i)
      end do
   end subroutine intersection_of_sets

   ! res = s1 union s2
   subroutine union_of_sets(s1, s2, res)
      implicit none
      logical, intent(in)    :: s1(0:intset_limit), s2(0:intset_limit)
      logical, intent(inout) :: res(0:intset_limit)
      integer(kind=int64)    :: i
      do i = 0, intset_limit
         res(i) = s1(i) .or. s2(i)
      end do
   end subroutine union_of_sets

   ! s1 <= s2
   function contains_set(s1, s2)
      implicit none
      logical              :: contains_set
      logical, intent(in)  :: s1(0:intset_limit), s2(0:intset_limit)
      integer(kind=int64)  :: i
      contains_set = .true.
      do i = 0, intset_limit
         if (s2(i) .and. s1(i)) cycle
         contains_set = .false.
         exit
      end do
   end function
end module intset
