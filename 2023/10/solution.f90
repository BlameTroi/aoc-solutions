! solution.f90 -- 2022 day 10 -- -- troy brumley blametroi@gmail.com

! advent of code 2023 day 10 -- pipe maze
!
! for part 1 of day 10 we're doing a variation of the longest path
! through a maze. while there's no exit point, find the point on the
! maze furthest from the entry point. and of course there are
! extraneous and unconnected paths in the maze.
!
! for part 2 of day 10 we need to count the number of points in the
! maze that are completely within the looping path. a simple flood
! fill won't work without a lot of work around the issue of parallel
! adjecent pipes opening into what might look to be an area within the
! loop but really isn't. after being stuck for several days i checked
! for hints but none of them led to a solution for me. eventually i
! saw "point in a polygon" and some fruitful searching led me to ray
! casting.
!
! there were a couple of small test datasets provided in the problem
! description, and i even made a couple of different ones as i worked
! through this. given the varying dimensions i'm going to finally try
! to dynamically allocate the globals after first inspecting the data
! at runtime to determine the limits.
!
! limits from observation of data sets
!  testone.txt is small (5, 5) but has bogus pipes
!  testtwo.txt has the same loop but no bogus pipes
!  testten.txt (21, 12)
!  testfoura/b (9, 11)
!  ... a/b same answer but the arrangment has a need
!      to fill between parallel pipes to get all the
!      outside points.

program solution

   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input
   use aochelpers, only: is_digit, char_int

   implicit none

   integer, parameter            :: AOCIN = 10
   integer(kind=int64)           :: part_one, part_two  ! results
   character(len=max_aoc_reclen) :: rec

   integer :: map_dimensions(1:2, 1:2) ! row and column min and max
   integer :: maxrows, maxcols ! our geometry, will be determined from scan of input

   ! glyphs representing a node in the maze in the maze.
   !            origin = 'S'   origin, must infer shape
   !            ground = '.'   ornt  side     ornt  side
   !            ns     = '|'   vert  both
   !            ew     = '-'                  horz  both
   !            ne     = 'L'   vert  left     horz  bott
   !            nw     = 'J'   vert  right    horz  bott
   !            sw     = '7'   vert  right    horz  top
   !            se     = 'F'   vert  left     horz  top

   integer :: begrow, begcol ! the 's' or entry node location
   ! the original data
   character(len=1), allocatable :: glyphs(:, :)
   ! as we learn a node's status, mark it in the appropriate array
   character(len=1), allocatable :: filled(:, :)
   character(len=1), allocatable :: onloop(:, :)
   character(len=1), allocatable :: inside(:, :)

   ! initialize
   part_one = 0; part_two = 0
   rec = ""
   call open_aoc_input(AOCIN)

   ! analyze input, allocate arrays, and load
   call init
   call load

   part_one = dopartone()

   ! report and close
   print *
   print *, part_one, "part one"
   print *, part_two, "part two"
   print *

   call close_aoc_input(AOCIN)

