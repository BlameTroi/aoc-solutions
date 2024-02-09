! solution.f90 -- 2022 day 14 -- regolith reservoir  -- troy brumley blametroi@gmail.com

! advent of code 2022 day 14 -- regolith reservoir -- falling sand will trap you!
!
! sand is falling down from a hole in the wall.the wall is hollow but has rock
! structures(ledges and vertical runs) that slow, divert, and accumulate the
! sand.
!
! for part one, how much sand falls before no more can accumulate?
!
! the origin of the sand is at 500, 0.x increases to the right, and y increases
! toward the bottom.
!
! input traces the solid rock structures that will divert and accumulate sand,
! with straight lines from point to point.
!
! 400, 0 - > 410, 0 - > 410, 10 describes a blocky looking 7 10 units wide and
! 10 units tall.
!
! oh this is gonna be fun.
!
! the small test data of:
!
! 498, 4 - > 498, 6 - > 496, 6 503, 4 - > 502, 4 - > 502, 9 - > 494, 9
!
! describes a small backwards L nested above and to the left of a larger
! backwards L with a serif across its top.
!
! 24 units of sand can fall before the overflow begins.
!
! as with some other problems, this involves 'simultaneous' movement in a unit
! of time.
! ! edit ! no, it didn't. but it was a fun diversion.
!
! movement rules:
!
! * sand moves 1 unit down unless blocked.
! * if blocked attempt to move diagonally down and to the left.
! * if that attempt fails, attempt to move diagonally down and to the right.
! * if attempt fails, sand is at rest.
!
! once sand is past the lowest bound point (maximum y value) the overflow has begun.

program solution

   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input

   implicit none

   integer, parameter            :: AOCIN = 10
   integer(kind=int64)           :: part_one, part_two  ! results
   character(len=max_aoc_reclen) :: rec

   ! constants describing the wall grid or its contents. the glyphs for rock, air,
   ! and sand can be used both for checking and as part of a visualization display.
   integer, parameter            :: ORIGINX = 500   ! 473..527 observed
   integer, parameter            :: ORIGINY = 0     ! 13..165 observed
   integer, parameter            :: MINX = 0
   integer, parameter            :: MAXX = 1000
   integer, parameter            :: MINY = 0
   integer, parameter            :: MAXY = 200
   character(len=1), parameter   :: ROCK = "#"
   character(len=1), parameter   :: AIR = "."
   character(len=1), parameter   :: SAND = "o"

   ! problem state
   character(len=1)              :: grid(MINX:MAXX, MINY:MAXY)
   integer                       :: xrange(0:1), yrange(0:1)
   integer                       :: abyss    ! it is looking back at you!

   ! our domain types are points and grids
   type point_t
      integer :: x, y
   end type point_t

   ! initialize
   rec = ""
   call open_aoc_input(AOCIN)
   part_one = 0; part_two = 0
   grid = AIR
   xrange = 0
   yrange = 0
   abyss = 0

   call init
   call rewind_aoc_input(AOCIN)
   call load
   call displaygrid
   part_one = dopartone()

   call init
   call rewind_aoc_input(AOCIN)
   call load
   part_two = doparttwo()

   ! report and close

   print *
   print *, part_one, "part one"
   print *, part_two, "part two"
   print *

   call close_aoc_input(AOCIN)

contains

