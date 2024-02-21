! solution.f90 -- 2022 day 15 -- beacon exclusion zone -- troy brumley blametroi@gmail.com

! advent of code 2022 day 15 -- beacon exclusion zone

! given sensors that can identify the closest beacon, cast a net of sensors to
! map beacons. then determine where a beacon can not be in a specific row.

! part two ups the volume (e, not n). there's a lot of unpruned experimental
! stuff in here, but the right answers are found, and it wasn't as slow as i
! feared to check all the points on a line between x=0 and x=4000000. leaving
! the pruning for a future pass. there's stuff to learn from mistakes and
! missteps.
!
! the unpruned code includes a lot of debug print type stuff that i want to
! pull into some common modules at some point. commenting out most of the
! actual writes for now.

program solution

   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input
   use aochelpers, only: is_digit, char_int, str_int64

   implicit none

   integer, parameter            :: AOCIN = 10
   integer(kind=int64)           :: part_one, part_two  ! results
   character(len=max_aoc_reclen) :: rec

   integer, parameter :: MAXDIM = 30 ! only 23 in test data, but pad

   type sensor_t
      integer(kind=int64) :: x, y ! location
      integer(kind=int64) :: bx, by ! detected beacon location
      integer(kind=int64) :: distance ! to beacon
      integer(kind=int64) :: tydistance ! would x, ty be in range?
      integer(kind=int64) :: tyminx ! x crosses target y at min
      integer(kind=int64) :: tymaxx ! and max
      integer(kind=int64) :: nx, ny ! coordinates of diamond points at range+1
      integer(kind=int64) :: ex, ey ! redundancies exist, but this makes code easier
      integer(kind=int64) :: sx, sy
      integer(kind=int64) :: wx, wy
   end type sensor_t

   type range_t
      integer(kind=int64) :: lo
      integer(kind=int64) :: hi
   end type range_t

   ! the slope intercept form of the line, y = m*x + b. for this problem we know
   ! that slope is |1|, but we'll carry it around anyway. the interesting points
   ! of the line lie in the region from 0 to 4,000,000 for both x and y.
   ! according to the problem statement, we should find exactly one point that
   ! is outside the range of any sensor. sadly, my best approach once i have
   ! this information is to brute force scan.

   integer(kind=int64), parameter  :: MINCOORD = 0
   integer(kind=int64), parameter  :: MAXCOORD = 4000000 ! for small test data, 20

   type line_t
      integer(kind=int64) :: m ! y=m*x +b, in this problem m is + or - 1
      integer(kind=int64) :: b
      integer(kind=int64) :: x_at_y0              ! intercepts, but not the ones
      integer(kind=int64) :: y_at_x4m             ! we usually look for
   end type line_t

   type(sensor_t) :: sensors(0:MAXDIM)
   integer :: numsensors
   integer(kind=int64) :: minx, miny
   integer(kind=int64) :: maxx, maxy
   integer(kind=int64) :: targety
   integer(kind=int64) :: mintyx
   integer(kind=int64) :: maxtyx

   type(range_t) :: ranges(0:MAXDIM)
   integer :: numranges

   ! lines are in groups of four, with directions ...
   integer, parameter :: wn = 0
   integer, parameter :: se = 1
   integer, parameter :: sw = 2
   integer, parameter :: en = 3
   type(line_t) :: lines(0:MAXDIM * 4)
   integer :: numlines

   integer(kind=int64) :: foundx, foundy
   ! initialize

   part_one = 0; part_two = 0
   rec = ""
   call open_aoc_input(AOCIN)

   call init
   call load
   call display_sensors

   part_one = do_part_one()

   part_two = do_part_two()

   ! report and close

   write (*, *)
   write (*, *) part_one, "part one"
   write (*, *) part_two, "part two"
   write (*, *)

   call close_aoc_input(AOCIN)

