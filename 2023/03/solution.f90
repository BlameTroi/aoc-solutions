! solution.f90 -- 2023 day 03 -- -- troy brumley blametroi@gmail.com

! advent of code 2023 day 03 -- gear ratios.
!
! determine gear ratios from a jumbled schematic. for part one sum the part
! numbers found. for part two, identify gears (symbol "*" with qualifications)
! and then its gear ratio (adjacent part numbers multiplied).
!
! the input grid is square, so no need to differentiate between row and column
! in the limits. as is my wont, i've adding padding around the edges for
! simplicity in coding.
!
! on the grid digits and symbols are surrounded by empty space. a symbol is
! anything not a digit or empty. by examination the hard coded set includes all
! symbols.
!
! for part two gears "*" are important. any "*" that is adjacent to exactly two
! part numbers is a gear.
!
! not one, not three, two.
!
! by examination there are ~1250 numbers found on the grid without considering
! if they are part numbers, and the largest run of digits appears to be 3. i did
! a frequency count and there are about 370 "*" characters.
!
program solution

   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input

   implicit none

   integer, parameter            :: AOCIN = 10
   integer(kind=int64)           :: part_one, part_two  ! results
   character(len=max_aoc_reclen) :: rec

   ! our geometry
   integer, parameter :: GRID_MIN = 0
   integer, parameter :: GRID_BEG = 1
   integer, parameter :: GRID_END = 140
   integer, parameter :: GRID_MAX = 141

   ! our character set
   character(len=1), parameter :: DIGITS(10) = ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9"]
   character(len=1), parameter :: EMPTY = "."
   character(len=1), parameter :: SYMBOLS(10) = ["#", "$", "%", "&", "*", "+", "/", "-", "@", "="]
   character(len=1), parameter :: GEAR = "*"

   ! array sizes
   integer, parameter :: MAX_NUMBERS = 1500
   integer, parameter :: MAX_DIGITS = 4
   integer, parameter :: MAX_GEARS = 500

   ! the data
   character(len=1) :: grid(GRID_MIN:GRID_MAX, GRID_MIN:GRID_MAX)

   ! a number found on the grid. they will be horizontal
   type number_t
      integer                   :: row, col
      integer                   :: len
      character(len=MAX_DIGITS) :: str      ! as a string
      integer                   :: val
      logical                   :: adj      ! adjacent gears?
   end type number_t

   ! remember the numbers
   type(number_t) :: numbers(1:MAX_NUMBERS)

   ! a gear we've found
   type gear_t
      integer :: row, col
      logical :: adj
      integer :: pix(1:8)
   end type gear_t

   ! remember the gears
   type(gear_t) :: gears(1:MAX_GEARS)

   ! initialize

   rec = ""
   call open_aoc_input(AOCIN)
   part_one = 0; part_two = 0
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

   ! predicates

   ! is this a symbol

   function is_symbol(c) result(res)
      implicit none
      logical :: res
      character(len=1), intent(in) :: c
      integer :: i
      res = .true.
      do i = 1, size(SYMBOLS)
         if (c == SYMBOLS(i)) return
      end do
      res = .false.
   end function is_symbol

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

   ! are any of the adjacent grid cells holding a symbol?

   function adjacency(r, c) result(res)
      implicit none
      logical :: res
      integer, intent(in) :: r, c
      ! pardon the + 0, i want this lined up a bit
      res = is_symbol(grid(r - 1, c - 1)) .or. is_symbol(grid(r - 1, c + 0)) .or. is_symbol(grid(r - 1, c + 1)) .or. &
            is_symbol(grid(r + 0, c - 1)) .or. is_symbol(grid(r + 0, c + 1)) .or. &
            is_symbol(grid(r + 1, c - 1)) .or. is_symbol(grid(r + 1, c + 0)) .or. is_symbol(grid(r + 1, c + 1))
   end function adjacency

   ! load and populate the grid from the input.

   subroutine load
      implicit none
      integer :: i, j
      grid = EMPTY
      i = 0
      do while (read_aoc_input(AOCIN, rec))
         if (len_trim(rec) < 1) cycle
         i = i + 1
         do j = GRID_BEG, GRID_END
            if (j <= len_trim(rec)) grid(i, j) = rec(j:j)
         end do
      end do
   end subroutine load

   subroutine init
      implicit none
      integer :: i, j

      ! initialize numbers array

      do i = 1, size(numbers)
         numbers(i) % row = GRID_MIN; numbers(i) % col = GRID_MIN
         numbers(i) % len = 0
         do j = 1, MAX_DIGITS
            numbers(i) % str(j:j) = EMPTY
         end do
         numbers(i) % val = 0
         numbers(i) % adj = .false.
      end do

      ! initialize the gears array

      do i = 1, size(gears)
         gears(i) % row = GRID_MIN; gears(i) % col = GRID_MIN
         gears(i) % adj = .false.
         gears(i) % pix = 0
      end do

   end subroutine init

   ! for part one examine each number found on the grid, determine if
   ! it is adjacent to a symbol, and if so add its value to the answer.
   !
   ! the information on the numbers, their location, and adjacency, are
   ! persisted in an array for possible use in part two.

   function dopartone() result(res)
      implicit none
      integer :: res
      integer :: i, j, r, c

      res = 0

      ! scan the grid, find digit runs, and note them in the numbers array.
      ! as a digit is identified, check its adjacent cells for symbols.

      i = 0
      do r = GRID_BEG, GRID_END
         ! note, rows beg-end, cols min-max
         c = GRID_MIN
         inner: do while (c < GRID_MAX)
            c = c + 1
            if (.not. is_digit(grid(r, c))) cycle inner
            i = i + 1
            numbers(i) % row = r; numbers(i) % col = c
            numbers(i) % len = 0
            numbers(i) % adj = .false.
            digit: do while (is_digit(grid(r, c)))
               numbers(i) % len = numbers(i) % len + 1
               numbers(i) % str(numbers(i) % len:numbers(i) % len) = grid(r, c)
               numbers(i) % adj = numbers(i) % adj .or. adjacency(r, c)
               c = c + 1
            end do digit
            numbers(i) % val = 0
            val: do j = 1, numbers(i) % len
               numbers(i) % val = numbers(i) % val * 10 + char_int(numbers(i) % str(j:j))
            end do val
         end do inner
      end do

      ! sum the values of part numbers adjacent to parts
      do j = 1, i
         if (numbers(j) % adj) res = res + numbers(j) % val
      end do
   end function dopartone
