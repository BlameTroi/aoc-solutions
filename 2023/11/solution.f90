! solution.f90 -- 2022 day ?? -- -- troy brumley blametroi@gmail.com

! advent of code 2023 day 11 -- cosmic expansion
!
! for part 1 of day 11 is a sum of distances between "galaxies" in an expanding
! "universe." the expansion rules are relatively simple: each empty row or
! column becomes two. integer distances and "stair step" walks between galaxies
! should be kept in mind.
!
! for part 2 of day 11 ... after working through some of part 1 i halfway expect
! part 2 to just change that weight or cost of an empty row or column. so the
! cost to to cross a cell that is on a row and column both without a galaxy
! becomes 4. most obvious ways of solving part 1 would not be at all amenable to
! this idea.
!
! when i got to part 2, i discovered i was partially right. the weight to cross
! any void row or column increased so much that some possible solutions to part
! 1 would bog bog down, but not as bad as my idea.

program solution

   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input

   implicit none

   integer, parameter            :: AOCIN = 10
   integer(kind=int64)           :: part_one, part_two  ! results
   character(len=max_aoc_reclen) :: rec

   integer, parameter            :: MAXDIM = 140      ! observed, 10x10 in smaller
   integer, parameter            :: MAXGALAXIES = 500 ! actually 450
   integer, parameter            :: TAGLEN = 9        ! "[rrr,ccc]"
   character(len=*), parameter   :: TAGFMT = "('[',i3.3,',',i3.3,']')"
   type galaxy_t
      character(len=TAGLEN) :: tag
      integer               :: row, col
   end type galaxy_t

   character(len=1)              :: cosmos(1:MAXDIM, 1:MAXDIM)
   type(galaxy_t)                :: galaxies(1:MAXGALAXIES)
   integer                       :: num_galaxies
   integer(kind=int64)           :: gap_dist

   ! initialize
   rec = ""
   call open_aoc_input(AOCIN)
   part_one = 0; part_two = 0

   call init

   call load

   part_one = do_part_one()

   part_two = do_part_two()

   ! report and close
   print *
   print *, part_one, "part one"
   print *, part_two, "part two"
   print *

   call close_aoc_input(AOCIN)

