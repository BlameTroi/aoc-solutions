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

   ! our character set, digits and ...
   character(len=1), parameter :: EMPTY = "."
   character(len=*), parameter :: SYMBOLS = "#$%&*+/-@="
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
      integer :: adj
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
   part_two = doparttwo()

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
      do i = 1, len(SYMBOLS)
         if (c == SYMBOLS(i:i)) return
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
         gears(i) % adj = 0
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
      integer(kind=int64) :: res
      integer             :: i, j, r, c

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

   ! are any of the adjacent grid cells holding a part number? count the grid
   ! cells that could.

   function adjparts(r, c) result(res)
      implicit none
      integer, intent(in) :: r, c
      integer             :: res
      res = 0
      if (is_digit(grid(r - 1, c - 1))) res = res + 1
      if (is_digit(grid(r - 1, c - 0))) res = res + 1
      if (is_digit(grid(r - 1, c + 1))) res = res + 1
      if (is_digit(grid(r - 0, c - 1))) res = res + 1
      !                     r, c doesn't count obviously
      !                          if i did check, it'd be false
      if (is_digit(grid(r - 0, c + 1))) res = res + 1
      if (is_digit(grid(r + 1, c - 1))) res = res + 1
      if (is_digit(grid(r + 1, c - 0))) res = res + 1
      if (is_digit(grid(r + 1, c + 1))) res = res + 1
   end function adjparts

   ! we've been given a grid cell and want to know if it is part of a part
   ! number. implementated as a function but i turned out to not use the
   ! boolean result.

   function partingrid(r, c, ix) result(res)
      implicit none
      integer, intent(in)    :: r, c
      integer, intent(inout) :: ix    ! index of part number in numbers()
      logical                :: res
      integer                :: i

      ! assume this grid cell is not in a part number
      ix = 0
      res = .false.

      ! if we're not pointing at a digit, it's not a number
      if (.not. is_digit(grid(r, c))) return

      ! is there a part number with digits that span this row, column?
      ! no assumptions about the order of numbers and their location
      ! in the grid are made. scan the whole list of numbers.
      do i = 1, MAX_NUMBERS
         if (.not. numbers(i) % adj) cycle
         if (numbers(i) % row /= r) cycle
         if (c < numbers(i) % col) cycle
         if (c > numbers(i) % col + numbers(i) % len) cycle
         ! ok, this location is inside a part number
         ix = i
         res = .true.
         return
      end do

      ! none found, failing result already set during init
   end function partingrid

   ! for part two all gears must be found. a gear is a '*' symbol adjacent to
   ! exactly two parts. sum the part numbers of those parts.

   function doparttwo() result(res)
      implicit none
      integer(kind=int64) :: res
      integer             :: i, j, k, r, c, pn1, pn2, pn3
      logical             :: b ! throw away a function return i'm not using

      res = 0

      ! scan the grid, find potential gears. a gear is a '*' grid cell adjacent
      ! to exactly two part numbers. first check is just for any possible
      ! numbers. a second pass will be made to finalize the choices.

      i = 0
      do r = GRID_BEG, GRID_END
         c = GRID_MIN ! again, not grid_beg
         do while (c < GRID_MAX)
            c = c + 1
            if (grid(r, c) /= GEAR) cycle
            ! found a potential gear, remember it and the count of surrounding
            ! cells that could hold part numbers.
            i = i + 1
            gears(i) % row = r; gears(i) % col = c
            gears(i) % adj = adjparts(r, c)
         end do
      end do

      ! now, for each potential gear found, if there are at least two possible
      ! part numbers, check all the surrounding cells to find out if these are
      ! really part numbers, and if so how many there are

      do j = 1, i
         if (gears(j) % adj < 2) cycle

         ! check each adjacent cell and if it holds a number, get the index of
         ! that part number in the numbers list. there can be adjacent cells,
         ! say upper left and upper, that are in the same single part number.
         ! after we get the indices, we need two and only two unique part
         ! numbers to identify this as an actual gear.

         r = gears(j) % row; c = gears(j) % col
         b = partingrid(r - 1, c - 1, gears(j) % pix(1)) ! the function result is not used
         b = partingrid(r - 1, c - 0, gears(j) % pix(2)) ! just the index into part numbers
         b = partingrid(r - 1, c + 1, gears(j) % pix(3)) ! that is an inout in the function
         b = partingrid(r - 0, c - 1, gears(j) % pix(4)) ! arguments
         ! and of course, r,c isn't checked
         b = partingrid(r - 0, c + 1, gears(j) % pix(5))
         b = partingrid(r + 1, c - 1, gears(j) % pix(6))
         b = partingrid(r + 1, c - 0, gears(j) % pix(7))
         b = partingrid(r + 1, c + 1, gears(j) % pix(8))

         ! now how many unique part numbers are there? exactly two are needed
         ! for this to be an actual gear.

         pn1 = 0; pn2 = 0; pn3 = 0 ! none found yet
         ! find first possible part number
         do k = 1, 8
            if (gears(j) % pix(k) /= 0) then
               pn1 = gears(j) % pix(k)
               exit
            end if
         end do
         if (pn1 == 0) cycle
         ! find second possible part number
         do k = 1, 8
            if (gears(j) % pix(k) /= 0 .and. gears(j) % pix(k) /= pn1) then
               pn2 = gears(j) % pix(k)
               exit
            end if
         end do
         ! if we didn't find two, try again
         if (pn2 == 0) cycle
         ! got two, but is there a third?
         do k = 1, 8
            if (gears(j) % pix(k) /= 0 .and. gears(j) % pix(k) /= pn1 .and. gears(j) % pix(k) /= pn2) then
               pn3 = gears(j) % pix(k)
               exit
            end if
         end do
         ! a third is a deal breaker.
         if (pn3 /= 0) cycle

         ! add the product of the part numbers to the result for part two
         pn1 = numbers(pn1) % val
         pn2 = numbers(pn2) % val
         res = res + (pn1 * pn2)
      end do
   end function doparttwo

end program solution
