! solution.f90 -- 2022 day 04 -- scratch cards -- troy brumley blametroi@gmail.com

! advent of code 2023 day 4 -- scratch cards
!
! given a listing of scratch off cards and possible winners, score the cards and
! report the total. you later realize that the score actually just awards more
! cards to play. report the total number of scratch cards won under the rules. }

program solution

   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input
   use intset, only: size_of_set, clear_set, include_in_set, exclude_from_set, intersection_of_sets, &
                     union_of_sets, intset_limit, contains_set, is_in_set

   implicit none

   integer, parameter            :: AOCIN = 10
   integer(kind=int64)           :: part_one, part_two  ! results
   character(len=max_aoc_reclen) :: rec

   ! test data has few winning numbers and draws, 5 and 8 instead of 10 and 25
   ! allocate for full data and just pay attention to the actual data read.

   integer, parameter :: MAX_CARDS = 219    ! 10 in small set
   integer, parameter :: MAX_WNOS = 10      ! 5 in small set
   integer, parameter :: MAX_DNOS = 25      ! 8 in small set

   ! the input format is "Card #: ww ww ww ... | dd dd dd ..."
   character(len=1), parameter :: SPACE = " "
   character(len=1), parameter :: COLON = ":"         ! end of card number
   character(len=2), parameter :: BREAKER = " |"      ! separates winners from draws
   character(len=5), parameter :: CARDPFX = "Card "   ! input marker

   ! draw numbers range from 0 to 99

   type card_t
      integer :: cno
      integer :: wno(1:MAX_WNOS)  ! winning numbers
      integer :: dno(1:MAX_DNOS)  ! drawn numbers
      integer :: win              ! wins in draws
      integer :: pts              ! points
      integer(kind=int64) :: cpy  ! copies of card 1+
   end type card_t

   type(card_t) :: deck(1:MAX_CARDS)

   integer :: numcards, numwno, numdno ! our data geometry

   ! initialize
   part_one = 0; part_two = 0
   rec = ""
   call open_aoc_input(AOCIN)

   call init
   call load

   call score

   ! report and close

   print *
   print *, part_one, "part one"
   print *, part_two, "part two"
   print *

   call close_aoc_input(AOCIN)

contains

   ! zero/invalidate the deck
   subroutine init
      implicit none

      numcards = -1
      numwno = -1
      numdno = -1

   end subroutine init

   subroutine load
      implicit none
      integer :: i
      i = 0
      do while (read_aoc_input(AOCIN, rec))
         if (len_trim(rec) < 1) cycle
         i = i + 1
         deck(i) = parsecard(trim(rec)//" ")
      end do
      numcards = i
   end subroutine load

   function is_digit(c) result(res)
      implicit none
      character(len=1), intent(in) :: c
      logical                      :: res
      res = c <= "9" .and. c >= "0"
   end function is_digit

   function char_int(c) result(res)
      implicit none
      character(len=1), intent(in) :: c
      integer                      :: res
      res = -1
      if (is_digit(c)) res = ichar(c) - ichar("0")
   end function char_int

   ! parse the input card. the interesting data is in two arrays, the first for
   ! winning numbers and the second for numbers drawn. the extent of each array
   ! is different between the small test data and the run data. the extent will
   ! be discovered on the first card and then is expected to stay constant
   ! throughout a run.
   !
   ! the input record is expected to be trimmed such that it has only one
   ! trailing blank.
   !
   ! given the nature of the data and problem, there aren't many safety checks
   ! here. if running past the end of input causes an access violation, that's
   ! ok.

   function parsecard(s) result(res)
      implicit none
      character(len=*), intent(in) :: s
      type(card_t)                 :: res
      integer                      :: i        ! work/subscripts
      integer                      :: n        ! work/convert string to int
      integer                      :: p        ! position in rec

      if (.not. s(1:len(CARDPFX)) == CARDPFX) then
         print *, "*** error in input, missing marker '", CARDPFX, "'"
         stop 1
      end if

      res % cno = 0 ! fill the shell
      res % wno = 0
      res % dno = 0
      res % win = 0
      res % pts = 0
      res % cpy = 1 ! not zero

      ! set up to start parse at first digit of cno
      p = len(CARDPFX) + 1

      n = 0
      do while (s(p:p) /= COLON)
         if (is_digit(s(p:p))) n = n * 10 + char_int(s(p:p))
         p = p + 1
      end do
      res % cno = n

      ! skip past colon to " ##"
      p = p + 1

      ! everything up to " |", the break marker between wins and draws,
      ! gets put in wno().

      i = 0
      do while (s(p:p + len(BREAKER) - 1) /= BREAKER)
         p = p + 1
         if (s(p:p) == SPACE) cycle
         i = i + 1
         n = 0
         do while (is_digit(s(p:p)))
            n = n * 10 + char_int(s(p:p))
            p = p + 1
         end do
         res % wno(i) = n
      end do

      ! remember extent of wno if this is first time through

      if (numwno < 0) then
         numwno = i
      else if (numwno /= i) then
         print *, "*** error *** differing wno counts, expected", numwno, "but got", i
         stop 1
      end if

      ! advance to " ##"
      p = p + len(BREAKER)

      ! and everything up to the end of the record gets put into dno.

      i = 0
      do while (p < len(s))
         p = p + 1
         if (s(p:p) == SPACE) cycle
         i = i + 1
         n = 0
         do while (is_digit(s(p:p)))
            n = n * 10 + char_int(s(p:p))
            p = p + 1
         end do
         res % dno(i) = n
      end do

      ! remember extent of dno if this is first time through

      if (numdno < 0) then
         numdno = i
      else if (numdno /= i) then
         print *, "*** error *** differing dno counts, expected", numdno, "but got", i
         stop 1
      end if
   end function parsecard

   subroutine scorecardone(c)
      implicit none
      type(card_t), intent(inout) :: c
      integer                     :: i
      logical                     :: s(0:intset_limit)

      ! a set is quicker than repeated sequential scans, even though in my
      ! implementation, it is a repeated sequential scan ... at least it
      ! reads a little better.

      call clear_set(s)
      do i = 1, numwno
         call include_in_set(s, c % wno(i))
      end do

      c % win = 0
      c % pts = 0
      do i = 1, numdno
         if (is_in_set(c % dno(i), s)) then
            c % win = c % win + 1
         end if
      end do

      if (c % win > 0) then
         c % pts = shiftl(1, c % win - 1)
      end if
      call flush

   end subroutine scorecardone

   ! score all the cards in the deck for part two

   subroutine scorecardtwo
      implicit none
      integer(kind=int64) :: i, j, k
      do i = 1, numcards
         do j = 1, deck(i) % cpy
            do k = i + 1, min(numcards, i + deck(i) % win)
               deck(k) % cpy = deck(k) % cpy + 1
            end do
         end do
      end do
   end subroutine scorecardtwo

   subroutine score
      implicit none
      integer :: i

      do i = 1, numcards
         call scorecardone(deck(i))
      end do

      call scorecardtwo

      do i = 1, numcards
         part_one = part_one + deck(i) % pts
         part_two = part_two + deck(i) % cpy
      end do

   end subroutine score

end program solution