end program solution
!
!
!{ are any of the adjacent grid cells holding a part number? count
!  the grid cells. }
!function adjparts(g : tGrid;
!                  r : integer;
!                  c : integer) : integer;
!begin
!   adjparts = 0;
!   if g(r-1, c-1) in DIGITS then
!      adjparts = adjparts + 1;
!   if g(r-1,   c) in DIGITS then
!      adjparts = adjparts + 1;
!   if g(r-1, c+1) in DIGITS then
!      adjparts = adjparts + 1;
!   if g(  r, c-1) in DIGITS then
!      adjparts = adjparts + 1;
!   if g(  r, c+1) in DIGITS then
!      adjparts = adjparts + 1;
!   if g(r+1, c-1) in DIGITS then
!      adjparts = adjparts + 1;
!   if g(r+1,   c) in DIGITS then
!      adjparts = adjparts + 1;
!   if g(r+1, c+1) in DIGITS then
!      adjparts = adjparts + 1;
!end;
!
!
!function partingrid(    g  : tGrid;
!                        r  : integer;
!                        c  : integer;
!                        n  : tNumbers;
!                    var ix : integer  { index of part no }
!                        ) : boolean;
!var
!   i : integer;
!
!begin
!   if not (g(r,c) in DIGITS) then begin
!      ix = 0;
!      partingrid = false;
!      exit
!   end;
!   for i = low(n) to high(n) do
!      with n(i) do begin
!         if row <> r then
!            cycle;
!         if c < col then
!            cycle;
!         if c > col+len then
!            cycle;
!         partingrid = true;
!         ix = i;
!         break;
!      end;
!end;
!
!
!{ for part two all gears must be found. a gear is a '*' symbol
!  adjacent to exactly two parts. sum the part numbers of those
!  parts. }
!
!function parttwo(var g  : tGrid;
!                 var n  : tNumbers;
!                 var x  : tGears) : integer;
!var
!   i, j, k, r, c, pn1, pn2, pn3 : integer;
!
!begin
!   parttwo = 0;
!
!   { scan the grid, find potential gears. a gear is a '*' grid
!     cell adjacent to exactly two part numbers. first check is
!     just for any possible numbers. a second pass will be made
!     to finalize the choices. }
!   i = 0;
!   for r = GRIDBEG to GRIDEND do begin
!      c = GRIDMIN; { not gridbeg }
!      while c < GRIDMAX do begin
!         c = c + 1;
!         if g(r, c) <> GEAR then
!            cycle;
!         i = i + 1; { log the number }
!         with x(i) do begin
!            row = r;
!            col = c;
!            adj = adjparts(g, r, c);
!         end;
!      end; { while c }
!   end; { for }
!
!   for j = 1 to i do
!      with x(j) do begin
!         if adj > 1 then begin
!            partingrid(g, row-1, col-1, n, pix(1));
!            partingrid(g, row-1, col  , n, pix(2));
!            partingrid(g, row-1, col+1, n, pix(3));
!            partingrid(g, row  , col-1, n, pix(4));
!            partingrid(g, row  , col+1, n, pix(5));
!            partingrid(g, row+1, col-1, n, pix(6));
!            partingrid(g, row+1, col  , n, pix(7));
!            partingrid(g, row+1, col+1, n, pix(8));
!            pn1 = 0; pn2 = 0; pn3 = 0;
!            for k = 1 to 8 do
!               if pix(k) <> 0 then begin
!                  pn1 = pix(k);
!                  break;
!               end;
!            for k = 1 to 8 do
!               if (pix(k) <> 0) and (pix(k) <> pn1) then begin
!                  pn2 = pix(k);
!                  break;
!               end;
!            if pn2 == 0 then
!               continue;
!            for k = 1 to 8 do
!               if (pix(k) <> 0) and (pix(k) <> pn1) and (pix(k) <> pn2) then begin
!                  pn3 = pix(k);
!                  break;
!               end;
!            if pn3 <> 0 then begin
!               { more than two parts found }
!               continue;
!            end;
!            pn1 = n(pn1).val;
!            pn2 = n(pn2).val;
!            parttwo = parttwo + (pn1 * pn2);
!         end
!      end;
!end;
