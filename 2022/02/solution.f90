! solution.f90 -- 2022 day 02 -- rock paper scissors -- Troy Brumley blametroi@gmail.com

! given a 'strategy guide' for rock paper scissors, find the score if you follow
! the strategy. for part one, you interpret the guide as 'opponent play' and
! 'you play'. a, b, c by the opponent for rock, paper, scissors, and x, y, z for
! you in the same order. for part two, you learn that your play is instead 'you
! get result'. x, y, and z for lose, y for draw, and z for win.
!
! for each part, report the sum of your scores for each round.
!
! the score calcuation is the score for the shape you selected (1 for rock, 2
! for paper, and 3 for scissors) plus the score for the outcome of the round (0
! if you lost, 3 if the round was a draw, and 6 if you won).

program solution

   implicit none

   ! file io
   integer, parameter  :: funitin = 10
   character(len=255)  :: fname = "./dataset.txt"
   integer             :: fstat
   character(len=255)  :: frecin

   ! application
   character(len=1)    :: opp, you, myou          ! input for opponent, you, and later morphed you
   integer             :: sopp, syou, smyou       ! a-c and x-z as subscripts
   integer(kind=8)     :: i                       ! count
   integer(kind=8)     :: partone, parttwo        !
   integer             :: scores(3, 3)             ! map scores ax->cz
   character(len=1)    :: morphplays(3, 3)         ! part two strategy changer

   ! in a real app we'd check for missing arguments
   call get_command_argument(1, fname)
   open (unit=funitin, file=trim(fname), access="stream", form="formatted", status="old", iostat=fstat)
   if (fstat /= 0) then ! ERROR
      print *, "error opening dataset = ", fstat
      stop
   end if

   ! initialization

   i = 0; partone = 0; parttwo = 0
   call inittables(scores, morphplays)

   ! input is a series of instructions for a round of rock-paper-scissors.
   ! opponent play is one character, a blank, and then the player play. there
   ! are two different interpretations for the player play but the processing
   ! is straight forward and easily handled in one pass through the data.
   !
   ! lookup table 'scores', translates a-c and x-z to an appropriate subscript
   ! to get actual score. the calculation is based on a weight for the item
   ! you seelcted (rock=1, paper=2, scissors=3) and the outcome (lose=0, draw=3,
   ! and win=6).
   !
   ! for part two, x-y indicates an outcome and not a play. the lookup table
   ! 'morphplays' maps the opponent play and desired outcome back to the actual
   ! play you would make. then the part one score is applied.

   do

      i = i + 1

      read (funitin, "(a)", iostat=fstat) frecin
      if (fstat < 0) exit ! end of file
      if (fstat > 0) then ! some other error
         print *, "error reading dataset = ", fstat
         stop
      end if

      opp = frecin(1:1)
      you = frecin(3:3)

      ! part one uses your selection as the play
      sopp = ichar(opp) - ichar('A') + 1
      syou = ichar(you) - ichar('X') + 1
      partone = partone + scores(sopp, syou)

      ! part two uses your selection as outcome, convert to the
      ! right play to get the outcome and score as in part one.
      myou = morphplays(sopp, syou)
      smyou = ichar(myou) - ichar('X') + 1
      parttwo = parttwo + scores(sopp, smyou)

   end do

   ! report and close
   print *
   print *, i, " records read "
   print *, partone, " part one "
   print *, parttwo, " part two "
   print *

   close (funitin)

contains

   subroutine inittables(s, m)
      implicit none
      integer, intent(inout) :: s(3, 3)
      character*1, intent(inout) :: m(3, 3)

      ! scores for opponent play (rows) versus your play (columns)
      s(1, 1) = 4; s(1, 2) = 8; s(1, 3) = 3
      s(2, 1) = 1; s(2, 2) = 5; s(2, 3) = 9
      s(3, 1) = 7; s(3, 2) = 2; s(3, 3) = 6

      ! treating your play as a desired outcome, translate it back
      ! to the right play to get that outcome versus the opponent
      ! play.
      m(1, 1) = 'Z'; m(1, 2) = 'X'; m(1, 3) = 'Y'
      m(2, 1) = 'X'; m(2, 2) = 'Y'; m(2, 3) = 'Z'
      m(3, 1) = 'Y'; m(3, 2) = 'Z'; m(3, 3) = 'X'
   end subroutine inittables
end program solution
