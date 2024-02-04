! solution.f90 -- 2022 day 10 -- cathode ray tube -- troy brumley blametroi@gmail.com

! advent of code 2022 day 10 -- cathode ray tube
!
! simulate a program execution to drive a small crt display
!
!   noop
!   addx 3
!   addx - 5
!
! initial x = 1
! addx takes 2 cycles to complete, at the end of the cycles, x is
! increased by value(which can be negative) noop takes 1 cycle
! and has no other effect
!
! for part one we take a sum of the signal strength(x*cycles),
! and for part two we use the value of x combined with the cycle
! to determine if a pixel on the display should be lit.

program solution

   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input

   implicit none

   integer, parameter            :: AOCIN = 10
   integer(kind=int64)           :: part_one, part_two  ! results
   character(len=max_aoc_reclen) :: rec

   ! constants to hopefully improve readability
   character(len=1), parameter   :: OPC_NOP = 'n'
   character(len=1), parameter   :: OPC_ADX = 'a'
   character(len=1), parameter   :: OPC_INV = '-'
   character(len=1), parameter   :: OPC_EOJ = '|'
   character(len=1), parameter   :: PIXEL_ON = '#'
   character(len=1), parameter   :: PIXEL_OFF = '.'
   integer, parameter            :: MAXLEN = 150          ! observed
   integer, parameter            :: SCANLINE = 40         ! spec
   integer, parameter            :: CYCLES = 6*SCANLINE   ! spec

   ! 'assembled' program and trace of execution
   type :: trace_t
      character(len=1) :: opcode
      integer          :: oprand
      integer          :: scycle
      integer          :: ecycle
      integer          :: sx
      integer          :: ex
   end type trace_t

   ! execution
   type(trace_t)       :: trace(-1:MAXLEN)
   character(len=1)    :: pixels(0:CYCLES - 1)

   rec = ""
   call open_aoc_input(AOCIN)

   ! initialize
   part_one = 0; part_two = 0

   ! for part one, sum signal strengths at 20, 60, 80, 120, 140, 180, and 220 cycles.
   call load
   call runner
   part_one = sigstr(20) + sigstr(60) + sigstr(100) + sigstr(140) + sigstr(180) + sigstr(220)

   ! for part two show what would be displayed on the screen.with each cycle the
   ! scan position is positioned on a pixel.if the sprite(a 3 pixel wide area,
   ! centered by the current value of x) has any of its area over the a pixel at
   ! that time, the pixel is turned on.
   !
   ! this is the first time that a part answer hasn't been an integer.

   call load
   call runner
   call scanner
   call printscreen

   ! report and close
   print *
   print *, part_one, "part one"
   print *, part_two, "part two"
   print *

   call close_aoc_input(AOCIN)

contains

   ! init, a 'hardware reset'. the program storage and trace are set to
   ! predictable values and the x register is seeded with its defined starting
   ! value. the 'display' is also cleared.
   subroutine init
      implicit none
      integer :: i
      do i = -1, MAXLEN
         trace(i)%opcode = OPC_INV
         trace(i)%oprand = 0
         trace(i)%scycle = 0
         trace(i)%ecycle = 0
         trace(i)%sx = 0
         trace(i)%ex = 0
      end do
      trace(-1)%sx = 1
      trace(-1)%ex = 1
      pixels = PIXEL_OFF
   end subroutine init

   ! read and 'assemble' the program into storage. add an 'end' instruction to
   ! mark the end of the load.
   subroutine load
      implicit none
      integer :: i
      character(len=16) :: opc
      call init
      call rewind_aoc_input(AOCIN)
      i = -1
      do while (read_aoc_input(AOCIN, rec))
         if (len_trim(rec) == 0) cycle
         if (rec(1:1) == "#") cycle
         i = i + 1
         read (rec, *) opc
         trace(i)%opcode = opc(1:1)
         if (opc(1:1) == OPC_NOP) cycle
         read (rec, *) opc, trace(i)%oprand
      end do
      trace(i + 1)%opcode = OPC_EOJ
   end subroutine load

   ! execute the program and update the trace to show cycles used and the value
   ! of x at the start and end of the instruction.
   subroutine runner
      implicit none
      integer :: i
      i = -1
      do while (trace(i)%opcode /= OPC_EOJ)
         i = i + 1
         trace(i)%scycle = trace(i - 1)%ecycle
         trace(i)%sx = trace(i - 1)%ex
         select case (trace(i)%opcode)
         case (OPC_INV)
            print *, "*** error *** invlaid opcode at:", i
            stop 1
         case (OPC_NOP)
            trace(i)%ex = trace(i)%sx
            trace(i)%ecycle = trace(i)%scycle + 1
         case (OPC_ADX)
            trace(i)%ex = trace(i)%sx + trace(i)%oprand
            trace(i)%ecycle = trace(i)%scycle + 2
         end select
      end do
   end subroutine runner

   ! find the value of x at a certain cycle time.
   function xat(c)
      implicit none
      integer, intent(in) :: c
      integer :: xat
      integer :: i
      i = -1
      do
         i = i + 1
         if (trace(i)%opcode == OPC_EOJ) then
            print *, "*** warning *** program did not run long enough"
            exit
         end if
         if ((c >= trace(i)%scycle) .and. (c <= trace(i)%ecycle)) exit
      end do
      xat = trace(i)%sx
   end function xat

   ! return the signal strength at a particular cycle time. that is, the
   ! product of x and the cycle time.
   function sigstr(c)
      implicit none
      integer, intent(in) :: c
      integer sigstr
      sigstr = xat(c)*c
   end function sigstr

   ! simulate the scan line and sprite moving across the display
   ! and use the x value as per the part two rules to determine
   ! if a pixel should be on or off.
   subroutine scanner
      implicit none
      integer :: i, x, o
      do i = 1, 240
         x = xat(i)
         o = mod(i, 40) - 1 ! sprite horizontal location at this cycle
         if ((o > x - 2) .and. (o < x + 2)) then
            pixels(i - 1) = PIXEL_ON
         else
            pixels(i - 1) = PIXEL_OFF
         end if
      end do
   end subroutine scanner

   ! print the screen face as it would appear based on the part two
   ! instructions.
   subroutine printscreen
      implicit none
      integer :: i
      character(len=SCANLINE + 2) :: buffer
      print *
      print *, "screen:"
      buffer = ""
      do i = 0, CYCLES - 1
         if (mod(i, SCANLINE) == 0) then
            print *, trim(buffer)
            buffer = ""
         end if
         buffer = trim(buffer)//pixels(i)
      end do
      print *, trim(buffer)
      print *
   end subroutine printscreen

end program solution
