! solution.f90 -- 2022 day 07 -- camel cards -- troy brumley blametroi@gmail.com

! advent of code 2023 day 7, camel cards
!
! scoring a poker like card game played without suits. the parts were split into
! two files to simplify some code in the pascal implementation. i am attempting
! to pull the code back into one file for the fortran implementation.
!
! enumerated types are different, so coming up with an approach may take me into
! some new territory, which is the goal of these exercises.
!
! it's not clear which way enumerations are going to go in fortran but it is
! easy enough to used named contants as with c. some type safety and pascal's
! ord/pred/succ are lost, but i almost never rely on them.
!
! the rules are those of simplified five card poker but no suits. in part one
! there are no wild cards. in part two, jokers are wild for purposes of hand
! classification but are considered the lowest card when breaking ties. ties
! are broken card by card in order delt.

program solution

   use iso_fortran_env, only: int64, int8
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input
   use aochelpers, only: is_char_in, str_int

   implicit none

   integer, parameter            :: AOCIN = 10
   integer(kind=int64)           :: part_one, part_two  ! results
   character(len=max_aoc_reclen) :: rec

   integer, parameter            :: MAXHANDS = 1000 ! 5 in test
   integer, parameter            :: MAXCARDS = 5    ! in hand
   character(len=*), parameter   :: RANKS_SIMPLE = "23456789TJQKA" ! card faces
   character(len=*), parameter   :: RANKS_WILD = "J23456789TQKA" ! card faces
   character(len=*), parameter   :: JOKER = "J"

   character(len=len(RANKS_SIMPLE))  :: CARDRANK                  ! changes based on rule set one or two

   ! i'm looking around for enumeration support in Fortran and it seems all they
   ! have is a support for c interop. until i see more of fortran 2023, i'll
   ! just go with named constants. we lose the type checking support that pascal
   ! has, but i'm comfortable without it.

   ! card hand kinds, a classification from lowest to highest ranking
   integer(kind=int8), parameter :: hk_unknown = 0, &
                                    hk_single = 1, &
                                    hk_pair = 2, &
                                    hk_two_pair = 3, &
                                    hk_three = 4, &
                                    hk_full_house = 5, &
                                    hk_four = 6, &
                                    hk_five = 7

   ! the hand as read and interpreted, merging part two into part one.
   type hand_t
      character(len=MAXCARDS) :: cards         ! as read
      integer                 :: bid           ! and bid
      character(len=MAXCARDS) :: sorted        ! sorted in current rank
      integer                 :: unique        ! unique ranks in the hand
      integer(kind=int8)      :: hk            ! hand kind (hk_unknown .. hk_five)
      integer                 :: jokers        ! if under part two, how many jokers?
      character(len=MAXCARDS) :: joked         ! hand after changing jokers out
   end type hand_t

   ! the interesting data
   type(hand_t), dimension(1:MAXHANDS) :: hands
   integer                             :: numhands

   ! initialize
   part_one = 0; part_two = 0
   rec = ""
   call open_aoc_input(AOCIN)

   ! part one uses the ranks sans jokers
   part_one = dopartone()

   ! part two uses the ranks with jokers
   part_two = doparttwo()

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
      do i = 1, MAXHANDS
         associate (h => hands(i))
            h % cards = ""
            h % bid = 0
            h % sorted = ""
            h % unique = 0
            h % jokers = 0
            h % joked = ""
            h % hk = hk_unknown
         end associate
      end do
      numhands = 0
   end subroutine init

   ! read the hands and do basic parsing. while space is allocated for part two
   ! this load is presently geared toward part one rules.

   subroutine loadone
      implicit none
      integer :: i, p
      i = 0
      call rewind_aoc_input(AOCIN)
      do while (read_aoc_input(AOCIN, rec))
         i = i + 1
         associate (h => hands(i))
            ! the basics
            CARDRANK = RANKS_SIMPLE
            h % cards = rec(1:MAXCARDS)
            p = MAXCARDS + 2 ! first digit of bid
            h % bid = str_int(rec, p)
            h % sorted = arrangecards(h % cards)
            ! part one
            h % unique = countranks(h % sorted)
            h % hk = whatkind(h % unique, h % sorted)
            h % sorted = resortcards(h % hk, h % sorted)
         end associate
      end do
      numhands = i
   end subroutine loadone

   ! this is pretty much a mirror of the load for one, but wildcards
   ! change a few things around. structure parallel, but some of the
   ! later details morph a bit.

   subroutine loadtwo
      implicit none
      integer :: i, p
      i = 0
      call rewind_aoc_input(AOCIN)
      do while (read_aoc_input(AOCIN, rec))
         i = i + 1
         associate (h => hands(i))
            ! the basics
            h % cards = rec(1:MAXCARDS)
            p = MAXCARDS + 2 ! first digit of bid
            h % bid = str_int(rec, p)
            h % sorted = arrangecards(h % cards)
            h % jokers = countjokers(h % cards)
            h % unique = countranks(h % sorted)
            if (h % jokers == 0) then
               h % joked = h % sorted
            else if (h % jokers == MAXCARDS) then
               h % joked = repeat("A", MAXCARDS)
            else
               h % hk = whatkind(h % unique - 1, trim(h % sorted(1:MAXCARDS - h % jokers)))
               h % joked = jokercards(trim(h % sorted(1:MAXCARDS - h % jokers)), h % hk)
            end if
            h % unique = countranks(h % joked)
            h % hk = whatkind(h % unique, h % joked)
         end associate
      end do
      numhands = i
   end subroutine loadtwo

   ! the sum of rank (subscript, but watch for ties) and bid.

   function dopartone() result(res)
      implicit none
      integer(kind=int64) :: res
      integer :: i
      CARDRANK = RANKS_SIMPLE
      call init
      call loadone
      res = 0
      call sorthands
      do i = 1, numhands
         res = res + hands(i) % bid * i
      end do
   end function dopartone

   function doparttwo() result(res)
      implicit none
      integer(kind=int64) :: res
      integer :: i
      CARDRANK = RANKS_WILD
      call init
      call loadtwo
      res = 0
      call sorthands
      do i = 1, numhands
         res = res + hands(i) % bid * i
      end do
   end function doparttwo

   ! an alternative to sortcards
   function arrangecards(s) result(res)
      implicit none
      character(len=*), intent(in) :: s ! card hand
      character(len=len(s))        :: res
      integer                      :: i, j
      res = ""
      do i = len(CARDRANK), 1, -1
         do j = 1, len_trim(s)
            if (s(j:j) == CARDRANK(i:i)) res = trim(res)//s(j:j)
         end do
      end do
   end function arrangecards

   ! how many unique ranks are in the hand. doing this quickly identifies a
   ! couple of hand types and provides a filter to simplify checking for the
   ! rest.

   function countranks(s) result(res)
      implicit none
      character(len=*), intent(in)        :: s       ! the hand
      integer                             :: res     ! rank count
      integer                             :: i
      logical, dimension(1:len(CARDRANK)) :: seen    ! a quick and dirty set
      seen = .false.
      do i = 1, len_trim(s)
         seen(index(CARDRANK, s(i:i))) = .true.
      end do
      res = 0
      do i = 1, len(CARDRANK)
         if (seen(i)) res = res + 1
      end do
   end function countranks

   ! now that we know the kind of hand, resort the cards so that and single
   ! cards come at the end of the hand.
   !
   ! trying out pass by value so i can use an argument as a work variable. it
   ! seems fortran does this in backwards from pascal. here we pass by reference
   ! (which is faster) and use the intent specifier to control available
   ! operations. but it turns out that this requires setting up an explicit
   ! interface. so, i'll just create locals.

   function resortcards(hk, hs) result(res)
      implicit none
      integer(kind=int8), intent(in)       :: hk   ! hand kind
      character(len=*), intent(in)         :: hs   ! the cards in the hand, sorted
      character(len=len(hs))               :: res  ! reordered
      character(len=len(hs))               :: s    ! work
      integer                              :: i, j
      character(len=1)                     :: c

      res = ""
      s = hs

      select case (hk)

      case (hk_unknown)
         ! should never happen so

         res = "ERROR"

      case (hk_single, hk_five)
         ! the basic ranked sort is all that is needed

         res = s

      case (hk_pair)
         ! move the paired cards to the front while preserving the order of the
         ! other cards.

         i = 1
         j = 0

         ! find the start of the run
         do while (j == 0 .and. i < len(s))
            if (s(i:i) == s(i + 1:i + 1)) j = i
            i = i + 1
         end do
         if (j > 1) then
            c = s(1:1)
            s(1:1) = s(j:j)
            s(j:j) = c
            c = s(2:2)
            s(2:2) = s(j + 1:j + 1)
            s(j + 1:j + 1) = c
         end if
         res = s

      case (hk_three)
         ! for three of a kind, move the triple to the front of the hand. if the
         ! run doesn't start at position one, it must start at two or three. if
         ! it starts at two (xyyyz) swap one with four. if it starts at three
         ! (xzyyy) also swap two and five. if somehow we've gotten garbage, do
         ! nothing.

         i = 1
         j = 0
         do while (j == 0 .and. i < len(s))
            if (s(i:i) == s(i + 1:i + 1)) j = i ! found start
            i = i + 1
         end do
         select case (j)
         case (2)
            c = s(1:1)
            s(1:1) = s(4:4)
            s(4:4) = c
         case (3)
            c = s(1:1)
            s(1:1) = s(4:4)
            s(4:4) = c
            c = s(2:2)
            s(2:2) = s(5:5)
            s(5:5) = c
         end select
         res = s

      case (hk_two_pair)
         ! for two pair, move the pairs to the front. since the cards are
         ! already sorted by rank, the od card an only be in positions one,
         ! three, or five. we want it in five. the effect of these two if blocks
         ! is to bubble the odd card from one to three to five as needed.

         if (s(1:1) /= s(2:2)) then
            c = s(1:1)
            s(1:1) = s(3:3)
            s(3:3) = c
         end if

         if (s(3:3) /= s(4:4)) then
            c = s(3:3)
            s(3:3) = s(5:5)
            s(5:5) = c
         end if
         res = s

      case (hk_full_house)
         ! for a full house, we want the triple in front and the pair last. so,
         ! out of order would be ( yyxxx ). if position one doesn't match three,
         ! swap one and two with four and five.

         if (s(1:1) /= s(3:3)) then
            c = s(1:1)
            s(1:1) = s(4:4)
            s(4:4) = c
            c = s(2:2)
            s(2:2) = s(5:5)
            s(5:5) = c
         end if
         res = s

      case (hk_four)
         ! for four of a kind, the singleton comes last. if one matches two,
         ! we're already correct.

         if (s(1:1) /= s(2:2)) then
            c = s(1:1)
            s(1:1) = s(5:5)
            s(5:5) = c
         end if
         res = s

      case default
         ! an even bigger should not occur then hk_unknown

         res = "WTFIT"

      end select
   end function resortcards

   ! compare hands and return their ordering via the neg-0-pos convention. this
   ! peeks into the hands array. the hk_* enums should be in ascending order. if
   ! two hands are equal, then the strength of the indivudual cards as
   ! originally delt is used to break the tie.

   function comparehands(i, j) result(res)
      implicit none
      integer, intent(in) :: i, j
      integer             :: res
      integer             :: k
      res = hands(i) % hk - hands(j) % hk
      k = 0
      do while (res == 0 .and. k < len(hands(i) % cards))
         k = k + 1
         res = index(CARDRANK, hands(i) % cards(k:k)) - index(CARDRANK, hands(j) % cards(k:k))
      end do
   end function comparehands

   ! an exchange sort of the collection of card hands

   subroutine sorthands
      implicit none
      integer      :: i, j      ! indices
      logical      :: s         ! did we do an exchange
      type(hand_t) :: third     ! the third or gripping hand for the swap

      s = .true.
      do while (s)
         s = .false.
         do i = 1, numhands - 1
            j = comparehands(i, i + 1)
            if (j > 0) then
               s = .true.
               third = hands(i)
               hands(i) = hands(i + 1)
               hands(i + 1) = third
            end if
         end do
      end do
   end subroutine sorthands

   ! determine best kind of hand that can be made by wise promotion of the
   ! jokers. this is actually straightforward, unique card count combined with
   ! the number of cards yields only a few possibilities.
   !
   ! n     meaning              maps to
   ! 1     xjjjj                xxxxx
   ! 2     xyjjj, xxyjj, xxxyj  xxxxy
   ! 3     xyzjj, xxyzj         xxyyz where x and y are highest ranks
   ! 4     xyzuj                xxyzu
   ! 5     xyzuv                xyzuv shouldn't be called for 5, but that's a fold hand

   function whatkindshort(n) result(res)
      implicit none
      integer, intent(in) :: n ! number of unique card ranks
      integer(kind=int8) :: res

      select case (n)
      case (1)
         res = hk_five
      case (2)
         res = hk_four
      case (3)
         res = hk_three
      case (4)
         res = hk_pair
      case (5)
         res = hk_single
      case default
         res = hk_unknown
      end select
   end function whatkindshort

   ! determine the hands kind or rank, full house, three of a kind, etc. this
   ! evaluates the full hand without considering jokers. the number of unique
   ! cards found in the hand provide a quick first pass to narrow choices down.
   ! from there count each card rank to finish figuring out the hand.
   !
   ! unique ccounts of 1, 4, and 5 can each be of only one hand classification
   ! (five of a kind, full house, and trash hand). two distinct ranks are either
   ! a full house or four of a kind, while 3 distinct ranks could be either two
   ! pair or three of a kind. }

   function whatkindfull(n, s) result(res)
      implicit none
      integer, intent(in) :: n ! unique card count
      character(len=*), intent(in) :: s ! the card hand
      integer(kind=int8) :: res ! tk_unknonw ...
      integer :: j, k, l
      character(len=1) :: c

      select case (n)

      case (1)
         res = hk_five

      case (2)
         j = 0
         res = hk_unknown
         ! go through the hand and identify the first pair, triple, or quad
         ! if we find a pair or triple first, this must be a full house
         ! if we find a quad, four of a kind
         do while (res == hk_unknown)
            j = j + 1
            k = 0
            c = s(j:j)
            do l = 1, len(s)
               if (c == s(l:l)) k = k + 1
            end do
            if (k == 2 .or. k == 3) res = hk_full_house
            if (k == 4) res = hk_four
         end do

      case (3)
         j = 0
         res = hk_unknown
         ! this has the same basic approach as the case for 2 unique
         ! ranks, but seach for a triple or pair, and whatever is
         ! found first determines the hand kind
         do while (res == hk_unknown)
            j = j + 1
            k = 0
            c = s(j:j)
            do l = 1, len(s)
               if (c == s(l:l)) k = k + 1
            end do
            if (k == 3) res = hk_three
            if (k == 2) res = hk_two_pair
         end do

      case (4)
         res = hk_pair

      case (5)
         res = hk_single

      case default
         res = hk_unknown

      end select
   end function whatkindfull

   ! determine the kind of hand (pair, full house, etc). the cards should
   ! be in order by rank. as with countranks, a full 5 card hand is not
   ! required for this check. this allows us to find the best hand before
   ! applying any jokers.
   function whatkind(n, s) result(res)
      implicit none
      integer, intent(in)          :: n          ! count of unique cards
      character(len=*), intent(in) :: s          ! the cards sorted by ranks
      integer(kind=int8)           :: res        ! kind enumeration

      if (len(trim(s)) == MAXCARDS) then
         res = whatkindfull(n, s)
      else
         res = whatkindshort(n)
      end if
   end function whatkind

   ! how many jokers are in the hand?

   function countjokers(s) result(res)
      implicit none
      character(len=*), intent(in) :: s ! a hand
      integer                      :: res
      integer                      :: i
      res = 0
      do i = 1, len(s)
         if (s(i:i) == JOKER) res = res + 1
      end do
   end function countjokers

   ! build the new hand by adding jokers to maximize the hand value.

   function jokercards(si, hk) result(res)
      implicit none
      character(len=*), intent(in)   :: si   ! the hand
      integer(kind=int8), intent(in) :: hk   ! hand kind
      character(len=MAXCARDS)        :: s    ! hand after arrangement and work
      character(len=MAXCARDS)        :: res  ! the new hand
      integer                        :: i, j

      ! the cards should already be arranged, but since we need to
      ! make our own copy, let's be sure.
      s = arrangecards(trim(si))

      select case (hk)

      case (hk_five)
         ! four jokers, xjjjj -> xxxxx
         s = repeat(s(1:1), MAXCARDS)

      case (hk_four)
         ! three of a kind becomes four, just identify the card

         ! to duplicate
         do while (len_trim(s) < MAXCARDS)
            if (s(1:1) == s(2:2)) then
               s = s(1:1)//s             ! xxxyJ -> xxxxy
            else
               s = trim(s)//s(2:2)       ! xyyyJ -> xyyyy
            end if
         end do

      case (hk_full_house)
         ! we have two pair and a joker, pick the high pair to triple
         if (index(CARDRANK, s(1:1)) < index(CARDRANK, s(3:3))) then
            s = trim(s)//s(3:3)          ! xxyyj -> xxyyy
         else
            s = s(1:1)//trim(s)          ! xxyyj -> xxxyy
         end if

      case (hk_three)
         ! we have a pair and a couple of singles to start, make the pair a triple
         if (s(1:1) == s(2:2)) then
            s = s(1:1)//trim(s)          ! xxyzj -> xxxyz
         else if (s(2:2) == s(3:3)) then
            s = s(2:2)//trim(s)          ! xyyzj -> yxyyz -> xyyyz
         else
            s = s(3:3)//trim(s)          ! xyzzj -> xyzzz
         end if

      case (hk_two_pair)
         ! make extra copies of the two highest cards
         ! this is a transliteration of the pascal ord/chr
         ! and could stand a redo using more of the fortran
         ! functions.
         !
         ! to be flagged as two pair, we start with three
         ! singletons and duplicate the highest two. as
         ! digits come before the alphabet in ascii, this
         ! works.
         j = 0
         do i = 1, len(trim(s))
            j = max(j, ichar(s(i:i)))
         end do
         s = achar(j)//trim(s)          ! xyzjj -> Mxyzj
         do i = 2, len(trim(s))
            j = max(j, ichar(s(i:i)))
         end do
         s = achar(j)//trim(s)          ! Mxyzj -> MMxyz

      case (hk_pair)
         ! four singletons and a joker become one pair,
         ! as with hk_two_pair, pick the highest of the
         ! singletons as the card to duplicate
         j = 0
         do i = 1, len(s)
            j = max(j, ichar(s(i:i)))
         end do
         s = achar(j)//trim(s)

      case default

      end select

      res = s

   end function jokercards

end program solution
