! solution.f90 -- 2022 day 6 -- tuning trouble -- troy brumley blametroi@gmail.com

! identify start markers in streams of characters.
! you receive what looks like gibberish but there is a structure to
! the stream. for part one, what is the number of characters from
! beginning of the buffer to the end of the first start-of-packet
! marker. the start of packet marker is any sequence of four
! characters that are all different.
!
! for part two a start-of-message marker is the target instead of a
! start-of-packet. this is a sequence of fourteen (14) distinct
! characters instead of four.

program solution

   use iso_fortran_env, only: int64
   use iso_c_binding, only: c_int8_t
   use aocinput, only: open_aoc_input_byte, read_aoc_input_byte, close_aoc_input, rewind_aoc_input

   implicit none

   integer, parameter            :: AOCIN = 10
   integer, parameter            :: START_PACKET_LEN = 4
   integer, parameter            :: START_MESSAGE_LEN = 14
   integer(kind=int64)           :: part_one, part_two  ! results
   integer                       :: i                   ! work
   integer(kind=c_int8_t)        :: b
   integer(kind=c_int8_t)        :: packet_mark_buf(START_PACKET_LEN)
   integer(kind=c_int8_t)        :: message_mark_buf(START_MESSAGE_LEN)

   ! input file is argument 1
   call open_aoc_input_byte(AOCIN)

   ! initialize
   part_one = 0; part_two = 0

   ! clear start marker buffers
   packet_mark_buf = 0
   message_mark_buf = 0

   ! parts one and two differ only in the length of the packet signature. the
   ! packet for part one has to come before part two, so the combination is
   ! easy. keep accumulating message bytes in the circular buffers and once the
   ! part two marker is recognized, we're done.

   i = 0
   do while (read_aoc_input_byte(AOCIN, b) .and. (part_two == 0))
      i = i + 1
      packet_mark_buf(mod(i, START_PACKET_LEN) + 1) = b
      message_mark_buf(mod(i, START_MESSAGE_LEN) + 1) = b
      if (part_one == 0) then
         if (is_buffer_mark(packet_mark_buf)) part_one = i
      end if
      if (is_buffer_mark(message_mark_buf)) part_two = i
   end do

   ! report and close
   print *
   print *, part_one, "part one"
   print *, part_two, "part two"
   print *

   call close_aoc_input(AOCIN)

contains

   ! are there any duplicate bytes in the buffer? relies on the input stream
   ! containing no null bytes. the buffer is circular but we always scan from
   ! index 1 not index oldest.
   function is_buffer_mark(ab)
      implicit none
      logical :: is_buffer_mark
      integer(kind=c_int8_t), intent(in) :: ab(:)
      integer :: vi, vj
      integer(kind=c_int8_t) :: vb
      is_buffer_mark = .true.
      outer: do vi = 1, size(ab)
         vb = ab(vi)
         if (vb == 0) then
            is_buffer_mark = .false.
            exit outer
         end if
         inner: do vj = 1, size(ab)
            if (vj == vi) cycle inner
            if (ab(vj) /= vb) cycle inner
            is_buffer_mark = .false.
            exit outer
         end do inner
      end do outer
   end function is_buffer_mark

end program solution
