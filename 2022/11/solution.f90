! solution.f90 -- 2022 day 11 -- monkey business -- troy brumley blametroi@gmail.com

! advent of code 2022 day 11 -- monkey business -- which monkeys have your stuff?
!
! all your stuff fell out of the your pack and monkeys are tossing it around. the
! monkeys pay attention to your anxiety level, devious monkeys.

program solution

   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input

   implicit none

   integer, parameter            :: AOCIN = 10
   integer(kind=int64)           :: part_one, part_two  ! results
   character(len=max_aoc_reclen) :: rec

   ! input should be a block for each monkey as follows.
   character(len=*), parameter :: cmonkey = "Monkey "                      ! n: (0-9)
   integer, parameter :: lcmonkey = 7
   character(len=*), parameter :: cstart = "  Starting items: "            ! n (, n)*
   integer, parameter :: lcstart = 18
   character(len=*), parameter :: coper = "  Operation: new = old "        ! [+*] n
   integer, parameter :: lcoper = 23
   character(len=*), parameter :: ctest = "  Test: divisible by "          ! n
   integer, parameter :: lctest = 21
   character(len=*), parameter :: cift = "    If true: throw to monkey "   ! n
   integer, parameter :: lcift = 29
   character(len=*), parameter :: ciff = "    If false: throw to monkey "  ! n
   integer, parameter :: lciff = 30
   character(len=*), parameter :: cold = " old"
   integer, parameter :: lcold = 4

   ! constraints or limits
   integer, parameter             :: MAXITEMS = 36         ! 10 test, 36 live
   integer, parameter             :: LASTMONKEY = 7        ! 0-3 test, 0-7 live
   integer, parameter             :: MINROUNDS = 20        ! for both test and live
   integer, parameter             :: MAXROUNDS = 10000     ! for both test and live
   integer(kind=int64), parameter :: CRELIEF = 3           ! a scaling value for min rounds

   ! runtime representation of the monkey and items
   type monkey_t
      character(len=1)              :: id              ! we'll actually use the subscript to id these
      integer                       :: numitems        ! how many
      integer(kind=int64), dimension(1:MAXITEMS) :: priority
      character(len=1)              :: operation       ! multiply or add
      integer                       :: opervalue       ! 0 means old, otherwise integer
      integer                       :: testdivisor     ! priority test value
      integer                       :: throwtrue       ! send there if true
      integer                       :: throwfalse      ! otherwise here
      integer(kind=int64)           :: examined        ! how many items has the monkey examined
   end type monkey_t

   type(monkey_t), dimension(0:LASTMONKEY) :: monkey

   rec = ""
   call open_aoc_input(AOCIN)

   ! initialize
   part_one = 0; part_two = 0

   call init
   call load
   call goaround(MINROUNDS, CRELIEF)
   part_one = multiplytoptwo()

   call init
   call load
   call goaround(MAXROUNDS, calculatedrelief())
   part_two = multiplytoptwo()

   ! report and close
   print *
   print *, part_one, "part one"
   print *, part_two, "part two"
   print *

   call close_aoc_input(AOCIN)

