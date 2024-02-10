! solution.f90 -- 2023 day 02 -- cube conundrum -- troy brumley blametroi@gmail.com

! advent of code 2023 day 02 -- cube conundrum
!
!  drawing colored cubes from a bag, how many draws can be made?
!
! input format:
!
! game #: sample (;sample)*
!
!   sample is # color (, # color)*
!   color is red | blue | green
!   # is an integer
!
! The Elf would first like to know which games would have been possible if the bag
! contained only 12 red cubes, 13 green cubes, and 14 blue cubes?

program solution

   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input

   implicit none

   integer, parameter            :: AOCIN = 10
   integer(kind=int64)           :: part_one, part_two  ! results
   character(len=max_aoc_reclen) :: rec

   ! domain data and constants

   integer, parameter  :: MAX_SAMPLES = 10
   integer, parameter  :: MAX_GAMES = 100

   character(len=5), parameter :: color_names(3) = ["red  ", "green", "blue "]

   type sample_t
      integer          :: red
      integer          :: green
      integer          :: blue
   end type sample_t

   type game_t
      integer         :: num
      integer         :: count
      type(sample_t)  :: sample(1:MAX_SAMPLES)
   end type game_t

   type(game_t)       :: game(1:MAX_GAMES)
   integer            :: numgames

   ! initialize
   rec = ""
   call open_aoc_input(AOCIN)
   part_one = 0; part_two = 0
   numgames = 0

   ! load
   do while (read_aoc_input(AOCIN, rec))
      if (len_trim(rec) < 1) cycle
      numgames = numgames + 1
      game(numgames) = parse()
   end do

   ! and find our answers
   part_one = dopartone()
   part_two = doparttwo()

   ! report and close
   print *
   print *, part_one, "part one"
   print *, part_two, "part two"
   print *

   call close_aoc_input(AOCIN)

contains

   ! simple predicates and converters

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

   ! parse an input record into a game structure. assumes input is "rec" and
   ! returns a populated game_t.
   !
   ! game #: sample (;sample)*
   !
   !   sample is # color (, # color)*
   !   color is red | blue | green
   !   # is an integer

   function parse() result(res)
      implicit none
      character(len=:), allocatable :: s
      type(game_t)                  :: res
      integer                       :: i, pos
      type(sample_t)                :: zero_sample

      ! an empty template
      zero_sample % red = 0
      zero_sample % green = 0
      zero_sample % blue = 0

      ! some work on the record to ease parsing. replace all the trailing
      ! blanks with "semicolon<blank>". you no longer need to trim when
      ! checking length.
      allocate (character(len=len_trim(rec) + 2) :: s)
      s = trim(rec)//"; "

      ! extract game number, reacord header is "game #: "

      res % num = 0
      pos = 6 ! "game "
      gameno: do while (pos < len(s) .and. is_digit(s(pos:pos)))
         res % num = res % num * 10 + char_int(s(pos:pos))
         pos = pos + 1
      end do gameno
      ! pos is now at ": "

      ! gather each sample, the parser is very trusting and will fall all to
      ! pieces if you feed it the wrong file.

      res % count = 0
      pos = pos + 2 ! first digit of first sample color

      samples: do while (pos < len(s))
         res % count = res % count + 1
         res % sample(res % count) = zero_sample

         colors: do

            ! find start of color count
            seekdigit: do while (.not. is_digit(s(pos:pos)))
               pos = pos + 1
            end do seekdigit
            ! pos is at first digit

            ! convert
            i = 0
            accumint: do while (is_digit(s(pos:pos)))
               i = i * 10 + char_int(s(pos:pos))
               pos = pos + 1
            end do accumint
            pos = pos + 1
            ! pos is now at the first letter of the color name

            ! as colors are uniquely identified by the first letter, we're not
            ! checking the whole string.

            if (s(pos:pos) == "r") then ! red
               res % sample(res % count) % red = i
               pos = pos + 3
            else if (s(pos:pos) == "g") then ! green
               res % sample(res % count) % green = i
               pos = pos + 5
            else if (s(pos:pos) == "b") then ! blue
               res % sample(res % count) % blue = i
               pos = pos + 4
            else ! what, you thought i'd just assume blue above?
               print *, "*** error *** expected color at position", pos, "not found '", trim(rec), "'"
               stop 1
            end if
            ! pos is now at first character after color name

            ! pos is now on either a comma, meaing another color is in this sample, or a
            ! semicolon, meaning that was the of this sample
            if (s(pos:pos + 1) == "; ") then
               pos = pos + 2
               cycle samples
            else if (s(pos:pos + 1) == ", ") then
               pos = pos + 2
               cycle colors
            end if

            print *, "*** error *** expected comma or semicolon at position", pos, "not found '", trim(rec), "'"
            stop 1
         end do colors
      end do samples
   end function parse

   ! for part one the game draws of input are checked to see if the would be
   ! possible from a bag loaded with only 12 red cubes, 13 green cubes, and 14
   ! blue cubes. the answer for part one is the sum of game numbers of the
   ! possible games.

   function dopartone() result(res)
      implicit none
      integer :: res

      type(sample_t) :: bag
      integer :: i, j

      ! available cubes as defined in the problem statement
      bag % red = 12
      bag % green = 13
      bag % blue = 14

      res = 0
      i = 0
      do i = 1, NUMGAMES
         !print *, "game", i
         ! assume this passes
         res = res + game(i) % num
         samples: do j = 1, game(i) % count
            !print *, "sample", j, game(i) % sample(j) % red, game(i) % sample(j) % green, game(i) % sample(j) % blue
            if ((bag % red < game(i) % sample(j) % red) .or. &
                (bag % green < game(i) % sample(j) % green) .or. &
                (bag % blue < game(i) % sample(j) % blue)) then
               !print *, "bzzt"
               ! the assumption has proved wrong
               res = res - game(i) % num
               exit samples
            end if
         end do samples
      end do
   end function dopartone

! for part two we must find the fewest number of cubes can satisfy a
! game's draws. the answer for part two is sum of the products of the cubes
! for each game.

   function doparttwo() result(res)
      implicit none
      integer :: res
      type(sample_t) :: bag
      integer :: power, i, j

      res = 0
      do i = 1, numgames

         bag % red = 0 ! initialize to empty
         bag % green = 0
         bag % blue = 0

         do j = 1, game(i) % count
            bag % red = max(bag % red, game(i) % sample(j) % red)
            bag % green = max(bag % green, game(i) % sample(j) % green)
            bag % blue = max(bag % blue, game(i) % sample(j) % blue)
         end do

         power = bag % red * bag % green * bag % blue
         res = res + power
      end do
   end function doparttwo

end program solution