contains

   ! rows will be the same in this dataset, but i've seen at least one where they aren't

   subroutine get_dimensions
      implicit none
      integer :: i
      map_dimensions(1, 1) = huge(map_dimensions) ! min row size
      map_dimensions(1, 2) = huge(map_dimensions) ! min col size
      map_dimensions(2, 1) = -huge(map_dimensions) ! max row
      map_dimensions(2, 2) = -huge(map_dimensions) ! max col
      call rewind_aoc_input(AOCIN)
      i = 0
      do while (read_aoc_input(AOCIN, rec))
         i = i + 1
         map_dimensions(1, 1) = i
         map_dimensions(1, 2) = min(map_dimensions(1, 2), len_trim(rec))
         map_dimensions(2, 1) = i
         map_dimensions(2, 2) = max(map_dimensions(2, 2), len_trim(rec))
      end do
      if (map_dimensions(1, 2) /= map_dimensions(2, 2)) then
         print *, "*** error *** input is not rectangular!"
         stop 1
      end if
   end subroutine get_dimensions

   subroutine init
      implicit none
      call get_dimensions
      maxrows = map_dimensions(2, 1)
      maxcols = map_dimensions(2, 2)
      write (*, "('maze dimensions are ', i3, 'x', i3, '.')") maxrows, maxcols
      allocate (character(len=1) :: glyphs(1:maxrows, 1:maxcols))
      allocate (character(len=1) :: filled(1:maxrows, 1:maxcols))
      allocate (character(len=1) :: onloop(1:maxrows, 1:maxcols))
      allocate (character(len=1) :: inside(1:maxrows, 1:maxcols))
      glyphs = " "
      filled = " "
      onloop = " "
      inside = " "
      begrow = -1
      begcol = -1
   end subroutine init

   ! read the maze map and parse it as much as we can. basically, replace
   ! the origin glyph with a valid pipe shape.

   subroutine load
      implicit none
      integer :: r, c
      logical :: b
      call rewind_aoc_input(AOCIN)

      do r = 1, maxrows
         b = read_aoc_input(AOCIN, rec)
         do c = 1, maxcols
            glyphs(r, c) = rec(c:c)
            if (glyphs(r, c) == "S") then
               begrow = r
               begcol = c
            end if
         end do
      end do

      if (begrow == -1) then
         print *, "*** error *** failed to find origin point"
         stop 1
      end if

      glyphs(begrow, begcol) = infer_origin()

      if (glyphs(begrow, begcol) == "S") then
         print *, "*** error *** could not resolve origin pipe shape"
         stop 1
      end if

   end subroutine load

   ! according to our spec, a pipe can have only one two access points from
   ! among directions north east south and west. by examining the neighboring
   ! pipes will reveal the shape of the origin pipe.

   function infer_origin() result(res)
      implicit none
      character(len=1) :: res        ! replacement glyphe holding proper origin pipe shape
      logical          :: n, e, s, w ! directions you can move from the origin based on neighbor pipes

      res = "S"
      n = .false.; e = .false.; s = .false.; w = .false.

      ! how do our neighbors connect?
      if (begrow > 1) then
         n = index("7|F", glyphs(begrow - 1, begcol)) > 0
      end if
      if (begcol < maxcols) then
         e = index("J-7", glyphs(begrow, begcol + 1)) > 0
      end if
      if (begrow < maxrows) then
         s = index("J|L", glyphs(begrow + 1, begcol)) > 0
      end if
      if (begcol > 1) then
         w = index("L-F", glyphs(begrow, begcol - 1)) > 0
      end if

      ! only two of n e s w should be true. if not, that's a data
      ! problem but it's not checked for here.
      if (n .and. s) then
         res = "|"
      else if (n .and. w) then
         res = "J"
      else if (n .and. e) then
         res = "L"
      else if (w .and. e) then
         res = "-"
      else if (s .and. w) then
         res = "7"
      else if (s .and. e) then
         res = "F"
      end if

   end function infer_origin

   ! common accessors and checks. they all guard against out of bounds access and
   ! return sensible values when that happens. it's cleaner to just call these
   ! functions and have them return a "not here" response that wrapping all those
   ! calls in boundary checks.

   function is_valid_coord(r, c) result(res)
      implicit none
      integer, intent(in) :: r, c
      logical             :: res
      res = (r >= 1) .and. (r <= maxrows) .and. (c >= 1) .and. (c <= maxcols)
   end function is_valid_coord

   ! is this particular point on the loop?

   function is_on_loop(r, c) result(res)
      implicit none
      integer, intent(in) :: r, c
      logical             :: res
      res = .false.
      if (is_valid_coord(r, c)) res = onloop(r, c) /= " "
   end function is_on_loop

   ! could we go in a cardinal direction from a point? the pipe shape
   ! at the current point has to be able to reach that direction, and
   ! the point there can't be marked as part of the loop.

   function can_go_north(r, c) result(res)
      implicit none
      integer, intent(in) :: r, c
      logical             :: res
      res = .false.
      if (.not. is_valid_coord(r, c) .or. .not. is_valid_coord(r - 1, c)) return
      res = (.not. is_on_loop(r - 1, c)) .and. (index("|LJ", glyphs(r, c)) > 0)
   end function can_go_north

   function can_go_east(r, c) result(res)
      implicit none
      integer, intent(in) :: r, c
      logical             :: res
      res = .false.
      if (.not. is_valid_coord(r, c) .or. .not. is_valid_coord(r, c + 1)) return
      res = (.not. is_on_loop(r, c + 1)) .and. (index("-LF", glyphs(r, c)) > 0)
   end function can_go_east

   function can_go_south(r, c) result(res)
      implicit none
      integer, intent(in) :: r, c
      logical             :: res
      res = .false.
      if (.not. is_valid_coord(r, c) .or. .not. is_valid_coord(r + 1, c)) return
      res = (.not. is_on_loop(r + 1, c)) .and. (index("|F7", glyphs(r, c)) > 0)
   end function can_go_south

   function can_go_west(r, c) result(res)
      implicit none
      integer, intent(in) :: r, c
      logical             :: res
      res = .false.
      if (.not. is_valid_coord(r, c) .or. .not. is_valid_coord(r, c - 1)) return
      res = (.not. is_on_loop(r, c - 1)) .and. (index("-J7", glyphs(r, c)) > 0)
   end function can_go_west

   ! part one
   !
   ! walk around the loop from point to point. due to the loop nature of the
   ! pipes, we know that there are only two paths into (or out of) a point.
   !
   ! picking a direction from the origin, walk until the end is reached. this is
   ! defind as there being no unvisted point available. the solution to the
   ! problem is 1/2 this number of steps.

   ! every journey begins with a single step. here we step into the maze at the
   ! origin, pick one of the two paths available, and start walking.
   ! recursive function for the walk. returns distance.

   recursive function loop_walker(r, c) result(res)
      implicit none
      integer, intent(in) :: r, c
      integer :: res

      res = 1                         ! count this point
      onloop(r, c) = glyphs(r, c)       ! remember we've been visited
      if (can_go_north(r, c)) then
         res = res + loop_walker(r - 1, c)
      else if (can_go_east(r, c)) then
         res = res + loop_walker(r, c + 1)
      else if (can_go_south(r, c)) then
         res = res + loop_walker(r + 1, c)
      else if (can_go_west(r, c)) then
         res = res + loop_walker(r, c - 1)
      end if
   end function loop_walker

   ! driver for part one
   function dopartone() result(res)
      implicit none
      integer :: res
      integer :: i
      i = loop_walker(begrow, begcol)
      res = (i / 2) + mod(i, 2)
   end function dopartone

