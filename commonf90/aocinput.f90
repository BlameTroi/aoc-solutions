! aocinput.f90 -- standard i/o for advent of code -- troy brumley blametroi@gmail.com

! my standard io for advent of code. read records a one large string. size is
! arbitrarily set to 1k. i've seen a few datasets with lines longer than 255,
! but none longer than 1024 (yet).
!
! update: a 4k single line file has been seen! setting the max to 8K and also
! adding a byte reader.
!
! pass the unit number from the caller.
!
! limited error handling, this isn't production code. in general, if an error is
! recognized, print the status code and stop the program.

module aocinput
   use iso_fortran_env, only: iostat_end, error_unit
   use iso_c_binding, only: c_int8_t

   implicit none

   private

   public :: open_aoc_input
   public :: open_aoc_input_byte
   public :: close_aoc_input
   public :: rewind_aoc_input
   public :: read_aoc_input
   public :: read_aoc_input_byte

   public :: max_aoc_reclen
   integer, parameter  :: max_aoc_reclen = 8192

   logical, save       :: opened = .false.
   logical, save       :: asbyte = .false.

   integer             :: ioerr
   character(len=512)  :: iotxt

contains

   ! the dataset name is the first command line argument. open that file for
   ! reading. prints whether or not the file was opened on stdout.
   subroutine open_aoc_input(u)
      implicit none
      character(len=255)  :: fname
      integer, intent(in) :: u

      if (opened) call report_aoc_error("open", -1, "file already opened!")

      call get_command_argument(1, fname)
      open (unit=u, file=trim(fname), access="stream", form="formatted", status="old", iostat=ioerr, iomsg=iotxt)
      if (ioerr /= 0) call report_aoc_error("opening "//trim(fname), ioerr, iotxt)

      opened = .true.
      asbyte = .false.

      print *, "aoc dataset '", trim(fname), "' opened for reading"
   end subroutine open_aoc_input

   ! the dataset name is the first command line argument. open that file for
   ! reading. prints whether or not the file was opened on stdout.
   subroutine open_aoc_input_byte(u)
      implicit none
      character(len=255)  :: fname
      integer, intent(in) :: u

      if (opened) call report_aoc_error("open", -1, "file already opened!")

      call get_command_argument(1, fname)
      open (unit=u, file=trim(fname), access="stream", form="unformatted", status="old", iostat=ioerr, iomsg=iotxt)
      if (ioerr /= 0) call report_aoc_error("opening "//trim(fname), ioerr, iotxt)

      opened = .true.
      asbyte = .true.

      print *, "aoc dataset '", trim(fname), "' opened for reading"
   end subroutine open_aoc_input_byte

   ! close the file, quietly ignoring runtime errors.
   subroutine close_aoc_input(u)
      implicit none
      integer, intent(in) :: u

      if (.not. opened) call report_aoc_error("close", -1, "file not open")

      close (u)

      opened = .false.
      asbyte = .false.

      print *, "aoc dataset closed"
   end subroutine close_aoc_input

   ! it's a tape drive! reposition to first record.
   subroutine rewind_aoc_input(u)
      implicit none
      integer, intent(in) :: u

      if (.not. opened) call report_aoc_error("rewind", -1, "file not open")

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

      if (.not. opened) call report_aoc_error("read", -1, "file not open")
      if (asbyte) call report_aoc_error("read", -1, "file not opened for formatted access")

      read (u, "(a)", iostat=ioerr, iomsg=iotxt) r
      if (ioerr == iostat_end) then
         read_aoc_input = .false.
      else if (ioerr /= 0) then
         call report_aoc_error("read", ioerr, iotxt)
      else
         read_aoc_input = .true.
      end if
   end function read_aoc_input

   ! read the next byte from the aoc dataset on unit u into the field b.
   ! returns .true. if a byte was read, and .false. if at end of file.
   function read_aoc_input_byte(u, b)
      implicit none
      integer, intent(in)            :: u
      integer(c_int8_t), intent(out) :: b
      logical                        :: read_aoc_input_byte

      if (.not. opened) call report_aoc_error("read", -1, "file not open")
      if (.not. asbyte) call report_aoc_error("read", -1, "file not opened for byte access")

      read (u, iostat=ioerr, iomsg=iotxt) b
      if (ioerr == iostat_end) then
         read_aoc_input_byte = .false.
      else if (ioerr /= 0) then
         call report_aoc_error("read", ioerr, iotxt)
      else
         read_aoc_input_byte = .true.
      end if
   end function read_aoc_input_byte

   subroutine report_aoc_error(func, err, msg)
      implicit none
      integer, intent(in)      :: err
      character(*), intent(in) :: func, msg
      write (error_unit, *) "error on ", trim(func), " iostat:", err, " iomsg: ", trim(msg)
      stop 1
   end subroutine report_aoc_error

end module aocinput
