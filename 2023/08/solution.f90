! solution.f90 -- 2022 day 08 -- haunted wasteland -- troy brumley blametroi@gmail.com

! advent of code 2023 day 8 -- haunted wasteland
!
! this is a map node traversal problem. walk from node to node according to the
! directions given and count your steps.
!
! for part one of the problem, given an instruction loop and a list of map
! nodes, how many steps are needed to go from node aaa to node zzz?
!
! for part two of the problem, it's a simultaneous traversal of nodes with
! slightly different instructions. count the steps to where all the simultaneous
! paths reach the same point at the same time.
!
! part two was a bear and i solved it outside of this framework but using some
! of the code to determine how long it would take each of the walks to loop.
! finding prime multiples and then multiplying all the steps sans the common
! gets the answer which is in problem.txt.

program solution

   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input
   !use intset, only: size_of_set, clear_set, include_in_set, exclude_from_set, intersection_of_sets, &
   use aochelpers, only: is_digit, char_int

   implicit none

   integer, parameter            :: AOCIN = 10
   integer(kind=int64)           :: part_one, part_two  ! results
   character(len=max_aoc_reclen) :: rec

   integer, parameter :: MAXNODES = 766           ! 7 or 8 in two smaller datasets
   integer, parameter :: MAXMOVES = 1000          ! 100 in the smaller datasets
   integer, parameter :: MAXSIMULTANEOUS = MAXNODES
   character(len=*), parameter :: STARTAT = "AAA"      ! node to start the walk from
   character(len=*), parameter :: STARTSUFFIX = "A"    ! in part two, node suffix of the start
   character(len=*), parameter :: ENDAT = "ZZZ"        ! node to end the walk at
   character(len=*), parameter :: ENDSUFFIX = "Z"      ! and in part two, node suffix of end
   character(len=*), parameter :: GOLEFT = "L"         ! our directions are pretty simple
   character(len=*), parameter :: GORIGHT = "R"        ! we take the left or right branch

   ! input is a graph, format is: NNN = (LLL,RRR)
   ! i store it in an array of nodes.
   type node_t
      character(len=3) :: id        ! this node tag
      character(len=3) :: leftid    ! left node tag
      character(len=3) :: rightid   ! right node tag
      integer          :: pleft     ! subscript of left cached
      integer          :: pright    ! and right
   end type node_t

   type(node_t)     :: nodes(1:MAXNODES)
   integer          :: numnodes
   character(len=1) :: moves(1:MAXMOVES)
   integer          :: nummoves
   integer          :: nextmove
   integer          :: simulnodes(1:MAXSIMULTANEOUS)
   integer          :: numsimuls

   ! initialize
   part_one = 0; part_two = 0
   rec = ""
   call open_aoc_input(AOCIN)

   call init

   call load
   part_one = dopartone()

   ! report and close
   print *
   print *, part_one, "part one"
   print *, part_two, "part two"
   print *

   call close_aoc_input(AOCIN)

