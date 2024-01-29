! solution.f90 -- 2022 day 03 -- rucksack reorganization -- Troy Brumley blametroi@gmail.com

! the elves have packed poorly, silly elves.
!
! the input gives us the contents of a rucksack, which are evenly divided
! between two sides. per the problem specification there should be only one item
! type in common between the compartments.
!
! the items are identified by a unique priority code, a-zA-Z.
!
! the problem is a natural for sets but other than bit setting i don't see a set
! datatype in base fortran. i know there are ast libraries out there with sets
! but my goal is to practice programming, not librarying, so i'll roll my own
! for this problem.
!
! as with the pascal versions of these solutions, i'm seeing opportunities for
! reuse and even when i create something horrid at this point, i'm thinking
! about how to do it better. once i learn about modules and interface sections
! i'll be comfortable factoring out code and possibly creating reusable modules
! for i/o and other functions.
!
! one obvious improvement for readability is knowing when to use the if and do
! statements and not the constructs. they are distinct and there are times
! where if then/do ... end if/do can be cleanly and clearly collapsedd.

program solution

   implicit none

   ! file io
   integer, parameter  :: funitin = 10
   character(len=255)  :: fname = "./dataset.txt"
   integer             :: fstat
   character(len=512)  :: frecin

   ! application
   integer(kind=8)     :: cntone, cnttwo          ! work
   integer(kind=8)     :: partone, parttwo        ! results
   integer             :: item                    ! work
   integer             :: reclen
   logical             :: left(255), right(255)   ! a poor man's set of characters
   logical             :: first(255), second(255), third(255) ! for part two
   logical             :: dup12(255), dup23(255), dups(255)   ! ..

   ! in a real app we'd check for missing arguments
   call get_command_argument(1, fname)
   open (unit=funitin, file=trim(fname), access="stream", form="formatted", status="old", iostat=fstat)
   if (fstat /= 0) then ! ERROR
      print *, "error opening dataset = ", fstat
      stop
   end if

   ! initialize
   cntone = 0; cnttwo = 0; partone = 0; parttwo = 0

   ! for part one, identify the one item type that is duplicate in each rucksack
   ! and its priority value. sum these priorities and report the total.

   readone: do

      read (funitin, "(a)", iostat=fstat) frecin
      if (fstat < 0) exit ! end of file
      if (fstat > 0) then ! some other error
         print *, "error reading dataset = ", fstat
         stop
      end if

      cntone = cntone + 1

      reclen = len_trim(frecin)
      if (reclen < 1) cycle readone

      call clearset(left)
      call clearset(right)
      do item = 1, reclen/2
         call includeinset(left, frecin(item:item))
         call includeinset(right, frecin((item + reclen/2):(item + reclen/2)))
      end do

      call intersectsets(left, right, dups)
      if (countmembers(dups) /= 1) then
         print *, "error wrong number of dups found in record ", cntone, " [", trim(frecin), "]"
         stop
      end if

      do item = 1, 255
         if (dups(item)) then
            partone = partone + priorityvalue(achar(item))
            cycle readone
         end if
      end do

   end do readone

   ! for part two, consider every three elves as a group. find the item in common
   ! in all three rucksacks and sum their priorities.

   rewind (funitin, iostat=fstat)
   if (fstat /= 0) then
      print *, "error on rewind ", fstat
      stop
   end if

   readtwo: do

      call clearset(first)
      call clearset(second)
      call clearset(third)

      read (funitin, "(a)", iostat=fstat) frecin
      if (fstat < 0) exit ! end of file
      if (fstat > 0) then ! some other error
         print *, "error reading dataset = ", fstat
         stop
      end if

      cnttwo = cnttwo + 1

      reclen = len_trim(frecin)
      if (reclen < 1) cycle readtwo
      do item = 1, reclen
         call includeinset(first, frecin(item:item))
      end do

      read (funitin, "(a)", iostat=fstat) frecin
      if (fstat /= 0) then ! end of file not allowed here
         print *, "error reading dataset = ", fstat
         stop
      end if

      reclen = len_trim(frecin)
      if (reclen < 1) then
         print *, "error blank record found"
         stop
      end if
      do item = 1, reclen
         call includeinset(second, frecin(item:item))
      end do

      read (funitin, "(a)", iostat=fstat) frecin
      if (fstat /= 0) then ! end of file not allowed here
         print *, "error reading dataset = ", fstat
         stop
      end if

      reclen = len_trim(frecin)
      if (reclen < 1) then
         print *, "error blank record found"
         stop
      end if
      do item = 1, reclen
         call includeinset(third, frecin(item:item))
      end do

      call intersectsets(first, second, dup12)
      call intersectsets(second, third, dup23)
      call intersectsets(dup12, dup23, dups)
      if (countmembers(dups) /= 1) then
         print *, "error wrong number of dups found in group ", cnttwo, " [", trim(frecin), "]"
         stop
      end if

      do item = 1, 255
         if (dups(item)) then
            parttwo = parttwo + priorityvalue(achar(item))
            cycle readtwo
         end if
      end do

   end do readtwo

   ! report and close
   print *
   print *, cntone, " records read "
   print *, cnttwo, " grops read "
   print *, partone, " part one "
   print *, parttwo, " part two "
   print *

   close (funitin)

contains

   ! i am getting several warnings about variables masking those in the parent
   ! scope. the answer is to use a module but i haven't learned them yet, so
   ! i'll rename what i can and live with the warnings for what i can't for now.

   function priorityvalue(c)
      implicit none
      character(len=1), intent(in) :: c
      integer                      :: priorityvalue

      if (c >= 'a' .and. c <= 'z') then
         priorityvalue = ichar(c) - ichar('a') + 1
      else if (c >= 'A' .and. c <= 'Z') then
         priorityvalue = ichar(c) - ichar('A') + 27
      else
         priorityvalue = 0
      end if
   end function priorityvalue

   function countmembers(s)
      implicit none
      logical, intent(in) :: s(255)
      integer             :: i
      integer             :: countmembers

      countmembers = 0
      do i = 1, 255
         if (s(i)) countmembers = countmembers + 1
      end do
   end function countmembers

   subroutine clearset(s)
      implicit none
      logical, intent(inout) :: s(255)
      integer                :: i
      do i = 1, 255
         s(i) = .false.
      end do
   end subroutine clearset

   subroutine includeinset(s, c)
      implicit none
      logical, intent(inout)       :: s(255)
      character(len=1), intent(in) :: c

      s(ichar(c)) = .true.
   end subroutine includeinset

   subroutine intersectsets(s1, s2, res)
      implicit none
      logical, intent(in)    :: s1(255), s2(255)
      logical, intent(inout) :: res(255)
      integer                :: i
      do i = 1, 255
         res(i) = s1(i) .and. s2(i)
      end do
   end subroutine intersectsets

   subroutine printset(s)
      implicit none
      logical, intent(in) :: s(255)
      character(len=255)  :: str
      integer             :: i
      str = ""
      do i = 1, 255
         if (s(i)) str = trim(str)//achar(i)
      end do
      print *, str
   end subroutine printset

end program solution
