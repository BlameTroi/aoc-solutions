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
   integer                    :: i                   ! work
   character(len=max_aoc_reclen) :: rec
   character(len=MAX_STACKS)  :: report_one, report_two

   rec = ""
   call open_aoc_input(AOCIN)

   ! initialize
   part_one = 0; part_two = 0
   report_one = ""; report_two = ""

   ! load initial stacks
   call load_stacks

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
   call report_stacks(report_one)

   ! reload and reprocess using the rules for part two
   call load_stacks

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
   call report_stacks(report_two)

   ! report and close
   print *
   print *, report_one, " part one"
   print *, report_two, " part two"
   print *

   call close_aoc_input(AOCIN)

contains

   ! load the crates onto the stacks on the dock. the input is a visual
   ! representation and the some work needs to be done to get everything in the
   ! right order.
   !
   !a sample of the input:
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
   !
   ! once complete, the input stream is positioned so the next read will return
   ! a move directive.
   !
   ! updates dock and buffer directly.
   subroutine load_stacks
      implicit none
      integer                       :: vj, vk
      character(len=max_aoc_reclen) :: vrec
      vrec = ""
      call rewind_aoc_input(AOCIN)
      call clear_stacks
      do while (read_aoc_input(AOCIN, vrec))
         if (len_trim(vrec) == 0) exit     ! blank line is end of this section
         if (scan(vrec, "[") == 0) cycle   ! ignore this line
         vj = 0;
         do vk = 1, len_trim(vrec), 4      ! "[x] "
            vj = vj + 1
            if (vrec(vk:vk) == "[") then
               call push_stack(dock(vj), vrec(vk + 1:vk + 1))
            end if
         end do
      end do
      call reverse_stacks
   end subroutine load_stacks

   ! reset the stacks in the dock and buffer stack.
   subroutine clear_stacks
      implicit none
      integer                 :: vi
      do vi = 1, MAX_STACKS
         call clear_stack(dock(vi))
      end do
      call clear_stack(buffer)
   end subroutine

   ! reverse the contents of the stacks in the dock in place.
   subroutine reverse_stacks
      implicit none
      integer                 :: vi
      do vi = 1, MAX_STACKS
         call reverse_stack(dock(vi))
      end do
   end subroutine reverse_stacks

   ! returns a string with the id of the top cargo crate of all the stacks on
   ! the dock.
   subroutine report_stacks(ar)
      implicit none
      character(len=MAX_STACKS), intent(out) :: ar
      integer                   :: vi
      ar = ""
      do vi = 1, MAX_STACKS
         if (peek_stack(dock(vi)) == " ") then
            ar = trim(ar)//"-"
         else
            ar = trim(ar)//peek_stack(dock(vi))
         end if
      end do
   end subroutine report_stacks

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

   ! make the stack empty
   subroutine clear_stack(astk)
      implicit none
      type(t_stack), intent(out) :: astk
      integer                 :: vi
      do vi = 1, MAX_DEPTH
         astk % crates(vi) = " "
      end do
      astk % depth = 0
   end subroutine clear_stack

   ! returns a copy of the top item on the stack
   function peek_stack(astk)
      implicit none
      type(t_stack), intent(in) :: astk
      character(len=1)        :: peek_stack
      if (astk % depth /= 0) then
         peek_stack = astk % crates(astk % depth)
      else
         peek_stack = " "
      end if
   end function peek_stack

   ! client code should not access members directly
   function depth_stack(astk)
      implicit none
      type(t_stack), intent(in) :: astk
      integer                 :: depth_stack
      depth_stack = astk % depth
   end function depth_stack

   ! remove and return the id of the top of the stack, or blank if empty
   ! with no error signal.
   function pop_stack(astk)
      implicit none
      type(t_stack), intent(inout) :: astk
      character(len=1)        :: pop_stack
      pop_stack = " "
      if (astk % depth > 0) then
         pop_stack = astk % crates(astk % depth)
         astk % crates(astk % depth) = " "
         astk % depth = astk % depth - 1
      end if
   end function pop_stack

   ! push a new crate onto the stack. there is no error checking.
   subroutine push_stack(astk, ac)
      implicit none
      type(t_stack), intent(inout) :: astk
      character(len=1), intent(in) :: ac
      astk % depth = astk % depth + 1
      astk % crates(astk % depth) = ac
   end subroutine push_stack

   ! reverse the order of items on a stack in place
   subroutine reverse_stack(astk)
      implicit none
      type(t_stack), intent(inout) :: astk
      character(len=1)        :: vc
      integer                 :: vi
      do vi = 1, astk % depth / 2
         vc = astk % crates(vi)
         astk % crates(vi) = astk % crates(astk % depth + 1 - vi)
         astk % crates(astk % depth + 1 - vi) = vc
      end do
   end subroutine reverse_stack

end program solution