end program solution

!
!{ ---------------------------------------------------------
!  part two.
!
!  find out how many points are inside the loop. the tricky
!  part is that it is possible to seep through parallel pipes
!  in which case the points are not enclosed. this is a paint
!  fill sort of problem.
!  --------------------------------------------------------- }
!
!
!{ fill points in the maze starting from r, c. can be called with
!  invalid coordinates and quietly ignores the request. it also quietly
!  ignores the request if the point has been filled already or is part
!  of the loop. }
!
!procedure filler(r, c : integer)
!
!begin
!  if not validcoord(r, c) then
!    exit
!  if (filled[r, c] <> ' ') or (onloop[r, c] <> ' ') then
!    exit
!  filled[r, c] = 'O'
!  filler(r-1, c-1); filler(r-1, c); filler(r+1, c+1)
!  filler(r,   c-1);                 filler(r,   c+1)
!  filler(r+1, c+1); filler(r+1, c); filler(r+1, c+1)
!end
!
!
!{ any outer row or column grid that isn't on the loop can be filled.
!  most of the requests after the first couple will return without
!  doing anything as the nodes will have been marked filled.
!
!  while the spec allows filling to bleed through parallel pipes in the
!  maze, that's being deferred to a later step.
!
!  when this fill is done, any nodes that are neither filled nor on the
!  loop are candidates for part two. attempting to figure out the
!  parallel pipes. }
!
!procedure firstfill
!
!var
!   r : integer
!
!begin
!   for r = low(glyphs) to high(glyphs) do begin
!      filler(r, low(glyphs[r]))
!      filler(r, high(glyphs[r]))
!   end
!end
!
!
!{ is this point inside the loop? use raycasting to check. }
!
!function interior(r, c : integer) : boolean
!
!var
!   ray       : integer
!   crossings : integer
!
!begin
!   interior = false
!   crossings = 0
!
!   if (r = low(glyphs)) or (r = high(glyphs)) then
!      exit
!   if (c = low(glyphs[r])) or (c = high(glyphs[r])) then
!      exit
!
!   for ray = low(glyphs[r]) to c do begin
!      if onloop[r, ray] in [' ', '-', 'J', 'L'] then
!         continue
!      crossings = crossings + 1
!   end
!
!   interior = (crossings > 0) and ((crossings mod 2) = 1)
!end
!
!
!{ check any remaining unmarked points in the maze. if it isn't filled
!  or on the loop, it might be completely contained within the loop.
!  use raycasting to see if it is, and if it isn't, fill it. }
!
!function checkremaining : integer
!
!var
!   r, c : integer
!   count : integer
!
!begin
!   count = 0
!   for r = low(glyphs) to high(glyphs) do begin
!      for c = low(glyphs[r]) to high(glyphs[r]) do begin
!         if (onloop[r, c] = ' ') and (filled[r, c] = ' ') then begin
!            inside[r, c] = '?'
!            if interior(r, c) then begin
!               count = count + 1
!               inside[r, c] = 'I'
!            end
!         end
!      end
!   end
!   checkremaining = count
!end
!
!
!{ find every node that is neither filled nor one of the loop nodes. is
!  there a path from it to a filled node? if so, fill it. whatever is
!  left after all that must be an interior node. }
!
!function parttwo : integer
!
!begin
!   firstfill
!   parttwo = checkremaining
!end
!
!{ mainline }
!
!var
!   i : integer
!
!begin
!   aocopen
!   i = partone
!   writeln('part one: ', i)
!   i = parttwo
!   writeln('part two: ', i)
!   aocclose
!end.
