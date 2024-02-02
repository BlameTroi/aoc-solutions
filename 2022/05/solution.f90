! solution.f90 -- 2022 day 05 -- supply stacks -- troy brumley blametroi@gmail.com

! a shell to fill in
! advent of code 2022 day 5 -- supply stacks
!
! cargo contains are in stacks and being moved around by a crane. at the end
! of the moves provided, what is the id of the crate on the top of each stack?
!
! for part one, treat the moves as push and pop on a stack. for part two, the
! moves grab more than one containter.
!
! the input consists of a representation of the starting state of the containers
! as they sit in one of the nine stacks, followed by a blank line, followed by
! instructions to move <count> from <stack> to <other stack>.
!
! there are a few different ways to approach reading the starting state. i've
! decided to try to read backwards from the stack number line.
!
! the full dataset has nine stacks, and the smaller set only three, but that
! won't cause any real problem.
!
! a sample of the input:
!
!     [S] [C]         [Z]
! [F] [J] [P]         [T]     [N]
! [G] [H] [G] [Q]     [G]     [D]
! [V] [V] [D] [G] [F] [D]     [V]
! [R] [B] [F] [N] [N] [Q] [L] [S]
!  1   2   3   4   5   6   7   8   9
!
! move 3 from 4 to 6
! move 1 from 5 to 8

program solution

   use iso_fortran_env, only: int64, error_unit
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input

   implicit none

   integer, parameter         :: MAX_STACKS = 9       ! stacks number 1-9 as observed in data
   integer, parameter         :: MAX_DEPTH = 100      ! arbitrary but big enough
   integer, parameter         :: MAX_MOVES = 600      ! observed

   type :: t_stack
      integer                 :: depth               ! how many
      character(len=1)        :: crates(MAX_DEPTH)   ! ids are single characters a-z
   end type t_stack

   type(t_stack)              :: buffer, dock(MAX_STACKS)  ! the dock and a convenience buffer
   integer                    :: to_move, from_stack, to_stack

   integer, parameter         :: AOCIN = 10
   integer(kind=int64)        :: part_one, part_two  ! results
   integer                    :: i, j, k             ! work
   character(len=max_aoc_reclen) :: rec
   character(len=MAX_STACKS)  :: report_one, report_two

   call open_aoc_input(AOCIN)

   ! initialize
   part_one = 0; part_two = 0
   report_one = ""; report_two = ""

   ! load initial stacks
   call rewind_aoc_input(AOCIN)
   call clear_stacks
   do while (read_aoc_input(AOCIN, rec))
      if (len_trim(rec) == 0) exit     ! blank line is end of this section
      if (scan(rec, "[") == 0) cycle   ! ignore this line
      j = 0;
      do k = 1, len_trim(rec), 4       ! "[x] "
         j = j + 1
         if (rec(k:k) == "[") then
            call push_stack(dock(j), rec(k + 1:k + 1))
         end if
      end do
   end do
   call reverse_stacks

   ! process moves under part one rules, last record read was the
   ! blank line between the initial stack and the moves
   do while (read_aoc_input(AOCIN, rec))
      call sanitize(rec)
      read (rec, *) to_move, from_stack, to_stack
      do i = 1, to_move
         call push_stack(dock(to_stack), pop_stack(dock(from_stack)))
      end do
   end do

   ! report the top crates of each stack
   do i = 1, MAX_STACKS
      if (peek_stack(dock(i)) == " ") then
         report_one = trim(report_one)//"-"
      else
         report_one = trim(report_one)//peek_stack(dock(i))
      end if
   end do

   ! reload and reprocess using the rules for part two
   call rewind_aoc_input(AOCIN)
   call clear_stacks
   do while (read_aoc_input(AOCIN, rec))
      if (len_trim(rec) == 0) exit     ! blank line is end of this section
      if (scan(rec, "[") == 0) cycle   ! ignore this line
      j = 0;
      do k = 1, len_trim(rec), 4       ! "[x] "
         j = j + 1
         if (rec(k:k) == "[") then
            call push_stack(dock(j), rec(k + 1:k + 1))
         end if
      end do
   end do
   call reverse_stacks

   ! process moves under part two rules, last record read was the
   ! blank line between the initial stack and the moves
   do while (read_aoc_input(AOCIN, rec))
      call sanitize(rec)
      read (rec, *) to_move, from_stack, to_stack
      do i = 1, to_move
         call push_stack(buffer, pop_stack(dock(from_stack)))
      end do
      do i = 1, to_move
         call push_stack(dock(to_stack), pop_stack(buffer))
      end do
   end do

   ! report the top crates of each stack
   do i = 1, MAX_STACKS
      if (peek_stack(dock(i)) == " ") then
         report_two = trim(report_two)//"-"
      else
         report_two = trim(report_two)//peek_stack(dock(i))
      end if
   end do

   ! report and close
   print *
   print *, report_one, " part one"
   print *, report_two, " part two"
   print *

   call close_aoc_input(AOCIN)

