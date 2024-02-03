! solution.f90 -- 2022 day 07 -- no space left on device -- troy brumley blametroi@gmail.com
!
! advent of code 2022 day 7 -- no space left on device. given a log of a shell
! session (cd, ls, the output of same) find space on a file system.
!
! our input is a log of a shell session with commands and output. the only
! commands issued are cd and ls. output of the ls commands is one entry per line
! of input, dir name or <integer size> name. parsing this will yield a file
! table for the device that can be analyzed to find entity sizes for the problem
! solution.

program solution

   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input

   implicit none

   integer, parameter             :: AOCIN = 10
   integer, parameter             :: MAX_ENTRIES = 1000         ! observed max
   integer(kind=int64), parameter :: FILESYSTEM_SIZE = 70000000 ! spec
   integer(kind=int64), parameter :: NEEDED_SIZE = 30000000     ! spec
   integer(kind=int64)            :: part_one, part_two         ! results
   integer                        :: i                          ! work

   type entry_t
      character(len=64)   :: name   ! file or directory name
      logical             :: isdir  ! well, is it?
      integer(kind=int64) :: len    ! bytes
      integer             :: parent ! entry subscript
   end type entry_t

   type(entry_t), dimension(0:MAX_ENTRIES) :: direntry ! and one of the few times i'll use a 0 base
   integer :: numentries, curdir, newent, candidate
   character(len=64) :: tempname
   integer(kind=int64) :: totalsize, freesize, targetsize, minsize
   character(len=max_aoc_reclen)  :: rec

   ! initialize
   !
   ! set root directory and clear all other entries
   direntry(0)%name = "/"
   direntry(0)%isdir = .true.
   direntry(0)%len = 0
   direntry(0)%parent = -1
   do i = 1, MAX_ENTRIES
      direntry(i)%name = ""
      direntry(i)%isdir = .false.
      direntry(i)%len = 0
      direntry(i)%parent = -1
   end do
   numentries = 1
   totalsize = 0; freesize = 0; targetsize = 0
   part_one = 0; part_two = 0
   rec = ""

   ! input file is argument 1
   call open_aoc_input(AOCIN)

   i = 0               ! slot in directory entry list
   curdir = -1         ! cwd is invalid
   reader: do while (read_aoc_input(AOCIN, rec))

      if (len_trim(rec) == 0) cycle           ! ignore blank lines
      if (rec(1:1) == "#") cycle              ! and comments
      if (trim(rec) == "$ cd /") then         ! set cwd to root
         curdir = 0
         cycle
      end if

      if (curdir < 0) then
         print *, "*** error *** working directory not set at '", trim(rec), "'"
         cycle
      end if

      ! up one
      if (trim(rec) == "$ cd ..") then
         curdir = direntry(curdir)%parent
         if (curdir < 0) print *, "*** error *** invalid directory parent at '", trim(rec), "'"
         cycle
      end if

      ! change to
      if (rec(1:5) == "$ cd ") then
         tempname = rec(6:)
         do i = 0, numentries - 1
            if ((direntry(i)%name == tempname) .and. (direntry(i)%isdir) .and. (direntry(i)%parent == curdir)) then
               curdir = i
               rec = "ok"
               exit
            end if
         end do
         if (rec /= "ok") print *, "*** error *** no matching directory found '", tempname, "'"
         cycle
      end if

      ! skip over ls
      if (rec == "$ ls") cycle

      ! from here the lines are output from an ls command. parse them to build
      ! file table entries.

      if (rec(1:4) == "dir ") then
         newent = numentries
         numentries = numentries + 1
         direntry(newent)%name = rec(5:)
         direntry(newent)%isdir = .true.
         direntry(newent)%parent = curdir
         cycle
      end if

      if (rec(1:1) >= "0" .and. rec(1:1) <= "9") then
         newent = numentries
         numentries = numentries + 1
         direntry(newent)%isdir = .false.
         direntry(newent)%parent = curdir
         read (rec, *) direntry(newent)%len, direntry(newent)%name
         cycle
      end if

      print *, "*** error *** not understood '", rec, "'"

   end do reader

   ! file table built but the length under each directory needs to be calculated
   direntry(0)%len = length_under(0)

   ! for part one, what is the total of the sizes of all directories that contain
   ! less than 100000 bytes?

   part_one = 0
   totalsize = 0
   do i = 0, numentries - 1
      if (.not. direntry(i)%isdir) then
         totalsize = totalsize + direntry(i)%len
      else if (direntry(i)%len < 100000) then
         part_one = part_one + direntry(i)%len
      end if
   end do

   ! for part two find the smallest directory that, if deleted, would free up enough
   ! space to run an update. we need 30000000 to run it.

   freesize = FILESYSTEM_SIZE - totalsize
   targetsize = NEEDED_SIZE - freesize
   candidate = 0
   minsize = totalsize

   do i = 1, numentries - 1 ! no need to check root
      if (.not. direntry(i)%isdir) cycle
      if (direntry(i)%len < targetsize) cycle
      if (direntry(i)%len >= minsize) cycle
      minsize = direntry(i)%len
      candidate = i
   end do

   part_two = direntry(candidate)%len

   ! report and close
   print *
   print *, part_one, "part one"
   print *, part_two, "part two"
   print *

   call close_aoc_input(AOCIN)

contains

   ! chase down all children and sum their sizes
   recursive function length_under(ad) result(rlen)
      integer, intent(in) :: ad
      integer(kind=int64) :: rlen
      integer :: vi
      rlen = 0
      do vi = 0, numentries - 1
         if (direntry(vi)%parent /= ad) cycle
         if (direntry(vi)%isdir) direntry(vi)%len = length_under(vi)
         rlen = rlen + direntry(vi)%len
      end do
   end function length_under

   ! chase up the parents to build the full path name
   recursive function full_path(ad) result(rpath)
      integer, intent(in) :: ad
      character(len=512) :: rpath
      if (direntry(ad)%parent == -1) then
         rpath = direntry(ad)%name
      else
         rpath = trim(full_path(direntry(ad)%parent))//trim(direntry(ad)%name)//"/"
      end if
   end function full_path

end program solution
!
!   { recursively determine size of directories }
!   direntry[0].len := lengthunder(0);
!
!end;
!
!
!{ part one did all the initial calculations. a total of 30000000
!  bytes of space is needed to run the update. find the one
!  directory to delete to get enough space. }
!
!function parttwo : integer;
!
!var
!end;
