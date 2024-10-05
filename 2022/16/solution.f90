! solution.f90 -- 2022 day 16 -- proboscidea volcanium -- troy brumley blametroi@gmail.com

! advent of code 2022 -- day 16: proboscidea volcanium -- elephants in the volcano! under pressure!
!
! open valves in a network of pipes to release the most pressure over
! 30 minutes. it takes one minute to move to an adjacent node, and
! another minute to open a valve.
!
! sample input and start of a run, with comments.:
!
! Valve AA has flow rate=0; exits lead to valves DD, II, BB
! Valve BB has flow rate=13; exits lead to valves CC, AA
! Valve CC has flow rate=2; exits lead to valves DD, BB
! Valve DD has flow rate=20; exits lead to valves CC, AA, EE
! Valve EE has flow rate=3; exits lead to valves FF, DD
! Valve FF has flow rate=0; exits lead to valves EE, GG
! Valve GG has flow rate=0; exits lead to valves FF, HH
! Valve HH has flow rate=22; tunnel leads to valve GG
! Valve II has flow rate=0; exits lead to valves AA, JJ
! Valve JJ has flow rate=21; tunnel leads to valve II
!
! == Minute 1 ==            ** you are in AA **
! No valves are open.
! You move to valve DD.
!
! == Minute 2 ==
! No valves are open.
! You open valve DD.
!
! == Minute 3 ==           ** note that pressure release starts during next minute **
! Valve DD is open, releasing 20 pressure.
! You move to valve CC.
!
! == Minute 4 ==
! Valve DD is open, releasing 20 pressure.
! You move to valve BB.
!
! == Minute 5 ==
! Valve DD is open, releasing 20 pressure.
! You open valve BB.
!
! == Minute 6 ==
! Valves BB and DD are open, releasing 33 pressure.
! You move to valve AA.    ** back to AA **
!
! == Minute 7 ==
! Valves BB and DD are open, releasing 33 pressure.
! You move to valve II     ** different path next **
!
! ... and so on ...
!
! you can revisit a node, but you don't backtrack, so once a path has
! been followed, it becomes closed. so a visited structure will be
! needed.

program solution

   use, intrinsic :: iso_fortran_env, only: int64

   use aocinput

   use aochelpers

   implicit none

   integer, parameter            :: AOCIN = 10
   integer(kind=int64)           :: part_one, part_two  ! results
   character(len=max_aoc_reclen) :: rec

   ! domain data and types
   integer, parameter            :: DURATION = 30    ! units of time
   integer, parameter            :: MAX_TUNNELS = 10 ! twice observed for padding

   ! cache node name to subscript info
   type map_id_subscript_t
      character(len=2) :: id                      ! aa-zz
      integer          :: isat                    ! subscript in nodes
   end type map_id_subscript_t

   ! our dataset
   type node_t
      character(len=2) :: id                      ! this node
      integer          :: flow                    ! pressure that can be released per unit of time
      integer          :: numexits                ! count of
      type(map_id_subscript_t) :: exits(1:MAX_TUNNELS)    ! ids of reachable nodes through tunnels
      logical          :: broken                  ! if pressure flow 0, this valve is broken
      logical          :: opened                  ! is this valve opened?
      integer          :: when                    ! when was it opened
   end type node_t

   type(node_t), allocatable :: nodes(:)
   integer      :: numnodes

   type valve_t
      integer          :: isat
      integer          :: flow
      integer          :: opened
      integer          :: when
      integer          :: cost
   end type valve_t

   type(valve_t), allocatable :: valves(:)
   integer      :: numvalves

   integer      :: starting, current
   integer      :: minpressure, maxpressure
   integer      :: minexits, maxexits

   ! initialize
   part_one = 0; part_two = 0
   rec = ""
   call open_aoc_input(AOCIN)

   call init

   call load

   part_one = do_part_one()

   part_two = do_part_two()

   ! report and close
   write (*, *)
   write (*, "('part one: ', i0)") part_one
   write (*, "('part two: ', i0)") part_two
   write (*, *)

   call close_aoc_input(AOCIN)

   stop

