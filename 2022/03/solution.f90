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
! one obvious improvement for readability is knowing when to use the if and
! implied do statements and not the constructs. they are distinct and there are
! times where if then ... end if can be cleanly and clearly collapsedd.

program solution

   use charset

   implicit none

   ! file io
   integer, parameter  :: funitin = 10
   character(len=255)  :: fname = "./dataset.txt"
   integer             :: fstat

   ! application
   integer(kind=8)     :: cntone, cnttwo          ! work
   integer(kind=8)     :: partone, parttwo        ! results
   integer             :: item, i                 ! work
   integer             :: reclen
   character(len=512)  :: recin
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

      read (funitin, "(a)", iostat=fstat) recin
      if (fstat < 0) exit ! end of file
      if (fstat > 0) then ! some other error
         print *, "error reading dataset = ", fstat
         stop
      end if

      cntone = cntone + 1

      reclen = len_trim(recin)
      if (reclen < 1) cycle readone

      call clear_set(left)
      call clear_set(right)
      do item = 1, reclen/2
         call include_in_set(left, recin(item:item))
         call include_in_set(right, recin((item + reclen/2):(item + reclen/2)))
      end do

      call intersection_of_sets(left, right, dups)
      if (size_of_set(dups) /= 1) then
         print *, "error wrong number of dups found in record ", cntone, " [", trim(recin), "]"
         stop
      end if

      do item = 1, 255
         if (dups(item)) then
            partone = partone + priorityvalue(achar(item))
            cycle readone
         end if
      end do

   end do readone

   ! for part two, consider every three elves as a group. find the item in
   ! common in all three rucksacks and sum their priorities.

   rewind (funitin, iostat=fstat)
   if (fstat /= 0) then
      print *, "error on rewind ", fstat
      stop
   end if

   readtwo: do

      call clear_set(first)
      call clear_set(second)
      call clear_set(third)

      read (funitin, "(a)", iostat=fstat) recin
      if (fstat < 0) exit ! end of file
      if (fstat > 0) then ! some other error
         print *, "error reading dataset = ", fstat
         stop
      end if

      cnttwo = cnttwo + 1

      reclen = len_trim(recin)
      if (reclen < 1) cycle readtwo
      do item = 1, reclen
         call include_in_set(first, recin(item:item))
      end do

      read (funitin, "(a)", iostat=fstat) recin
      if (fstat /= 0) then ! end of file not allowed here
         print *, "error reading dataset = ", fstat
         stop
      end if

      reclen = len_trim(recin)
      if (reclen < 1) then
         print *, "error blank record found"
         stop
      end if
      do item = 1, reclen
         call include_in_set(second, recin(item:item))
      end do

      read (funitin, "(a)", iostat=fstat) recin
      if (fstat /= 0) then ! end of file not allowed here
         print *, "error reading dataset = ", fstat
         stop
      end if

      reclen = len_trim(recin)
      if (reclen < 1) then
         print *, "error blank record found"
         stop
      end if
      do item = 1, reclen
         call include_in_set(third, recin(item:item))
      end do

      call intersection_of_sets(first, second, dup12)
      call intersection_of_sets(second, third, dup23)
      call intersection_of_sets(dup12, dup23, dups)
      if (size_of_set(dups) /= 1) then
         print *, "error wrong number of dups found in group ", cnttwo, " [", trim(recin), "]"
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
   print *, cnttwo, " groups read "
   print *, partone, " part one "
   print *, parttwo, " part two "
   print *

   close (funitin)

contains

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

end program solution
