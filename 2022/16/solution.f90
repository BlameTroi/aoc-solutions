! solution.f90 -- 2022 day 16 -- proboscidea volcanium -- troy brumley blametroi@gmail.com

! advent of code 2022 -- day 16: proboscidea volcanium -- elephants in the volcano! under pressure!

! open valves in a network of pipes to release the most pressure over 30 minutes. it takes one minute to
! move to a pipe and open a valve. pressure released varies with some dead nodes.
!
! sample of input:
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
! nodes are all two character (alphabetic) values, so this could be subscripted in base 36.
! i ended up doing base 26, which works but doesn't sit will with me for some reason.

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
   integer, parameter            :: MAX_TUNNELS = 10 ! 5 observed, but i always pad
   integer, parameter            :: MAX_NODES = 100  ! 00-ZZ, but we're only going to occupy 50 to 60

   ! cache node name to subscript info
   type id_to_subscript_t
      character(len=2) :: id                      ! aa-zz
      integer          :: isat                    ! subscript in nodes
   end type id_to_subscript_t

   type node_t
      character(len=2) :: id                      ! this node, also the subscript
      integer          :: flow                    ! pressure that can be released per unit of time
      integer          :: numexits                ! count of
      type(id_to_subscript_t) :: exits(1:MAX_TUNNELS)    ! ids of reachable nodes through tunnels
      integer          :: numentries              ! ..
      type(id_to_subscript_t) :: entries(1:MAX_TUNNELS)  ! ..
      logical          :: broken                  ! if pressure flow 0, this valve is broken
      logical          :: opened                  ! is this valve opened?
      integer          :: when                    ! when was it opened
   end type node_t

   type(node_t) :: nodes(1:MAX_NODES)    ! most of these will be empty
   integer      :: numnodes
   integer      :: current               ! which node am i in, first is aa

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

   ! initialize those things that must be initialized

   subroutine init
      integer :: i
      do i = 1, MAX_NODES
         associate (n => nodes(i))
            n % id = ""
            n % flow = 0
            n % numexits = 0
            n % exits = id_to_subscript_t("", -0)
            n % numentries = 0
            n % entries = id_to_subscript_t("", -0)
            n % broken = .false.
            n % opened = .false.
            n % when = 0
         end associate
      end do
      numnodes = 0
      current = 0
   end subroutine init

   ! read and parse input into nodes
   !
   ! 1     2  3   4    5    6  7       8    9  10     11 ...
   ! Valve GG has flow rate=0; exits lead to valves FF, HH
   ! node is 2
   ! flow rate is 6
   ! other nodes 11->ubound
   ! Valve HH has flow rate=22; tunnel leads to valve GG
   !       xx               99; tunnel(s) lead(s) to valve(s) yy (,zz)
   ! remove punctuation
   ! remove words

   subroutine load
      integer :: i, j, k
      character(len=:), dimension(:), allocatable :: words

      call rewind_aoc_input(AOCIN)
      numnodes = 0
      do while (read_aoc_input(AOCIN, rec))
         write (*, *) trim(rec)
         numnodes = numnodes + 1
         words = split_str(trim(rec), " ;=,")
         i = numnodes
         associate (n => nodes(i))
            n % id = words(2)
            read (words(6), *) n % flow
            n % broken = n % flow == 0
            k = 0
            do j = 11, ubound(words, 1)
               k = k + 1
               n % exits(k) % id = words(j)
            end do
            n % numexits = k
            n % opened = .false.
            n % when = 0
         end associate
      end do
   end subroutine load

   function do_part_one() result(res)
      implicit none
      integer(kind=int64) :: res
      integer             :: i, j
      write (*, "('nodes: ', i0)") numnodes
      do i = lbound(nodes, 1), ubound(nodes, 1)
         associate (n => nodes(i))
            if (n % id == "") cycle
            write (*, "('node ', a2, ' at ', i0, ' has ', i0, ' exits at:')") n % id, i, n % numexits
            do j = 1, n % numexits
               write (*, "('  ', a2, ' ', i0)") n % exits(i) % id, n % exits(i) % isat
            end do
         end associate
      end do
      res = -1
   end function do_part_one

   function do_part_two() result(res)
      implicit none
      integer(kind=int64) :: res
      res = -1
   end function do_part_two

end program solution