contains

   subroutine init
      implicit none
      integer :: i
      do i = 1, MAXNODES
         nodes(i) % id = ""
         nodes(i) % leftid = ""
         nodes(i) % rightid = ""
         nodes(i) % pleft = -1
         nodes(i) % pright = -1
      end do
      numnodes = 0
      moves = " "
      nummoves = 0
      nextmove = -1
      simulnodes = -1
      numsimuls = 0
   end subroutine init

   ! helpful predicates

   function isstartnode(i) result(res)
      implicit none
      integer, intent(in) :: i
      logical :: res
      res = nodes(i) % id(3:3) == STARTSUFFIX
   end function isstartnode

   function isendnode(i) result(res)
      implicit none
      integer, intent(in) :: i
      logical :: res
      res = nodes(i) % id(3:3) == ENDSUFFIX
   end function isendnode

   ! find all the starting nodes for a simultaneous pass through the graph

   subroutine buildsimulnodes
      implicit none
      integer :: i, j
      j = 0
      do i = 1, numnodes
         if (isstartnode(i)) then
            j = j + 1
            simulnodes(j) = i
         end if
      end do
      numsimuls = j
      print *, "found ", numsimuls, " start nodes"
   end subroutine buildsimulnodes

   ! check the current state of the simultaneous walks to see if they are all at
   ! endpoints.

   function allatend() result(res)
      implicit none
      integer :: i
      logical :: res
      res = .true.
      do i = 1, numsimuls
         if (.not. isendnode(simulnodes(i))) then
            res = .false.
            exit
         end if
      end do
   end function allatend

   ! look up a node via id sequential search. the nodes list is unordered.

   function findnode(nid) result(res)
      implicit none
      character(len=*), intent(in) :: nid
      integer :: i
      integer :: res
      res = -1
      do i = 1, numnodes
         if (nid == nodes(i) % id) then
            res = i
            return
         end if
      end do
   end function findnode

   ! get next move. if the moves are exhausted, start over from the beginning of
   ! the moves list.

   function getmove() result(res)
      implicit none
      character(len=1) :: res
      res = moves(nextmove)
      nextmove = nextmove + 1
      if (nextmove > nummoves) nextmove = 1
   end function getmove

   ! read the nodes. a node record = 'NNN = (LLL, RRR)' and the instructions are
   ! a single long string. the data given fits easily in the read buffer so i
   ! can do a normal read and then stuff it character by character into an
   ! array.

   subroutine load
      implicit none
      integer :: i
      logical :: b

      call rewind_aoc_input(AOCIN)

      ! first line of input should be the moves, a long string of LRLRLRLR
      i = 0
      b = read_aoc_input(AOCIN, rec) ! not checking the result
      do while (i < len_trim(rec))
         i = i + 1
         moves(i) = rec(i:i)
      end do
      nummoves = i
      nextmove = 1

      ! now load the nodes, allowing for a blank line after the moves
      ! input is fixed format nnn = (lll,rrr) so the parse is easy
      i = 0
      do while (read_aoc_input(AOCIN, rec))
         if (len_trim(rec) < 1) cycle
         i = i + 1
         nodes(i) % id = rec(1:3)
         nodes(i) % leftid = rec(8:10)
         nodes(i) % rightid = rec(13:15)
         nodes(i) % pleft = -1
         nodes(i) % pright = -1
      end do
      numnodes = i

   end subroutine load

! find the next node from the current one using the move. the nodes
! contain 'pointers' to cache the results of findnode calls. the
! dataset is small enough that this may not matter, but it's a good
! habit.
!
   function nextnode(curr, move) result(res)
      implicit none
      integer, intent(in) :: curr
      character(len=1), intent(in) :: move
      integer :: res
      res = -1
      if (move == GOLEFT) then
         if (nodes(curr) % pleft < 1) nodes(curr) % pleft = findnode(nodes(curr) % leftid)
         res = nodes(curr) % pleft
      else if (move == GORIGHT) then
         if (nodes(curr) % pright < 1) nodes(curr) % pright = findnode(nodes(curr) % rightid)
         res = nodes(curr) % pright
      end if
   end function nextnode

! walk the graph from node aaa to node zzz following the moves provided.

   function dopartone() result(res)
      implicit none
      integer(kind=int64) :: res
      integer :: curr, next
      integer(kind=int64) :: steps
      character(len=1) :: move

      res = -1

      curr = findnode(STARTAT)
      if (curr == -1) then
         print *, "*** error *** start node not found"
         stop 1
      end if

      steps = 0
      do while (nodes(curr) % id /= ENDAT)
         move = getmove()
         steps = steps + 1
         next = nextnode(curr, move)
         curr = next
      end do
      res = steps
   end function dopartone

end program solution
!
!{ for part two, do the moves simultaneously starting from all the
!  nodes that end with 'A' until all the nodes at the end of a move end
!  with 'Z'. }
!
!function parttwo : integer;
!
!var
!  i, j   : integer;
!  steps  : integer;
!  move   : char;
!  cycles : array[1..10] of integer;
!
!begin
!   buildsimulnodes;
!   for i := 1 to numsimuls do
!      cycles[i] := 0;
!   steps := 0;
!   { the sims will loop...some take more steps per loop than
!     others. once you know how long it takes each one to
!     finish, you should be able to figure out when they all
!     synch. }
!   while not allatend do begin
!      steps := steps + 1;
!      {if steps > 50 then
!      aochalt('debug break'); }
!      move := getmove;
!      for i := 1 to numsimuls do begin
!         if not isendnode(simulnodes[i]) then
!            simulnodes[i] := nextnode(simulnodes[i], move);
!         if cycles[i] < 1 then
!            if isendnode(simulnodes[i]) then
!               cycles[i] := steps;
!      end;
!   end;
!   j := 0;
!   for i := 1 to numsimuls do
!      if cycles[i] = 0 then
!         break
!      else
!         j := j + 1;
!   if j = numsimuls then begin
!      writeln;
!      writeln('all have cycled at least once');
!      for i := 1 to numsimuls do
!         writeln(i : 2, cycles[i] : 20);
!      writeln('break out a calculator to get the real answer');
!      { aochalt('check this output'); }
!   end;
!   parttwo := steps;
!end;
