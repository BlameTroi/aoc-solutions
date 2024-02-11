! solution.f90 -- 2022 day 04 -- camp cleanup -- troy brumley blametroi@gmail.com

! advent of code 2022 day 04 -- camp cleanup
!
! input is a list of paired integer ranges. the integers have a minimum value of 1
! and a maximum value of 99 and represent 'grid' assignments. in each part of the
! problem we are looking for how each pair may overlap.
!
! for example:
!
! 1-5,4-7  overlap
! 2-8,5-6  one completely contained in the other
! 1-3,7-9  no overlap
!
! for part one, how many assignment pairs does one range fully contain the other?
program solution

   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input
   use intset, only: size_of_set, clear_set, include_in_set, exclude_from_set, intersection_of_sets, &
                     union_of_sets, intset_limit, contains_set

   implicit none

   integer, parameter            :: AOCIN = 10
   integer(kind=int64)           :: part_one, part_two  ! results
   integer                       :: i                   ! work
   logical                       :: one(0:intset_limit), two(0:intset_limit), onetwo(0:intset_limit)
   integer                       :: onelo, onehi, twolo, twohi ! range endpoints
   character(len=max_aoc_reclen) :: rec

   ! input file is argument 1
   call open_aoc_input(AOCIN)

   ! initialize
   part_one = 0; part_two = 0

   read_one: do while (read_aoc_input(AOCIN, rec))

      call sanitize(rec)
      read (rec, *) onelo, onehi, twolo, twohi

      call clear_set(one)
      do i = onelo, onehi
         call include_in_set(one, i)
      end do

      call clear_set(two)
      do i = twolo, twohi
         call include_in_set(two, i)
      end do

      if (contains_set(one, two)) then
         part_one = part_one + 1
      else if (contains_set(two, one)) then
         part_one = part_one + 1
      end if

      call intersection_of_sets(one, two, onetwo)

      if (size_of_set(onetwo) /= 0) then
         part_two = part_two + 1
      end if

   end do read_one

   ! report and close
   print *
   print *, part_one, "part one"
   print *, part_two, "part two"
   print *

   call close_aoc_input(AOCIN)

   stop

contains

   ! the input record is a comma delimited list of positive integer ranges.
   ! changing the delimiters to blanks will allow us to use read to get
   ! extract the values.

   subroutine sanitize(ar)
      implicit none
      character(*), intent(inout) :: ar
      integer                     :: vi

      do vi = 1, len(trim(ar))
         if ((ar(vi:vi) == '-') .or. (ar(vi:vi) == ',')) then
            ar(vi:vi) = ' '
         end if
      end do
   end subroutine sanitize

end program solution
!   tGridAssignment = record
!                        spec: string;
!                        gridSet: tGridSet;
!                     end;
!
!
!! see load and parse for more details. count pairs where there is
!! any overlap.
!
!function part_two(pd : pData) : integer;
!
!begin
!   part_two := 0;
!   while pd <> nil do
!      with pd^.pld do begin
!         if (one.gridSet * two.gridSet) <> [] then
!            part_two := part_two + 1;
!         pd := pd^.fwd
!      end; ! with pd^.pld
!end;
