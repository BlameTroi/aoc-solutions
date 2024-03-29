! solution.f90 -- 2022 day 01 -- calorie hoarders -- troy brumley blametroi@gmail.com

! as an experiment, can i translate (transliterate more likely) from pascal to
! fortran 90? and i quickly determine that this will go better starting from
! scratch. input is a list of integers, one per line, with blank lines
! separating groups. part one: what is the sum of the values of the largest
! group? part two: what is the sum of the values of the three largest groups?

program solution
   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, close_aoc_input, read_aoc_input, rewind_aoc_input, max_aoc_reclen

   implicit none

   integer, parameter            :: AOCIN = 10
   logical                       :: eof
   character(len=max_aoc_reclen) :: rec
   integer                       :: i
   integer(kind=int64)           :: n                    ! count, work
   integer(kind=int64)           :: gtotal, stotal, mtotal  ! grand, sub, and max
   integer(kind=int64)           :: topthree(3)             ! top three subtotals

   rec = ""
   call open_aoc_input(AOCIN)

   ! initialization
   eof = .false.
   i = 0; n = 0; gtotal = 0; stotal = 0; mtotal = 0
   topthree = [0, 0, 0]

   ! input is a list of integers, one to a line, broken into groups by blank
   ! lines. find the grand total, the maximum single group total, and the sum of
   ! the totals of the three largest groups.
   do while (.not. eof)

      i = i + 1

      eof = .not. read_aoc_input(AOCIN, rec)

      ! if there's a better way to check for blank lines, i have yet to find it
      if ((eof .eqv. .false.) .and. (len_trim(adjustl(rec)) > 0)) then
         read (rec, *) n
         gtotal = gtotal + n
         stotal = stotal + n
         cycle
      end if

      ! control break time
      if (stotal > mtotal) then
         mtotal = stotal
      end if

      ! if this subtotal isn't in the top three so far, just cycle back
      if (stotal < topthree(3)) then
         stotal = 0
         cycle
      end if

      ! update the top three
      topthree(3) = stotal
      stotal = 0
      if (topthree(2) < topthree(3)) then
         n = topthree(2)
         topthree(2) = topthree(3)
         topthree(3) = n
      end if
      if (topthree(1) < topthree(2)) then
         n = topthree(1)
         topthree(1) = topthree(2)
         topthree(2) = n
      end if
   end do

   ! report and close
   print *
   print *, i, "records read"
   print *, gtotal, "grand total"
   print *, mtotal, "maximum single group subtotal"
   print *
   print *, "the top three groups are"
   stotal = 0
   do i = 1, 3
      print *, topthree(i)
      stotal = stotal + topthree(i)
   end do
   print *, stotal, " sum of top three groups "
   print *

   call close_aoc_input(AOCIN)
end program solution