contains

   ! -----------------------------------------------------------------
   ! initialize those things that must be initialized. read the
   ! dataset to count the number of rows so we can dynamically
   ! allocate the graph.
   ! -----------------------------------------------------------------

   subroutine init
      numnodes = 0
      numvalves = 0
      minexits = huge(0)
      maxexits = -huge(0)
      minpressure = huge(0)
      maxpressure = -huge(0)
      call rewind_aoc_input(AOCIN)
      do while (read_aoc_input(AOCIN, rec))
         numnodes = numnodes + 1
      end do
      current = 0
   end subroutine init

   ! -----------------------------------------------------------------
   ! given an input record, parse it into a graph node. the
   ! record layout is structured but variable:
   !
   ! Valve GG has flow rate=0; exits lead to valves FF, HH
   ! 1     2  3   4    5    6  7       8    9  10     11 ...
   !
   ! node id is word 2
   ! flow rate is word 66
   ! other nodes 11->ubound
   !
   ! Valve HH has flow rate=22; tunnel leads to valve GG
   !       xx               99; tunnel(s) lead(s) to valve(s) yy (,zz)
   !
   ! convert punctuation to whitespace, split at whitespace, and then
   ! extract based on word index.
   ! -----------------------------------------------------------------

   function parse(s) result(res)
      character(len=*), intent(in)                :: s
      type(node_t)                                :: res
      integer                                     :: i, j
      character(len=:), dimension(:), allocatable :: words
      words = split_str(trim(s), " ;=,")
      associate (n => res)
         n % id = words(2)
         read (words(6), *) n % flow
         n % broken = n % flow == 0
         j = 0
         do i = 11, ubound(words, 1)
            j = j + 1
            n % exits(j) % id = words(i)
         end do
         n % numexits = j
         n % opened = .false.
         n % when = 0
      end associate
   end function parse

   ! -----------------------------------------------------------------
   ! load and do some initial calculations.
   ! -----------------------------------------------------------------

   subroutine load
      integer      :: i, j
      type(node_t) :: n

      call rewind_aoc_input(AOCIN)
      allocate (nodes(numnodes))
      i = 0
      do while (read_aoc_input(AOCIN, rec))
         i = i + 1
         n = parse(rec)
         nodes(i) = n
         if (n % id == "AA") starting = i
         if (n % flow == 0) cycle
         numvalves = numvalves + 1
         if (n % flow < minpressure) minpressure = n % flow
         if (n % flow > maxpressure) maxpressure = n % flow
         if (n % numexits < minexits) minexits = n % numexits
         if (n % numexits > maxexits) maxexits = n % numexits
      end do

      call map_exits

      allocate (valves(numvalves))
      j = 0
      do i = 1, numnodes
         if (nodes(i) % flow == 0) cycle
         j = j + 1
         valves(i) % isat = i
         valves(i) % flow = nodes(i) % flow
         ! valves(i) % opened = nodes(i) % opened
         valves(i) % when = nodes(i) % when
      end do

   end subroutine load

   ! -----------------------------------------------------------------
   ! cache the subscripts of nodes that can reached from this node.
   ! -----------------------------------------------------------------

   subroutine map_exits
      integer :: i, j
      do i = 1, numnodes
         do j = 1, nodes(i) % numexits
            nodes(i) % exits(j) % isat = subscript_of(nodes(i) % exits(j) % id)
         end do
      end do
   end subroutine map_exits

   ! -----------------------------------------------------------------
   ! find id in nodes, return its subscript.
   ! -----------------------------------------------------------------

   function subscript_of(id) result(res)
      character(len=*), intent(in) :: id
      integer                      :: res
      integer                      :: i
      res = -1
      do i = 1, numnodes
         if (nodes(i) % id /= id) cycle
         res = i
         exit
      end do
   end function subscript_of

   ! -----------------------------------------------------------------
   ! for part one, return the maximum amount of pressure that could be
   ! released in 30 minutes. it takes one minute to move to a node,
   ! and one minute to open a valve.
   ! -----------------------------------------------------------------

   function do_part_one() result(res)
      integer(kind=int64) :: res
      integer             :: i, j
      do i = 1, numnodes
         associate (n => nodes(i))
            if (n % id == "") cycle
            write (*, "(' node ', a2, ' at ', i2, ' has exits to:')") n % id, i
            do j = 1, n % numexits
               write (*, "('      ', a2, ' at ', i2)") n % exits(j) % id, n % exits(j) % isat
            end do
         end associate
      end do

      ! find ... what exactly?
      res = -1
   end function do_part_one

   ! -----------------------------------------------------------------
   !
   ! -----------------------------------------------------------------
   function do_part_two() result(res)
      integer(kind=int64) :: res
      res = -1
   end function do_part_two

end program solution
