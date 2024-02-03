! solution.f90 -- 2022 day 08 -- treetop tree house -- troy brumley blametroi@gmail.com

! advent of code 2022 day 08 treetop tree house. given an 'orchard' of trees of
! varying heights, count trees based on their visibility or the quality of their
! scenic views.

program solution

   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input

   implicit none

   integer, parameter            :: AOCIN = 10
   integer(kind=int64)           :: part_one, part_two  ! results
   integer                       :: i, j                ! work
   character(len=max_aoc_reclen) :: rec

   ! orchard plot is 99x99, adding a padding cell around the edges
   ! to ease the coding
   integer, parameter            :: MAXSIDE = 99        ! observed
   character(len=1), dimension(0:MAXSIDE + 1, 0:MAXSIDE + 1) :: orchard
   integer                       :: sidelen

   ! input file is argument 1
   call open_aoc_input(AOCIN)

   ! initialize
   rec = ""
   part_one = 0; part_two = 0
   orchard = " "

   ! load the orchard. the test dataset is smaller than the max but still square
   ! use the number of input rows to limit the scope of searches
   i = 0
   do while (read_aoc_input(AOCIN, rec))
      if (len_trim(rec) > MAXSIDE) then
         print *, "*** error *** invalid input length at record", i + 1, "'", trim(rec), "'"
         stop 1
      end if
      i = i + 1
      do j = 1, 99
         orchard(i, j) = rec(j:J)
      end do
   end do
   sidelen = i

   ! for part one, how many trees can see to the edge of the orchard (can see
   ! the edge along at least one of the cardinal directions)

   do i = 1, sidelen
      do j = 1, sidelen
         if (visible(i, j)) part_one = part_one + 1
      end do
   end do

   ! for part two find the maximum scenic score (product of trees visible along
   ! the four cardinal directions)

   do i = 2, sidelen - 1
      do j = 2, sidelen - 1
         part_two = max(part_two, scenicscore(i, j))
      end do
   end do

   ! report and close
   print *
   print *, part_one, "part one"
   print *, part_two, "part two"
   print *

   call close_aoc_input(AOCIN)

contains

   ! is the tree at r, c visible from any of the four cardinal directions?
   function visible(ar, ac)
      implicit none
      integer, intent(in) :: ar, ac
      logical :: visible
      character(len=1) :: vt
      integer :: vd, vi, vj

      vd = 4;
      vt = orchard(ar, ac)

      do vi = 0, ar - 1
         if (orchard(vi, ac) >= vt) then
            vd = vd - 1
            exit
         end if
      end do
      ! we could exit after each of these checks if it didn't decrement
      ! vd, but it's cheap and clear to just do all

      do vi = sidelen + 1, ar + 1, -1
         if (orchard(vi, ac) >= vt) then
            vd = vd - 1
            exit
         end if
      end do

      do vj = 0, ac - 1
         if (orchard(ar, vj) >= vt) then
            vd = vd - 1
            exit
         end if
      end do

      do vj = sidelen + 1, ac + 1, -1
         if (orchard(ar, vj) >= vt) then
            vd = vd - 1
            exit
         end if
      end do

      visible = vd > 0
   end function visible

   ! for part two, calculate the senic score as how many trees can be seen
   ! before the view is blocked by another tree. trees on the edges are
   ! immediately blocked at one. multiple the number of trees in each cardinal
   ! direction and determine the highest scenic score.

   function scenicscore(ar, ac)
      implicit none
      integer, intent(in) :: ar, ac
      integer :: scenicscore
      integer :: vi, vj, vn, ve, vs, vw    ! subscripts, visible trees along direction
      character(len=1) :: vt               ! this tree

      vt = orchard(ar, ac)
      vn = 0; ve = 0; vs = 0; vw = 0

      do vi = ar - 1, 1, -1
         vn = vn + 1
         if (orchard(vi, ac) >= vt) exit
      end do

      do vi = ar + 1, sidelen
         vs = vs + 1
         if (orchard(vi, ac) >= vt) exit
      end do

      do vj = ac - 1, 1, -1
         vw = vw + 1
         if (orchard(ar, vj) >= vt) exit
      end do

      do vj = ac + 1, sidelen
         ve = ve + 1
         if (orchard(ar, vj) >= vt) exit
      end do

      scenicscore = vn*ve*vs*vw
   end function scenicscore

end program solution
