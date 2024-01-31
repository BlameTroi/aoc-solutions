! aocinput.f90 -- standard i/o for advent of code -- troy brumley blametroi@gmail.com

! my standard io for advent of code. read records a one large string. size is
! arbitrarily set to 1k. i've seen a few datasets with lines longer than 255,
! but none longer than 1024 (yet).
!
! pass the unit number from the caller.
!
! limited error handling, this isn't production code. in general, if an error is
! recognized, print the status code and stop the program.

module aocinput

   implicit none

   private

   public :: open_aoc_input
   public :: close_aoc_input
   public :: rewind_aoc_input
   public :: read_aoc_input

contains

   ! the dataset name is the first command line argument. open that file for
   ! reading. prints whether or not the file was opened on stdout.
   subroutine open_aoc_input(u)
      implicit none
      character(len=255)  :: fname = "./dataset.txt"
      integer, intent(in) :: u
      integer             :: fstat

      call get_command_argument(1, fname)
      open (unit=u, file=trim(fname), access="stream", form="formatted", status="old", iostat=fstat)
      if (fstat /= 0) then ! ERROR
         print *, "error opening aoc dataset '", trim(fname), "': ", fstat
         stop
      end if

      print *, "aoc dataset '", trim(fname), "' opened for reading"
   end subroutine open_aoc_input

   ! close the file, quietly ignoring errors.
   subroutine close_aoc_input(u)
      implicit none
      integer, intent(in) :: u

      close (u)
      print *, "aoc dataset closed"
   end subroutine close_aoc_input

   ! it's a tape drive! reposition to first record.
   subroutine rewind_aoc_input(u)
      implicit none
      integer, intent(in) :: u
      integer             :: fstat

      rewind (u, iostat=fstat)
      if (fstat /= 0) then
         print *, "error on rewind of aoc dataset: ", fstat
         stop
      end if
   end subroutine rewind_aoc_input

   ! read the next record/line from the aoc dataset on unit u into the character
   ! field r. length is hard coded. returns .true. if a record was read, and
   ! .false. if at end of file.
   function read_aoc_input(u, r)
      implicit none
      integer, intent(in) :: u
      character(len=1024), intent(out) :: r
      integer             :: fstat
      logical             :: read_aoc_input

      read (u, "(a)", iostat=fstat) r
      if (fstat > 0) then ! some other error
         print *, "error reading aoc dataset: ", fstat
         stop
      end if

      read_aoc_input = fstat == 0
   end function read_aoc_input

end module aocinput