contains

   ! remove non numerics from input record for easier unformatted read
   subroutine sanitize(ar)
      implicit none
      character(*), intent(inout) :: ar
      integer                 :: vi
      do vi = 1, len_trim(ar)
         if ((ar(vi:vi) == " ") .or. (ar(vi:vi) >= "0" .and. ar(vi:vi) <= "9")) cycle
         ar(vi:vi) = " "
      end do
   end subroutine sanitize

   ! empty what is full
   subroutine clear_stacks
      implicit none
      integer                 :: vi
      do vi = 1, MAX_STACKS
         call clear_stack(dock(vi))
      end do
      call clear_stack(buffer)
   end subroutine

   subroutine clear_stack(astk)
      implicit none
      type(t_stack), intent(out) :: astk
      integer                 :: vi
      do vi = 1, MAX_DEPTH
         astk%crates(vi) = " "
      end do
      astk%depth = 0
   end subroutine clear_stack

   ! what item is on top, or a blank
   function peek_stack(astk)
      implicit none
      type(t_stack), intent(in) :: astk
      character(len=1)        :: peek_stack
      if (astk%depth /= 0) then
         peek_stack = astk%crates(astk%depth)
      else
         peek_stack = " "
      end if
   end function peek_stack

   ! client code should not access members directly
   function depth_stack(astk)
      implicit none
      type(t_stack), intent(in) :: astk
      integer                 :: depth_stack
      depth_stack = astk%depth
   end function depth_stack

   ! remove and return the id of the top of the stack, or blank if empty
   ! with no error signal.
   function pop_stack(astk)
      implicit none
      type(t_stack), intent(inout) :: astk
      character(len=1)        :: pop_stack
      pop_stack = " "
      if (astk%depth > 0) then
         pop_stack = astk%crates(astk%depth)
         astk%crates(astk%depth) = " "
         astk%depth = astk%depth - 1
      end if
   end function pop_stack

   ! push a new crate onto the stack. there is no error checking.
   subroutine push_stack(astk, ac)
      implicit none
      type(t_stack), intent(inout) :: astk
      character(len=1), intent(in) :: ac
      astk%depth = astk%depth + 1
      astk%crates(astk%depth) = ac
   end subroutine push_stack

   ! reverse the stack, helpful since the load comes in the wrong order.
   subroutine reverse_stacks
      implicit none
      integer                 :: vi
      do vi = 1, MAX_STACKS
         call reverse_stack(dock(vi))
      end do
   end subroutine reverse_stacks

   subroutine reverse_stack(astk)
      implicit none
      type(t_stack), intent(inout) :: astk
      character(len=1)        :: vc
      integer                 :: vi
      do vi = 1, astk%depth/2
         vc = astk%crates(vi)
         astk%crates(vi) = astk%crates(astk%depth + 1 - vi)
         astk%crates(astk%depth + 1 - vi) = vc
      end do
   end subroutine reverse_stack

end program solution