! part one asks how long until sand starts overflowing the ledges
!
! for part one we want to know how many grains of sand drop before all
! the ledges overflow and sand starts falling into the abyss.i went a
! bit wild with a simultaneous solution using queues since i missed that
! the new grains are only spawned once the prior grain has stopped.
!
! oh well, i'm doing this for coding prompts more than problem solving,
! so the time wasn't wasted, just misdirected.

   function dopartone() result(res)
      implicit none
      integer(kind=int64) :: res
      logical :: done         ! true once a grain has entered the abyss
      logical :: falling      !
      integer :: x, y

      res = -1                ! yes, -1, we want to know how many grains
      done = .false.          ! it takes ~until~ we hit the abyss

      likesands: do while (.not. done)

         ! a new grain starts to fall
         res = res + 1
         x = ORIGINX; y = ORIGINY
         falling = .true.

         downward: do while (falling .and. y < abyss)

            ! fall straight down if there is room
            if (grid(x, y + 1) == AIR) then
               y = y + 1
               cycle downward
            end if

            ! blocked. can we fall to the left?
            if (grid(x - 1, y + 1) == AIR) then
               x = x - 1
               y = y + 1
               cycle downward
            end if

            ! how about right?
            if (grid(x + 1, y + 1) == AIR) then
               x = x + 1
               y = y + 1
               cycle downward
            end if

            ! mark the grid as blocked and let's drop another grain
            falling = .false.
            grid(x, y) = SAND
         end do downward

         ! we're done when we reach the abyss
         done = y >= abyss
      end do likesands

   end function dopartone

   function doparttwo() result(res)
      implicit none
      integer :: res
      res = -1

      !=========================================================================
      !part two changes the problem so that there is a floor instead of an
      !abyss.how many units of sand fall until the opening at 500, 0 is
      !blocked by sand.
      !=========================================================================
      !
      !function parttwo:integer
      !var
      !done:boolean; true if a grain has entered the abyss
      !falling:boolean; loop control flag for the fall
      !x, y:integer
      !begin
      !parttwo= 0; start from 0 for part two
      !done= false
      !lay down a floor for part two, there is no abyss
      !
      !while not done do begin
      !
      !a new grain starts to fall
      !parttwo= parttwo + 1
      !x= ORIGINX; y= ORIGINY
      !falling= true
      !while falling do begin
      !
      !fall straight down if there is room
      !if grid[x, y + 1] = AIR then begin
      !y= y + 1
      !continue
      !end
      !can we fall to the left side?
      !if grid[x - 1, y + 1] = AIR then begin
      !x= x - 1
      !y= y + 1
      !continue
      !end
      !well then how about to the right side?
      !if grid[x + 1, y + 1] = AIR then begin
      !x= x + 1
      !y= y + 1
      !continue
      !end
      !mark the grid occupied by sand and let's drop another
      !falling= false
      !grid[x, y]= SAND
      !end
      !grid[x, y]= SAND
      !done= (x=ORIGINX) and(y=ORIGINY)
      !end
      !displaygrid
      !end

   end function doparttwo

   ! clear out global storage for a pass at the problem

   subroutine init
      implicit none

      ! clear the wall
      grid = AIR

      ! set ranges for display clipping. these will be updated during the run
      ! to tighter windows
      xrange(0) = MAXX + 10; xrange(1) = MINX - 10
      yrange(0) = MAXY + 10; yrange(1) = MINY - 10

      ! default end, will also be updated after ledge definitions are parsed.
      abyss = -1
   end subroutine init

   ! read the ledge definitions and build the grid and ledges

   subroutine load
      implicit none
      integer :: pos
      type(point_t) :: currp, nextp

      print *, "begin load"
      ! load, track extents, and render the ledges
      reader: do while (read_aoc_input(AOCIN, rec))
         if (len_trim(rec) == 0) cycle reader ! skip blank lines

         pos = 1
         currp = nullpoint()
         scanrec: do while (pos < len_trim(rec))
            ! input format is "x1,y1 -> x2,y2 -> x3,y3" where each pair is
            ! a vertical or horizontal line segment
            nextp = strpoint(rec, pos)
            xrange(0) = min(xrange(0), nextp % x)
            xrange(1) = max(xrange(1), nextp % x)
            yrange(0) = min(yrange(0), nextp % y)
            yrange(1) = max(yrange(1), nextp % y)
            if (.not. isnullpoint(currp)) call rockledge(currp, nextp)
            if (rec(pos:pos + 3) == " -> ") then
               currp = nextp
               nextp = nullpoint()
               pos = pos + 4
               cycle scanrec
            end if
            if (pos < len_trim(rec)) then
               print *, "*** error *** unexpected end of record at ", intstr(pos), " in '", trim(rec), "'"
               stop 1
            end if
         end do scanrec
      end do reader

      ! add a border around the ledges
      xrange(0) = xrange(0) - 1; xrange(1) = xrange(1) + 1
      yrange(0) = yrange(0) - 1; yrange(1) = yrange(1) + 1

      ! for part one, when a grain reaches the abyss, we're done.
      abyss = yrange(1)

      ! for part two, there is a floor and the chamber is filling up with sand
      currp % x = MINX; currp % y = yrange(1) + 1
      nextp % x = MAXX; nextp % y = currp % y
      call rockledge(currp, nextp)
   end subroutine load

   function strint(str, pos) result(res)
      implicit none
      character(len=*), intent(in) :: str
      integer, intent(inout)       :: pos
      integer                      :: res

      res = 0
      do while (pos <= len_trim(str) .and. str(pos:pos) >= "0" .and. str(pos:pos) <= "9")
         res = res * 10 + (ichar(str(pos:pos)) - ichar("0"))
         pos = pos + 1
      end do
   end function strint

   ! gfortran print *,int produces a lot of leading blanks. lfortran does not.
   ! intstr gives the integer as a string with only one leading and one
   ! trailing blank.

   function intstr(n) result(res)
      implicit none
      integer, intent(in) :: n
      character(len=:), allocatable :: res
      character(len=32)   :: buf

      buf = ""
      write (buf, *) n
      allocate (character(len=len_trim(adjustl(buf))) :: res)
      res = trim(adjustl(buf))
   end function intstr

   ! parse point coordinates from string s starting from offset p.
   ! p advances to the position after the point coordinates.

   function strpoint(s, p) result(res)
      implicit none
      character(len=*), intent(in) :: s
      integer, intent(inout)       :: p
      type(point_t)                :: res

      res % x = strint(s, p)
      p = p + 1
      res % y = strint(s, p)
   end function strpoint

   function padleft(s, n) result(res)
      implicit none
      character(len=*), intent(in)  :: s
      integer, intent(in)           :: n
      character(len=:), allocatable :: res

      allocate (character(len=n) :: res)
      res = s
      do while (len(res) < n)
         res = " "//res
      end do
   end function padleft

   recursive subroutine vertical(p, q, c)
      implicit none
      type(point_t), intent(in) :: p, q
      character(len=*)          :: c
      integer                   :: y
      if (p % y > q % y) then
         call vertical(q, p, c)
      else
         do y = p % y, q % y
            grid(p % x, y) = c(1:1)
         end do
      end if
   end subroutine vertical

   recursive subroutine horizontal(p, q, c)
      implicit none
      type(point_t), intent(in) :: p, q
      character(len=*)          :: c
      integer                   :: x
      if (p % x > q % x) then
         call horizontal(q, p, c)
      else
         do x = p % x, q % x
            grid(x, p % y) = c(1:1)
         end do
      end if
   end subroutine horizontal

   subroutine rockledge(p, q)
      implicit none
      type(point_t), intent(in) :: p, q
      if (p % x == q % x) then
         call vertical(p, q, ROCK)
      else
         call horizontal(p, q, ROCK)
      end if
   end subroutine rockledge

   ! displayable representation for formatted output

   function pointstr(p) result(res)
      implicit none
      type(point_t), intent(in) :: p
      character(len=9)          :: res
      res = "("//padleft(intstr(p % x), 3)//","//padleft(intstr(p % y), 3)//")"
   end function pointstr

   ! returns a new empty point

   function nullpoint() result(res)
      implicit none
      type(point_t) :: res
      res % x = -1; res % y = -1
   end function nullpoint

   ! is the point empty?

   function isnullpoint(p) result(res)
      implicit none
      type(point_t), intent(in) :: p
      logical :: res
      res = (p % x == -1) .or. (p % y == -1)
   end function isnullpoint

   ! display grid with rock ledges and sand that has dropped so far
   ! needs much work

   subroutine displaygrid
      implicit none
      integer                       :: x, y
      type(point_t)                 :: p
      character(len=:), allocatable :: str

      ! this won't work if coordinates go negative
      allocate (character(len=9*2 + 2*2 + xrange(1) - xrange(0)) :: str)
      str = repeat(" ", 9 * 2 + 2 * 2 + xrange(1) - xrange(0))
      print *
      ! display (500,0), the source of the sand...
      p % x = xrange(0); p % y = 0
      str = pointstr(p)//": "
      do x = xrange(0), xrange(1)
         if (x == ORIGINX) then
            str = trim(str)//"|"
         else
            str = trim(str)//"="
         end if
      end do
      p % x = xrange(1)
      str = trim(str)//" :"//pointstr(p)
      print *, str

      ! now display the active grid area and border...
      do y = 1, yrange(1)
         p % x = xrange(0); p % y = y
         str = pointstr(p)//": "
         do x = xrange(0), xrange(1)
            str = trim(str)//grid(x, y)
         end do
         p % x = xrange(1)
         str = trim(str)//" :"//pointstr(p)
         print *, str
      end do
      print *
   end subroutine displaygrid

end program solution