contains

   ! the typical clear out the tables and set sensible empty values

   subroutine init
      implicit none
      integer :: i

      cosmos = " "
      do i = 1, MAXGALAXIES
         galaxies(i) % tag = ""
         galaxies(i) % row = -1
         galaxies(i) % col = -1
      end do
      num_galaxies = 0
      gap_dist = 1
   end subroutine init

   ! a handy handle, the coordinates. a debugging aid that doesn't really get used
   ! in the final runs.

   function make_tag(r, c) result(res)
      implicit none
      integer, intent(in)   :: r, c
      character(len=TAGLEN) :: res

      res = ""
      write (res, fmt=TAGFMT) r, c
   end function make_tag

   ! is a row or column devoid in the void? search said row or column for a galaxy.
   ! the size of the step across a row or column depends upon whether or not it
   ! is empty.

   function is_on_row(r) result(res)
      implicit none
      integer, intent(in) :: r
      logical             :: res
      integer             :: c

      res = .false.
      c = 0
      do while ((.not. res) .and. (c < MAXDIM))
         c = c + 1
         res = cosmos(r, c) == "#"
      end do
   end function is_on_row

   function is_on_col(c) result(res)
      implicit none
      integer, intent(in) :: c
      logical             :: res
      integer             :: r

      res = .false.
      r = 0
      do while ((.not. res) .and. (r < MAXDIM))
         r = r + 1
         res = cosmos(r, c) == "#"
      end do
   end function is_on_col

   ! return unsigned distance between rows or columns, accounting for the
   ! variable distance of the voids.

   recursive function col_dist(c1, c2) result(res)
      implicit none
      integer, intent(in) :: c1, c2
      integer(kind=int64) :: res
      integer             :: c

      res = 0

      if (c1 == c2) return

      if (c1 > c2) then
         res = -col_dist(c2, c1)
         return
      end if

      c = c1
      do while (c < c2)
         res = res + 1
         if (cosmos(1, c + 1) == "|" .or. cosmos(1, c + 1) == "+") then
            res = res + gap_dist
         end if
         c = c + 1
      end do
   end function col_dist

   recursive function row_dist(r1, r2) result(res)
      implicit none
      integer, intent(in) :: r1, r2
      integer(kind=int64) :: res
      integer             :: r

      res = 0

      if (r1 == r2) return

      if (r1 > r2) then
         res = -row_dist(r2, r1)
         return
      end if

      r = r1
      do while (r < r2)
         res = res + 1
         if (cosmos(r + 1, 1) == "|" .or. cosmos(r + 1, 1) == "+") then
            res = res + gap_dist
         end if
         r = r + 1
      end do
   end function row_dist

   ! read the primordial universe and then big bang it to determine the cost to
   ! move across a row or column.

   subroutine load
      implicit none
      integer :: i, j

      i = 0
      do while (read_aoc_input(AOCIN, rec))
         if (len_trim(rec) < 1) cycle
         i = i + 1
         do j = 1, (len_trim(rec))
            cosmos(i, j) = rec(j:j)
         end do
      end do

      ! update the grid so that empty rows and columns have their 'open space'
      ! glyph changed from '.' to one of '-', '|', or '+'. this is a minor
      ! optimization when walking from one galaxy to another. note the empty
      ! voids instead of rescanning.

      do i = 1, MAXDIM
         if (.not. is_on_row(i)) then
            do j = 1, MAXDIM
               if (cosmos(i, j) == ".") cosmos(i, j) = "-"
            end do
         end if
      end do

      do j = 1, MAXDIM
         if (.not. is_on_col(j)) then
            do i = 1, maxdim
               if (cosmos(i, j) == ".") then
                  cosmos(i, j) = "|"
               else if (cosmos(i, j) == "-") then
                  cosmos(i, j) = "+"
               end if
            end do
         end if
      end do

      ! how many galaxies are there? where are they?

      num_galaxies = 0
      do i = 1, MAXDIM
         do j = 1, MAXDIM
            if (cosmos(i, j) /= "#") cycle
            num_galaxies = num_galaxies + 1
            galaxies(num_galaxies) % tag = make_tag(i, j)
            galaxies(num_galaxies) % row = i
            galaxies(num_galaxies) % col = j
         end do
      end do

      ! sort the galaxies, i think they should already be in order, but a little
      ! paranoia goes a long way

      call sort_galaxies

   end subroutine load

   ! walk from one galaxy to another, counting steps.

   function walker(g1, g2) result(res)
      implicit none
      integer, intent(in) :: g1, g2          ! galaxies by subscript
      integer(kind=int64) :: res             ! distance allowing for variable size steps
      integer             :: r1, c1, r2, c2  ! actuall coordinates of g1 and g2
      integer(kind=int64) :: dr, dc          ! distances by row and col
      integer             :: adj             ! the amount to adjust a row or col by, it's +/- 1

      res = 0
      r1 = galaxies(g1) % row; c1 = galaxies(g1) % col
      r2 = galaxies(g2) % row; c2 = galaxies(g2) % col
      dr = row_dist(r1, r2); dc = col_dist(c1, c2)

      repeat: do

         ! strategy: reduce largest gap first

         if (abs(dr) > abs(dc)) then
            res = res + 1
            if (dr < 0) then
               adj = -1
            else
               adj = 1
            end if
            if (cosmos(r1 + adj, c1) == "-" .or. cosmos(r1 + adj, c1) == "+") then
               res = res + gap_dist
            end if
            r1 = r1 + adj
         end if

         ! if equal advance row

         if (abs(dr) == abs(dc)) then
            res = res + 1
            if (dr < 0) then
               adj = -1
            else
               adj = 1
            end if
            if (cosmos(r1 + adj, c1) == "-" .or. cosmos(r1 + adj, c1) == "+") then
               res = res + gap_dist
            end if
            r1 = r1 + adj
         end if

         ! and finally do smaller

         if (abs(dr) < abs(dc)) then
            res = res + 1
            if (dc < 0) then
               adj = -1
            else
               adj = 1
            end if
            if (cosmos(r1, c1 + adj) == "|" .or. cosmos(r1, c1 + adj) == "+") then
               res = res + gap_dist
            end if
            c1 = c1 + adj
         end if

         ! update the current grid distances and stop once we hit the end

         dr = row_dist(r1, r2); dc = col_dist(c1, c2)
         if (dr == 0 .and. dc == 0) exit repeat

      end do repeat

   end function walker

   ! compare two galaxies by their coorinates, returning a value in the
   ! neg-zero-pos convention.

   function compare_galaxies(i, j) result(res)
      implicit none
      integer, intent(in) :: i, j      ! subscripts
      integer             :: res       ! order of the galaxies if g1-g2 is negative, g1 first, etc.

      res = galaxies(i) % row - galaxies(j) % row
      if (res /= 0) return
      res = galaxies(i) % col - galaxies(j) % col
   end function compare_galaxies

   ! sort the galaxies table into ascending order of grid coordinates, row major
   subroutine sort_galaxies
      implicit none
      integer        :: i, j
      type(galaxy_t) :: s

      j = 0
      do while (j < num_galaxies - 1)
         j = j + 1
         do i = 1, num_galaxies - j
            if (compare_galaxies(i, i + 1) > 0) then
               s = galaxies(i)
               galaxies(i) = galaxies(i + 1)
               galaxies(i + 1) = s
            end if
         end do
      end do
   end subroutine sort_galaxies

   ! a simple sum the distances between all the points

   function do_part_one() result(res)
      implicit none
      integer(kind=int64) :: res
      integer             :: i, j

      res = 0
      gap_dist = 1
      do i = 1, num_galaxies - 1
         do j = i + 1, num_galaxies
            res = res + walker(i, j)
         end do
      end do
   end function do_part_one

   ! the algorithm is the same, but the size of the gap has grown

   function do_part_two() result(res)
      implicit none
      integer(kind=int64) :: res
      integer             :: i, j

      res = 0
      gap_dist = 1000000 - 1 ! you've got read the instructions closely in advent of code
      do i = 1, num_galaxies - 1
         do j = i + 1, num_galaxies
            res = res + walker(i, j)
         end do
      end do
   end function do_part_two

end program solution