!   tMove = record
!              count     : integer;
!              fromstack : integer;
!              tostack   : integer;
!           end;
!
!
!
!! list the crates on a stack }
!
!function crates(stack : integer) : string;
!
!var
!   i : integer;
!
!begin
!   crates := '';
!   for i := 1 to dock[stack].depth do
!      crates := crates + dock[stack].crates[i];
!end;
!
!
!! invert a stack }
!
!procedure invert(stack : integer);
!
!var
!   i : integer;
!   s : string;
!
!begin
!   s := crates(stack);
!   empty(stack);
!   i := 0;
!   while length(s) - i > 0 do begin
!      push(stack, s[length(s) - i]);
!      i := i + 1;
!   end;
!end;
!
!
!! list the top crates on all the live stacks }
!
!function report : string;
!
!var
!   i : integer;
!   c : char;
!
!begin
!   report := '';
!   for i := 1 to MAXSTACKS do begin
!      c := peek(i);
!      report := report + c
!   end
!end;
!
!
!! initialize global variables to empty }
!
!procedure init;
!
!var
!   i : integer;
!
!begin
!   for i := 0 to MAXSTACKS do
!      empty(i);
!   for i := 1 to MAXMOVES do begin
!      moves[i].count := 0;
!      moves[i].fromstack := 0;
!      moves[i].tostack := 0;
!   end;
!   nummoves := 0;
!end;
!
!
!! load and parse the dataset }
!
!procedure load;
!
!var
!   i, j, k : integer;
!   s       : string;
!
!begin
!   init;
!   aocrewind;
!
!   ! load stacks }
!   ! crates are enclosed in [ ], a line without means the stacks are
!     loaded. stack number is identified by position on the line. }
!   i := 0;
!   while not aoceof do begin
!      s := aocread;
!      if strindex(s, '[') < 1 then
!         break;
!      i := i + 1;
!      j := 0;
!      while j < length(s) do begin
!         j := j + 1;
!         if s[j] in [' ', '[', ']'] then
!            continue;
!         push((j div 4) + 1, s[j]);
!      end;
!   end; ! while not eof }
!
!   ! the load pushed the crates on upside down, flip them. }
!   for i := 1 to MAXSTACKS do
!      invert(i);
!
!   ! load moves }
!   ! format is 'move <count> from <stack> to <stack>' }
!   i := 0;
!   while not eof(input) do begin
!      s := aocread;
!      if s = '' then
!         continue;
!      i := i + 1;
!      k := 6;       ! past 'move ' }
!      moves[i].count := str2int(s, k);
!      k := k + 6;   ! past ' from ' }
!      moves[i].fromstack := str2int(s, k);
!      k := k + 4;   ! past ' to ' }
!      moves[i].tostack := str2int(s, k);
!   end; ! while not eof }
!   nummoves := i;
!end;
!
!
!! perform the moves and report which crate is on top of each stack
!  when all the moves are complete. push the pop, repeatedly. }
!
!function partone : string;
!
!var
!   i, j : integer;
!
!begin
!   load;
!   for i := 1 to nummoves do
!      for j := 1 to moves[i].count do
!         push(moves[i].tostack, pop(moves[i].fromstack));
!   partone := report;
!end;
!
!
!! for part two we learn that the moves (pop-push) are multi-crate.
!  since the push-pop already works, i'll just use a temporary holding
!  stack to preserve ordering. }
!
!function partTwo : string;
!
!var
!   i, j :  integer;
!
!begin
!   load;
!   for i := 1 to nummoves do begin
!      for j := 1 to moves[i].count do
!         push(0, pop(moves[i].fromstack));
!      while depth(0) > 0 do
!         push(moves[i].tostack, pop(0));
!   end;
!   partTwo := report;
!end;
!
!
!! the mainline driver for a day's solution. }
!
!procedure mainline;
!
!var
!   s : string;
!
!begin
!   aocopen;
!   s := partone;
!   writeln('part one answer : ', s);
!   s := parttwo;
!   writeln('part two answer : ', s);
!   aocclose;
!end;
!
!! minimal boot to mainline }
!
!begin
!  mainline;
!end.