contains

   ! part one -- where aren't the beacons
   !
   ! for part one we want to find out how many grid positions on a particular
   ! row (y=2,000,000 in the live data, and y=10 in the test data).
   !
   ! a grid and count solution will work for the test data to check my work, but
   ! the live data is looking too big for that to be practical.
   !
   ! what we have to do is determine the x range along the target y line that
   ! falls between the two x,y points that mark the distance to the nearest
   ! beacon. since the distances are 'manhattan' distances the calculation
   ! should be straight forward algebra.

   function do_part_one() result(res)
      implicit none
      integer(kind=int64) :: res
      integer             :: i, j
      integer(kind=int64) :: adjx
      character(len=*), parameter :: FMTSENSOR = &
                                     "('sensor',i3,' at ',a,' has a range of ',i8,' and can see along y=',i8,' from ',i8,' to ',i8)"
      character(len=200) :: buf

      res = 0

      ! scan for sensors that can see the target y line, and then find the
      ! points of intersection of a circle of radius distance-to-beacon and the
      ! line y=targety. there are either one or two possible points, but the
      ! problem statement says 'no ties' so a tangent line y=targety is not
      ! possible.
      !
      ! so range - abs(sensor y - target y) gives the adjustment to sensor x.

      j = 0

      ! write (*, *)
      ! write (*, "('of ', i0, ' sensors, the following are of interest:')") numsensors
      ! buf = sensor_range_prthdr()
      ! write (*, *) trim(buf)
      do i = 0, numsensors - 1
         associate (s => sensors(i))
            if (s % distance > s % tydistance) then
               adjx = s % distance - abs(s % y - targety)
               s % tyminx = s % x - adjx
               s % tymaxx = s % x + adjx
               ! buf = sensor_range_prtstr(i)
               ! write (*, *) trim(buf)
               ranges(j) % lo = s % tyminx; ranges(j) % hi = s % tymaxx
               j = j + 1
               mintyx = min(mintyx, s % tyminx)
               maxtyx = max(maxtyx, s % tymaxx)
            end if
         end associate
      end do

      numranges = j

      write (*, *) ""
      write (*, *) "sorting ranges..."
      call sort_ranges
      ! do i = 0, numranges - 1
      !    write (*, "(*(i14))") ranges(i) % lo, ranges(i) % hi
      ! end do

      write (*, *)
      write (*, *) "collapsing ranges..."
      do while (collapse_ranges())
         ! could require multiple passes
      end do

      write (*, *)
      write (*, *) "merging ranges..."
      do while (merge_ranges())
         ! could require multiple passes
      end do

      ! write (*, *)
      ! write (*, *) "collapsed and merged ranges:"
      ! do i = 0, numranges - 1
      !    write (*, "(*(i14))") ranges(i) % lo, ranges(i) % hi
      ! end do

      ! this is a wrong answer for the big data
      write (*, *)
      write (*, *) "minimum and maximum are:"
      write (*, "(*(i14))") mintyx, maxtyx
      write (*, *)
      write (*, "('for a maximum of ', i0)") abs(mintyx) + abs(maxtyx)

      res = abs(mintyx) + abs(maxtyx)
   end function do_part_one

   ! display and format helpers

   function base36(i) result(res)
      implicit none
      integer, intent(in) :: i
      character(len=1)    :: res
      if (i > 36) then
         res = "?"
      else if (i < 0) then
         res = "-"
      else if (i < 10) then
         res = achar(ichar("0") + i)
      else
         res = achar(ichar("A") + i - 10)
      end if
   end function base36

   function point_str(x, y) result(res)
      implicit none
      integer(kind=int64), intent(in) :: x, y
      character(len=:), allocatable :: res
      character(len=64) :: buf
      integer :: n
      buf = ""
      n = 3                    ! [,]
      write (buf, "(i8)") x    ! l'x
      n = n + len_trim(buf)
      write (buf, "(i8)") y    ! l'y
      n = n + len_trim(buf)
      allocate (character(len=n) :: res)
      write (res, "('(',i8,',',i8')')") x, y
   end function point_str

   ! distance calculation and testing using manhattan (rectilinear) distances
   !
   ! distance = abs(x1 - x2) + abs(y1 - y2)
   !
   ! does a sensor exclude any points along the target y line? yes if the
   ! perpendicular distance abs(y1 - target y) is <= the sensor's beacon
   ! range.

   function calculate_distance(x1, y1, x2, y2) result(res)
      implicit none
      integer(kind=int64), intent(in) :: x1, y1, x2, y2
      integer(kind=int64) :: res
      res = abs(x1 - x2) + abs(y1 - y2)
   end function calculate_distance

   ! sort covered ranges

   subroutine sort_ranges
      implicit none
      integer       :: i
      logical       :: did
      type(range_t) :: swap

      did = .true.
      do while (did)
         did = .false.
         do i = 0, numranges - 2
            if (ranges(i) % lo < ranges(i + 1) % lo) cycle
            if (ranges(i) % lo == ranges(i + 1) % lo .and. ranges(i) % hi <= ranges(i + 1) % hi) cycle
            did = .true.
            swap = ranges(i)
            ranges(i) = ranges(i + 1)
            ranges(i + 1) = swap
         end do
      end do
   end subroutine sort_ranges

   ! find any ranges completely contained in another and squeeze them out

   function collapse_ranges() result(res)
      implicit none
      logical :: res
      integer :: i, j

      res = .false.
      do i = 0, numranges - 1
         if (ranges(i) % lo == huge(int64)) cycle
         do j = 0, numranges - 1
            if (j == i) cycle
            if (ranges(i) % lo >= ranges(j) % lo .and. ranges(i) % hi <= ranges(j) % hi) then
               res = .true.
               ranges(i) % lo = huge(int64); ranges(i) % hi = huge(int64)
               exit
            end if
         end do
      end do

      if (res) then
         call sort_ranges
         do i = 0, numranges
            if (ranges(i) % lo /= huge(int64)) cycle
            numranges = i
            exit
         end do
      end if

   end function collapse_ranges

   ! merge adjacent ranges

   function merge_ranges() result(res)
      implicit none
      logical :: res
      integer :: i

      res = .false.

      do i = 1, numranges - 1
         if (ranges(i) % lo == huge(int64)) cycle
         if (ranges(i + 1) % lo == huge(int64)) cycle
         if (ranges(i + 1) % lo <= ranges(i) % hi) then
            res = .true.
            ranges(i) % hi = ranges(i + 1) % hi
            ranges(i + 1) % lo = huge(int64)
            ranges(i + 1) % hi = huge(int64)
         end if
      end do

      if (res) then
         call sort_ranges
         do i = 0, numranges
            if (ranges(i) % lo /= huge(int64)) cycle
            numranges = i
            exit
         end do
      end if
   end function merge_ranges

   ! the standard clear, load, and print helpers

   subroutine init
      implicit none
      integer :: i

      do i = 0, MAXDIM
         associate (s => sensors(i))
            s % x = 0; s % y = 0
            s % bx = 0; s % by = 0
            s % distance = 0
            s % tydistance = 0
            s % tyminx = 0
            s % tymaxx = 0
         end associate
         ranges(i) % lo = 0; ranges(i) % hi = 0
      end do
      numsensors = 0
      numranges = 0
      minx = huge(int64); miny = huge(int64)
      maxx = -huge(int64); maxy = -huge(int64)
      mintyx = huge(int64); maxtyx = -huge(int64)
      targety = -1
   end subroutine init

   ! remove non numeric characters from a copy of a string to make
   ! it readable by read(str,*).

   function sanatize(s) result(res)
      implicit none
      character(len=*), intent(in) :: s
      character(len=len(s)) :: res
      integer :: p
      res = repeat(" ", len(s))
      do p = 1, len(s)
         if (index("0123456789-", s(p:p)) /= 0) res(p:p) = s(p:p)
      end do
   end function sanatize

   ! loads the sensor and beacon data and might also create a visualization
   ! grid if i need one.
   !
   ! sample input:
   !
   ! Sensor at x=2, y=18: closest beacon is at x=-2, y=15
   ! Sensor at x=9, y=16: closest beacon is at x=10, y=16
   ! Sensor at x=13, y=2: closest beacon is at x=15, y=3
   ! Sensor at x=12, y=14: closest beacon is at x=10, y=16
   ! Target y=10

   subroutine load
      implicit none
      integer :: i
      character(len=:), allocatable :: t

      !write (*, *) "loading..."
      i = 0
      do while (read_aoc_input(AOCIN, rec))
         !write (*, *) trim(rec)
         if (rec(1:9) /= "Sensor at") exit
         associate (s => sensors(i))
            t = sanatize(trim(rec))
            read (t, *) s % x, s % y, s % bx, s % by
            s % distance = calculate_distance(s % x, s % y, s % bx, s % by)
            minx = min(minx, s % x); miny = min(miny, s % y)
            maxx = max(maxx, s % x); maxy = max(maxy, s % y)
            minx = min(minx, s % bx); miny = min(miny, s % by)
            maxx = max(maxx, s % bx); maxy = max(maxy, s % by)
            numsensors = numsensors + 1
            deallocate (t)
         end associate
         i = i + 1
      end do

      if (rec(1:9) == "Target y=") then
         t = sanatize(trim(rec))
         read (t, *) targety
      end if

      do i = 0, numsensors - 1
         sensors(i) % tydistance = calculate_distance(sensors(i) % x, sensors(i) % y, sensors(i) % x, targety)
      end do

   end subroutine load

   function sensor_prtstr(i) result(res)
      implicit none
      integer, intent(in) :: i
      character(len=*), parameter :: fmtsen = "(i7, '  ', a19, '  ', a19, '  ', i7, '  ', i7)"
      character(len=len(fmtsen) + 7*3 + 19*2 + 10) :: res
      res = ""
      associate (s => sensors(i))
         write (res, fmtsen) i, point_str(s % x, s % y), point_str(s % bx, s % by), s % distance, s % tydistance
      end associate
   end function sensor_prtstr

   function sensor_range_prtstr(i) result(res)
      implicit none
      integer, intent(in) :: i
      character(len=*), parameter :: fmtsenrng = "(i7, ' ', a19, ' ', i8, ' ', i8, ' ' i8, ' ', i8)"
      character(len=(len(fmtsenrng) + 8*5 + 19 + 10)) :: res
      res = ""
      associate (s => sensors(i))
         write (res, fmtsenrng) i, point_str(s % x, s % y), s % distance, s % tydistance, s % tyminx, s % tymaxx
      end associate
   end function sensor_range_prtstr

   function sensor_range_prthdr() result(res)
      implicit none
      character(len=*), parameter :: hdrsen = " sensor      at coordinates    range   sees y     from       to"
      character(len=len(hdrsen)) :: res
      res = hdrsen
   end function sensor_range_prthdr

   function sensor_prthdr() result(res)
      implicit none
      character(len=*), parameter :: hdrsen = " sensor       at coordinates       sees beacon at    range     to y"
      character(len=len(hdrsen)) :: res
      res = hdrsen
   end function sensor_prthdr

   subroutine display_sensors
      implicit none
      character(len=*), parameter :: fmtcor = "('used grid coordinates range from ', a19, ' to ', a19)"
      character(len=200) :: buf
      integer :: i
      return
      write (*, *)
      write (*, *) "our data"
      write (*, *)
      write (*, *) sensor_prthdr()
      write (*, *)
      do i = 0, numsensors - 1
         buf = sensor_prtstr(i)
         write (*, *) trim(buf)
      end do
      write (*, *)
      write (*, fmtcor) point_str(minx, miny), point_str(maxx, maxy)
      write (*, *)
      write (*, "('target y line is ', i7)") targety
      write (*, *)
   end subroutine display_sensors

   ! part two -- where is the beacon

   ! we are told that the beacon's x and y coordinates can range from 0 to
   ! 4000000. determine the beacon's tuning frequency (x * 4000000 + y). there
   ! is only one grid coordinate that can hold the beacon, what is its
   ! frequency?
   !
   ! in the small test data, the location is x=14 y=11 for a frequency of
   ! 56000011.
   !
   ! a brute force scan is feasible but will take forever. i'm trying to
   ! visualize finding gaps but it isn't coming clear to me yet. one idea to
   ! play with is sampling, just to keep me in the problem space and see what i
   ! might learn.
   !
   ! as i think about it, unlike circles the manhattan distance gives us diamond
   ! shapes. so there won't be the same sort of gap shapes that exist when
   ! working with cartesian coordinates. gaps will be along parallel diagonal or
   ! cardinal lines.
   !
   ! i spent too much time trying to find a way to rotate the whole graph so i
   ! could workd with rectangles instead of diamonds, but actually the diamond
   ! shape is fine. since we know there is only one point that is not covered by
   ! the sensors, it must be on a line that runs along one of the four edges of
   ! the coverage diamond at range+1.
   !
   ! the lines all have slopes of |1| so it's easy to figure out the
   ! intersections.
   !
   ! the answer to this is the tuning frequency of the beacon which is x *
   ! 4,000,000 + y. For the small test data is 56000011 for x=14, y = 11.
   !
   ! time to go brute force on this. find x and y intercept of each line, then
   ! stepping along them is a matter of addition or subtraction.

   ! y = mx + b
   ! if y = 0, x = -b/m,
   ! if x = 0, y = b
   !

   function do_part_two() result(res)
      implicit none
      integer(kind=int64) :: res
      integer :: i, j
      integer(kind=int64) :: x, y
      character(len=*), parameter :: fmtlin = "('    line ', a3, ' : ', a19, '-', a19)"

      x = 0; y = 0
      foundx = 0; foundy = 0
      res = -1

      write (*, *)
      ! determine endpoints of the open lines, all the lines have a slope of
      ! |1|. points on these lines are outside the range of the sensor they
      ! surround. given problem statement, the can be only one point that holds
      ! the out of range beacon. it must be on one of these lines.
      !
      ! n-e, w-s, w-n, s-e are the lines

      j = 0
      do i = 0, numsensors - 1
         associate (s => sensors(i))

            ! the lines are defined by segments from the four diamon points that
            ! are just out of range. these are nxy, exy, sxy, and wxy, giving
            ! segments from w-n, s-e, s-w, and e-n.

            s % nx = s % x
            s % ny = s % y - s % distance - 1

            s % ex = s % x + s % distance + 1
            s % ey = s % y

            s % sx = s % x
            s % sy = s % y + s % distance + 1

            s % wx = s % x - s % distance - 1
            s % wy = s % y

            ! find the line segments and add to lines. get_line populates the
            ! full line_t and gives us endpoints for our brute force search

            lines(j + wn) = get_line(s % wx, s % wy, s % nx, s % ny)
            lines(j + se) = get_line(s % sx, s % sy, s % ex, s % ey)
            lines(j + sw) = get_line(s % sx, s % sy, s % wx, s % wy)
            lines(j + en) = get_line(s % ex, s % ey, s % nx, s % ny)
            j = j + 4
         end associate
      end do
      numlines = j

      ! now that we've built the lines, the slow process of checking them starts
      ! point by point, line by line, against each sensor ...
      lineloop: do i = 0, numlines - 1
         associate (l => lines(i))
            write (*, "('checking line ', i0)") i
            !write (*, *) "checking line ", l % m, l % b, l % x_at_y0, l % y_at_x4m
            xloop: do x = MINCOORD, MAXCOORD
               y = l % m * x + l % b
               ! if (mod(x, 1000000) == 0) write (*, *) point_str(x, y)
               ! yes, this could be smarter
               if (y < MINCOORD .or. y > MAXCOORD) cycle xloop
               if (can_be_seen(x, y)) cycle xloop
               foundx = x
               foundy = y
               res = x * MAXCOORD + y
               write (*, *) "point can't be seen! ", point_str(x, y)
               write (*, *) "answer: ", res
               return
            end do xloop
         end associate
      end do lineloop
   end function do_part_two

   ! is a particular point in the valid range for this problem? 0->4000000

   function possible_location(x, y) result(res)
      implicit none
      integer(kind=int64), intent(in) :: x, y
      logical :: res
      res = (x >= MINCOORD) .and. (x <= MAXCOORD) .and. (y >= MINCOORD) .and. (y <= MAXCOORD)
   end function possible_location

   ! is a particular point visible to any sensor?

   function can_be_seen(x, y) result(res)
      implicit none
      integer(kind=int64), intent(in) :: x, y
      logical :: res
      integer(kind=int64) :: d
      integer :: i

      res = .false.
      do i = 0, numsensors - 1
         associate (s => sensors(i))
            d = calculate_distance(x, y, s % x, s % y)
            if (d <= s % distance) then
               res = .true.
               return
            end if
         end associate
      end do
   end function can_be_seen

   ! given two points on a line (in this case, the diamond tips of our
   ! 'just outside of range' boundary for a sensor) return a line structure
   ! describing the line in terms of the slope intercept formula and provide
   ! the coordates of the points where x = 4000000 or y = 0. i don't have
   ! a faster solution yet than iterating over the line between those
   ! points to see if it's in range of anything.

   recursive function get_line(ax, ay, bx, by) result(res)
      implicit none
      integer(kind=int64), intent(in) :: ax, ay, bx, by
      type(line_t) :: res

      ! init
      res % b = 0
      res % m = 0
      res % x_at_y0 = 0
      res % y_at_x4m = 0

      ! guard clause, ignore horiztonal and vertical lines for now.
      ! the value of m=0 is used to flag the line as impossible
      if (ax == bx .or. ay == by) then
         return
      end if

      ! redispatch to get these all leaning the same way, left down
      ! right up.
      if (ax > bx) then
         res = get_line(bx, by, ax, ay)
         return
      end if

      call slope_intercept(res % m, res % b, ax, ay, bx, by)

      res % x_at_y0 = find_x(res, 0_int64)
      res % y_at_x4m = find_y(res, 4000000_int64)
   end function get_line

   function find_x(l, y) result(res)
      implicit none
      type(line_t), intent(in) :: l
      integer(kind=int64), intent(in) :: y
      integer(kind=int64) :: res
      res = (y - l % b) / l % m
   end function find_x

   function find_y(l, x) result(res)
      implicit none
      type(line_t), intent(in) :: l
      integer(kind=int64), intent(in) :: x
      integer(kind=int64) :: res
      res = l % m * x + l % b
   end function find_y

   function in_bounds(x, y) result(res)
      implicit none
      integer(kind=int64), intent(in) :: x, y
      logical                         :: res
      res = (x >= mincoord) .and. (x <= maxcoord) .and. (y >= mincoord) .and. (y <= maxcoord)
   end function in_bounds

   ! given points on lines a-b and c-d, find their interesection. find the
   ! y=m*x+b equation (slope intercept form) for each line and then work from
   ! those to find the x and y coordinates of their intersection. the data in
   ! this problem gives us lines at 45 degrees from the axis (slope |1|) so
   ! calculations can all be done with integers. i lifted the code from a more
   ! general solution on rosetta code that used floating point and changed the
   ! data types.
   !
   ! there is no real error checking here. the data we are given is clean. i do
   ! double (or triple) dispatch as needed to make sure that all the segments
   ! are given from lower x to higher x.

   recursive subroutine intersection(x, y, xa, ya, xb, yb, xc, yc, xd, yd)
      implicit none
      integer(kind=int64), intent(out) :: x, y
      integer(kind=int64), intent(in) :: xa, ya, xb, yb, xc, yc, xd, yd
      integer(kind=int64) :: mab, mcd ! slopes
      integer(kind=int64) :: bab, bcd ! intercepts

      if (xa > xb) then
         ! lean consistently
         call intersection(x, y, xb, yb, xa, ya, xc, yc, xd, yd)
      else if (xc > xd) then
         ! lean consistently
         call intersection(x, y, xa, ya, xb, yb, xd, yd, xc, yc)
      else

         ! find m and b for y = m*x + b form
         call slope_intercept(mab, bab, xa, ya, xb, yb)
         call slope_intercept(mcd, bcd, xc, yc, xd, yd)

         ! find intersection
         x = (bcd - bab) / (mab - mcd)
         y = ya - xa * mab + x * mab

      end if
   end subroutine intersection

   ! given two points, what is the slope and intecept of the line
   ! using y = mx+b form? integers, the slope in the problem set is
   ! |1|.

   subroutine slope_intercept(m, b, ax, ay, bx, by)
      implicit none
      integer(kind=int64), intent(out) :: m, b
      integer(kind=int64), intent(in)  :: ax, ay, bx, by

      m = (by - ay) / (bx - ax)
      b = ay - ax * m
   end subroutine slope_intercept

end program solution
