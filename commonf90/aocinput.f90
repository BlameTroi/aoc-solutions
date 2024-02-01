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
   use iso_fortran_env, only: iostat_end, error_unit

   implicit none

   private

   public :: open_aoc_input
   public :: close_aoc_input
   public :: rewind_aoc_input
   public :: read_aoc_input

   public :: max_aoc_reclen
   integer, parameter  :: max_aoc_reclen = 1024

   integer             :: ioerr
   character(len=512)  :: iotxt

contains

   ! the dataset name is the first command line argument. open that file for
   ! reading. prints whether or not the file was opened on stdout.
   subroutine open_aoc_input(u)
      implicit none
      character(len=255)  :: fname
      integer, intent(in) :: u

      call get_command_argument(1, fname)
      open (unit=u, file=trim(fname), access="stream", form="formatted", status="old", iostat=ioerr, iomsg=iotxt)
      if (ioerr /= 0) call report_aoc_error("openening "//trim(fname), ioerr, iotxt)

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

      rewind (u, iostat=ioerr, iomsg=iotxt)
      if (ioerr /= 0) call report_aoc_error("rewind", ioerr, iotxt)
   end subroutine rewind_aoc_input

   ! read the next record/line from the aoc dataset on unit u into the character
   ! field r. length is hard coded. returns .true. if a record was read, and
   ! .false. if at end of file.
   function read_aoc_input(u, r)
      implicit none
      integer, intent(in)  :: u
      character(len=1024), intent(out) :: r
      logical                          :: read_aoc_input

      read (u, "(a)", iostat=ioerr, iomsg=iotxt) r
      if (ioerr == iostat_end) then
         read_aoc_input = .false.
      else if (ioerr /= 0) then
         call report_aoc_error("read", ioerr, iotxt)
      else
         read_aoc_input = .true.
      end if
   end function read_aoc_input

   subroutine report_aoc_error(func, err, msg)
      implicit none
      integer, intent(in)      :: err
      character(*), intent(in) :: func, msg
      write (error_unit, *) "error on ", trim(func), " iostat:", err, " iomsg: ", trim(msg)
      stop 1
   end subroutine report_aoc_error

end module aocinput