contains

   subroutine init
      implicit none
      integer :: i
      do i = 0, LASTMONKEY
         monkey(i)%id = " "
         monkey(i)%numitems = 0
         monkey(i)%priority = 0
         monkey(i)%operation = ' '
         monkey(i)%opervalue = 0
         monkey(i)%testdivisor = 0
         monkey(i)%throwtrue = 0
         monkey(i)%throwfalse = 0
         monkey(i)%examined = 0
      end do
   end subroutine init

   ! read a record, but it must match the epexted marker or we'll error out
   subroutine read_expecting(marker, eofok)
      implicit none
      character(len=*), intent(in) :: marker
      logical, intent(in)          :: eofok
      logical                      :: readok
      readok = read_aoc_input(AOCIN, rec)
      if ((readok) .and. (rec(1:len(marker)) == marker)) return
      rec = "$$$EOF$$$"
      if (eofok) return
      print *, "*** error *** unexpected eof on input"
      stop 1
   end subroutine

   subroutine load
      implicit none
      integer :: i, j
      integer, allocatable :: p(:)
      call rewind_aoc_input(AOCIN)
      i = 0
      do
         call read_expecting(cmonkey, .true.)
         if (trim(rec) == "$$$EOF$$$") exit

         monkey(i)%id = rec(lcmonkey + 1:lcmonkey + 1)
         call read_expecting(cstart, .false.)
         p = integers_from_string(rec, lcstart + 1, ",")
         do j = 1, size(p)
            monkey(i)%priority(j) = p(j)
         end do
         monkey(i)%numitems = size(p)
         call read_expecting(coper, .false.)
         monkey(i)%operation = rec(lcoper + 1:lcoper + 1)
         monkey(i)%opervalue = 0 ! 0 means 'old value'
         if (rec(lcoper + 3:lcoper + 6) /= "old") then
            read (rec(lcoper + 3:len(rec)), *) monkey(i)%opervalue
         end if
         call read_expecting(ctest, .false.)
         read (rec(lctest + 1:len(rec)), *) monkey(i)%testdivisor
         call read_expecting(cift, .false.)
         read (rec(lcift + 1:len(rec)), *) monkey(i)%throwtrue
         call read_expecting(ciff, .false.)
         read (rec(lciff + 1:len(rec)), *) monkey(i)%throwfalse
         call read_expecting("", .true.)

         i = i + 1
         ! print *, i, monkey(i)%numitems, monkey(i)%operation, monkey(i)%opervalue, monkey(i)%testdivisor, &
         !    monkey(i)%throwtrue, monkey(i)%throwfalse
      end do
   end subroutine load

   function integers_from_string(str, beg, sep) result(a)
      ! a beautiful find from a SO question. author is Vladimir Fuka, LadaF at github.
      ! needs error checking for general use, and i got strict on intent so the input
      ! string needs a local copy, but it's still a cool idea.
      implicit none
      integer, allocatable       :: a(:)
      character(*), intent(in)   :: str    ! string to pluck integers from
      integer, intent(in)        :: beg    ! start pos in string, 1 based
      character(*), intent(in)   :: sep    ! separator character, only first pos used
      character(len(str))        :: cpy
      integer                    :: i, n
      n = 0
      cpy = str(beg:len(str))
      do i = 1, len_trim(cpy)
         if (cpy(i:i) == sep) then
            n = n + 1
            cpy(i:i) = "," ! read wants a comma, your sep can be anything
         end if
      end do
      allocate (a(n + 1))
      read (cpy, *) a
   end function integers_from_string

   ! the monkey wonders if you're worried? are you worried? the operation is
   ! applied and then the new priority is divided by the relief value and rounded
   ! down.

   subroutine dooperation(m, i, relief)
      implicit none
      integer, intent(in) :: m       ! the monkey
      integer, intent(in) :: i       ! item number
      integer(kind=int64), intent(in) :: relief  ! how you feeling?

      integer(kind=int64) :: op, np

      op = monkey(m)%priority(i)
      if (monkey(m)%opervalue == 0) then
         np = op
      else
         np = monkey(m)%opervalue
      end if
      if (monkey(m)%operation == "+") then
         monkey(m)%priority(i) = op + np
      else if (monkey(m)%operation == "*") then
         monkey(m)%priority(i) = op*np
      else
         print *, "*** error *** illegal operation '", monkey(m)%operation, "' for monkey", m
         stop 1
      end if
      ! take the mod instead of doing division when doing the prime multiples version
      if (relief == CRELIEF) then
         monkey(m)%priority(i) = monkey(m)%priority(i)/relief
      else
         monkey(m)%priority(i) = mod(monkey(m)%priority(i), relief)
      end if
   end subroutine

   ! mine! mine! add the incoming priority to the end of this monkey's list.
   subroutine catchitem(m, n)
      implicit none
      integer, intent(in)             :: m   ! which monkey
      integer(kind=int64), intent(in) :: n   ! item priority value
      monkey(m)%numitems = monkey(m)%numitems + 1
      monkey(m)%priority(monkey(m)%numitems) = n
   end subroutine

   ! the monkey makes up his mind.
   function dotest(m, i)
      implicit none
      integer, intent(in) :: m   ! which monkey
      integer, intent(in) :: i   ! which item
      logical             :: dotest
      dotest = mod(monkey(m)%priority(i), monkey(m)%testdivisor) == 0
   end function

   ! calculate a relief value. i'm not much on number theory, but everything was
   ! a prime number so after thinking about it for a while i still had nothing
   ! and checked reddit. this is similar to the ghost walk problem in that we're
   ! dealing with prime multiples.
   function calculatedrelief() result(res)
      implicit none
      integer(kind=int64) :: res
      integer             :: i
      res = 1
      do i = 0, LASTMONKEY
         res = res*monkey(i)%testdivisor
      end do
   end function calculatedrelief

   ! round and round we go, and you're the monkey in the middle
   subroutine goaround(rounds, relief)
      implicit none
      integer, intent(in) :: rounds
      integer(kind=int64), intent(in) :: relief
      integer :: i, j, k
      do i = 1, rounds
         do j = 0, LASTMONKEY
            monkey(j)%examined = monkey(j)%examined + monkey(j)%numitems
            do k = 1, monkey(j)%numitems
               call dooperation(j, k, relief)
            end do
            do k = 1, monkey(j)%numitems
               if (dotest(j, k)) then
                  call catchitem(monkey(j)%throwtrue, monkey(j)%priority(k))
               else
                  call catchitem(monkey(j)%throwfalse, monkey(j)%priority(k))
               end if
            end do
            monkey(j)%numitems = 0
         end do
      end do
   end subroutine goaround

   ! find the number of items examined by the monkeys who touched your stuff
   ! most frequently. bad monkeys. report the values of the top two multiplied
   ! together. the things we do to avoid sorting or creating a priority queue.
   function multiplytoptwo() result(res)
      implicit none
      integer(kind=int64) :: res
      integer :: i, j, k

      j = 0 ! subscript of biggest found
      k = 1 ! subscript of second biggest found
      ! we've arbitrarily picked the first two, now make sure they're in order
      if (monkey(j)%examined < monkey(k)%examined) then
         i = j; j = k; k = i    ! ordered correctly
      end if
      ! now find the real top two
      do i = 2, LASTMONKEY
         if (monkey(i)%examined <= monkey(k)%examined) cycle ! too small
         if (monkey(i)%examined <= monkey(j)%examined) then ! between j and k
            k = i
         else                                               ! new top
            k = j
            j = i
         end if
      end do
      res = monkey(j)%examined*monkey(k)%examined
   end function multiplytoptwo

end program solution
