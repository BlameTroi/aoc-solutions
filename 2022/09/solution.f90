! solution.f90 -- 2022 day 9 -- rope bridge -- troy brumley blametroi@gmail.com

! advent of code 2022 day 9 -- rope bridge -- tracking the motion of the tail of
! a rope flailing around after the bridge breaks.
!
! rules of the tail:
!
! if head is ever 2 steps nsew from tail, tail moves 1 step nsew otherwise
!
! if head and tail aren't touching and aren't in same row or col the tail will
! move one step diagonally to keep up
!
! part one simulated a short rope, but for part two we are given a longer rope
! with which to hang ourselves. here 9 knots follow the head knot. 1 follows h,
! 2 follows 1, etc. the knot moves are 'simultaneous' with the head moves.
! switch the data structure for the knots to an array 0..9 of coordinates x,y.

program solution

   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input

   implicit none

   integer, parameter            :: AOCIN = 10
   integer(kind=int64)           :: part_one, part_two  ! results
   character(len=max_aoc_reclen) :: rec
   integer, parameter :: MAXCOORD = 250 ! observed maximum in data plus pad
   integer, parameter :: MAXTAILS = 9   ! spec
   integer, parameter :: X = 0          ! subscript in coords
   integer, parameter :: Y = 1          ! ..
   integer :: maxx, minx                ! visited max and min, was used in development
   integer :: maxy, miny                ! for clipping
   integer :: head, tail                ! subscripts of head and final tail in coords
   integer :: coords(0:MAXTAILS, X:Y)   ! head and several tails, coordinates of same
   integer :: visitgrid(0:MAXTAILS, -MAXCOORD:MAXCOORD, -MAXCOORD:MAXCOORD) ! count of visits by knot

   part_one = 0; part_two = 0

   ! input file is argument 1
   call open_aoc_input(AOCIN)

   ! tail chasing, one knot and then max
   part_one = chaser(1)
   part_two = chaser(MAXTAILS)

   ! report and close
   print *
   print *, part_one, "part one"
   print *, part_two, "part two"
   print *

   call close_aoc_input(AOCIN)
contains

   ! clear for next run
   subroutine init
      ! initialize
      implicit none
      integer :: i
      maxx = 0; minx = 0
      maxy = 0; miny = 0
      head = 0
      tail = 1
      coords = 0
      visitgrid = 0
      do i = 0, MAXTAILS
         visitgrid(i, 0, 0) = 1
      end do
      rec = ""
   end subroutine

   ! knots must be touching their leader
   function mustmove(ak)
      integer, intent(in) :: ak           ! knot number
      logical :: mustmove
      mustmove = (abs(coords(ak - 1, X) - coords(ak, X)) > 1) .or. (abs(coords(ak - 1, Y) - coords(ak, Y)) > 1)
   end function mustmove

   ! move the knot if needed
   subroutine tailmove(ak)
      integer, intent(in) :: ak           ! knot number
      integer :: vdx, vdy
      integer :: vmx, vmy

      vdx = coords(ak - 1, X) - coords(ak, X)
      vdy = coords(ak - 1, Y) - coords(ak, Y)

      ! on same x, move 1 closer y
      if (vdx == 0) then
         if (vdy < 0) then
            coords(ak, Y) = coords(ak, Y) - 1
         else
            coords(ak, Y) = coords(ak, Y) + 1
         end if
         return
      end if

      ! on same y, move 1 closer x
      if (vdy == 0) then
         if (vdx < 0) then
            coords(ak, X) = coords(ak, X) - 1
         else
            coords(ak, X) = coords(ak, X) + 1
         end if
         return
      end if

      ! if not on same x or y, move diagonally closer
      if (vdy < 0) then
         vmy = -1
      else
         vmy = 1
      end if
      if (vdx < 0) then
         vmx = -1
      else
         vmx = 1
      end if

      coords(ak, X) = coords(ak, X) + vmx
      coords(ak, Y) = coords(ak, Y) + vmy

   end subroutine tailmove

   ! how many grid coordinates are visited by the tail as it follows the head
   ! around the plane. for part one the tail is one 'knot' behind the tail, and
   ! in part two it is nine knots behind. in each case, the trialing knot
   ! follows the knot immediately before it. our input is the movement of the
   ! head knot.

   function chaser(atails)
      implicit none
      integer, intent(in) :: atails
      integer             :: chaser
      character(len=1)    :: vmove
      integer             :: vi, vj, vk, vdist
      logical             :: vdone

      ! reset globals and start new input pass
      call init
      call rewind_aoc_input(AOCIN)

      head = 0
      tail = atails

      ! read each move skipping blank lines and comments
      readloop: do while (read_aoc_input(AOCIN, rec))

         if (len_trim(rec) == 0) cycle readloop
         if (rec(1:1) == "#") cycle readloop
         read (rec, *) vmove, vdist     ! input is <UDLR> <integer>

         ! move the requested distance
         select case (vmove)
         case ("U")
            coords(head, Y) = coords(head, Y) + vdist
         case ("D")
            coords(head, Y) = coords(head, Y) - vdist
         case ("R")
            coords(head, X) = coords(head, X) + vdist
         case ("L")
            coords(head, X) = coords(head, X) - vdist
         end select

         ! track range for clipping
         minx = min(minx, coords(head, X))
         maxx = max(maxx, coords(head, X))
         miny = min(miny, coords(head, Y))
         maxy = max(maxy, coords(head, Y))

         ! count times visited
         visitgrid(head, coords(head, X), coords(head, Y)) = visitgrid(head, coords(head, X), coords(head, Y)) + 1

         ! keep moving trailing knot(s) until all touching
         vdone = .false.
         do while (.not. vdone)
            vdone = .true.
            do vk = 1, atails
               if (mustmove(vk)) then
                  vdone = .false.
                  call tailmove(vk)
                  visitgrid(vk, coords(vk, X), coords(vk, Y)) = visitgrid(vk, coords(vk, X), coords(vk, Y)) + 1
               end if
            end do
         end do

      end do readloop

      ! while we accumulate visits separately for each knot, we only need to
      ! report the visits for the final trailing one.

      chaser = 0
      do vi = minx, maxx
         do vj = miny, maxy
            if (visitgrid(tail, vi, vj) /= 0) chaser = chaser + 1
         end do
      end do

   end function chaser

end program solution
