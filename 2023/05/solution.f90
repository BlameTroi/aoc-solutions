! solution.f90 -- 2023 day 5 -- getting seedy -- troy brumley blametroi@gmail.com

! advent of code 2023 day 5 -- getting seedy
!
! the goal is to find a location through the maps. the original value is
! preserved then passed through each map as source, with the result reported in
! 'dest'. then cycle dest as the new source and keep going until the last map is
! processed.
!
! for part one, walk the maps and report the lowest location where a seed should be
! planted.
!
! for part two, we change the problem by decreeing the input for seeds is now a pair.
! instead of "seed seed seed seed" we get "seed count seed count" and have to walk
! all the seeds thorugh the maps. Determine the location that corresponds to any of
! the starting seed numbers.
!
! i'd originally split this into two separate programs but i'm going to put them
! back together in this conversion to fortran. i may also work on finding a better
! approach that the brute force method. it runs, albeit slowly, in pascal on the
! old windows laptop. it should be faster on the mac m2 but it's still a slow
! approach for part two.
!
! i know that i could check boundary conditions or even binary search the part
! two lookups to avoid needdless checks, but i haven't taken the time to figure
! out. the rules for exhausting a lookup and bouncing through seven maps are
! left for another time. instead of looking at the seeds individually, they need
! to be considered as a block.
!
! but for now, brute force delivers the right answer in a feasible amount of
! time. i can do the dishes while this runs.

program solution

   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input
   use aochelpers, only: is_digit, char_int, int64_str, str_int64, is_letter

   implicit none

   integer, parameter            :: AOCIN = 10
   integer(kind=int64)           :: part_one, part_two  ! results
   character(len=max_aoc_reclen) :: rec

   integer, parameter :: MAXSEEDS = 50        ! 10 in test
   integer, parameter :: MAXMAPS = 10         ! 10 in test
   integer, parameter :: MAXRANGES = 100      ! 10 in test

   character(len=*), parameter :: cseeds = "seeds:"   ! input markers
   character(len=*), parameter :: cmap = "map:"
   integer, parameter          :: NULLVAL = -1        ! dead cell marker

   ! walk seed through a map

   type seed_location_t
      integer(kind=int64) :: seed        ! as originally entered
      integer(kind=int64) :: dest        ! result from map
      integer(kind=int64) :: src         ! passed through map
   end type seed_location_t

   ! for part two, the input becomes seed/count pairs

   type seed_count_t
      integer(kind=int64) :: seed
      integer(kind=int64) :: count
   end type seed_count_t

   ! range translation, source to destination

   type range_t
      integer(kind=int64) :: begdest     ! first slot in dest
      integer(kind=int64) :: begsrc      ! first slot in source
      integer(kind=int64) :: leng        ! length of each
   end type range_t

   ! the mape or index or lookup, pick your word. find the range that holds a
   ! number in source and its equivalent in destination. that number is the
   ! input to the next map.

   type map_t
      character(len=64) :: tag
      type(range_t)     :: xrange(1:MAXRANGES)
      integer           :: numranges
   end type map_t

   type(seed_location_t) :: seedloc(1:MAXSEEDS)
   integer               :: numseeds1
   type(seed_count_t)    :: seedcnt(1:MAXSEEDS / 2)
   integer               :: numseeds2

   type(map_t)           :: xmap(1:MAXMAPS)
   integer               :: nummaps

   ! initialize

   rec = ""
   call open_aoc_input(AOCIN)
   part_one = 0; part_two = 0

   call init

   ! load using part one structures
   call loadseeds
   call loadmaps

   part_one = dopartone()

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
      integer :: i, j

      do i = 1, MAXSEEDS
         seedloc(i) % seed = NULLVAL
         seedloc(i) % dest = NULLVAL
         seedloc(i) % src = NULLVAL
      end do
      numseeds1 = 0

      do i = 1, MAXSEEDS / 2
         seedcnt(i) % seed = NULLVAL
         seedcnt(i) % count = NULLVAL
      end do
      numseeds2 = 0

      do i = 1, MAXMAPS
         xmap(i) % tag = "NULL"
         do j = 1, MAXRANGES
            associate (xmr => xmap(i) % xrange(j))
               xmr % begdest = NULLVAL
               xmr % begsrc = NULLVAL
               xmr % leng = NULLVAL
            end associate
         end do
         xmap(i) % numranges = 0
      end do
      nummaps = 0

   end subroutine init

   ! read the seed numbers. they are all one one input record, preceded by
   ! 'seed:', space delimited integers.
   !
   ! for part one, each integer is a seed.
   !
   ! for part two, pairs of integers denote a seed and count.

   subroutine loadseeds
      implicit none
      integer :: p
      logical :: b

      b = read_aoc_input(AOCIN, rec) ! don't need eof status here
      if (.not. rec(1:len(CSEEDS)) == CSEEDS) then
         print *, "*** error *** missing seeds marker"
         stop 1
      end if

      ! parse as part one seed numbers

      p = len(CSEEDS) + 1 ! space after marker
      numseeds1 = 0
      do while (p < len_trim(rec))
         numseeds1 = numseeds1 + 1
         if (rec(p:p) == " ") p = p + 1
         associate (slc => seedloc(numseeds1))
            slc % seed = str_int64(rec, p) ! p will be on first non digit
            slc % dest = NULLVAL
            slc % src = NULLVAL
         end associate
      end do

      ! parse as part two seed/count pairs

      p = len(CSEEDS) + 1 ! space after marker
      numseeds2 = 0
      do while (p < len_trim(rec))
         numseeds2 = numseeds2 + 1
         if (rec(p:p) == " ") p = p + 1
         seedcnt(numseeds2) % seed = str_int64(rec, p) ! p will be on first non digit
         if (rec(p:p) == " ") p = p + 1
         seedcnt(numseeds2) % count = str_int64(rec, p)
      end do
   end subroutine loadseeds

   ! each 'map' consists of one or more ranges. maps are identified by a tag
   ! record holding the name of the map (something-to-something) followed by the
   ! word 'map:'. there are seven maps and they happen to be in the correct
   ! order as read so no sort or link pointers are needed.
   !
   ! each range is a single input record holding three numbers. the destination
   ! index value start, source (or input) index value start, and the length of
   ! the range. a map can hold multiple ranges. a blank blank record end of file
   ! marks the end of the current map's ranges.

   subroutine loadmaps
      implicit none
      integer :: p, i
      logical :: b
      b = read_aoc_input(AOCIN, rec)
      if (len_trim(rec) /= 0) then
         print *, "*** error *** input sequence error in load maps"
         stop 1
      end if
      nummaps = 0
      do while (read_aoc_input(AOCIN, rec))
         if (.not. is_letter(rec(1:1))) then
            print *, "*** error *** unexpect input in load maps: '", trim(rec), "'"
            stop 1
         end if
         nummaps = nummaps + 1
         xmap(nummaps) % tag = trim(rec)
         i = 0
         do while (read_aoc_input(AOCIN, rec))
            if (len_trim(rec) < 1) exit
            p = 1
            i = i + 1
            associate (xmr => xmap(nummaps) % xrange(i))
               xmr % begdest = str_int64(rec, p)
               p = p + 1
               xmr % begsrc = str_int64(rec, p)
               p = p + 1
               xmr % leng = str_int64(rec, p)
            end associate
         end do
         xmap(nummaps) % numranges = i
      end do
   end subroutine loadmaps

   ! it's an index!

   function lookup(n, m) result(res)
      implicit none
      integer(kind=int64), intent(in)     :: n
      type(map_t), intent(in) :: m
      integer(kind=int64)     :: res
      integer                 :: i

      res = n
      do i = 1, m % numranges
         associate (xmr => m % xrange(i))
            if (xmr % begsrc <= n .and. n <= xmr % begsrc + xmr % leng) then
               res = xmr % begdest + (n - xmr % begsrc)
               return
            end if
         end associate
      end do

   end function lookup

! walk each seed through the maps using the lookup defined, then
! find the lowest final seed location and report it.

   function dopartone() result(res)
      implicit none
      integer(kind=int64) :: res
      integer :: i, lowest

      do i = 1, numseeds1
         seedloc(i) % src = seedloc(i) % seed
         seedloc(i) % dest = lookup(seedloc(i) % src, xmap(1))
         seedloc(i) % dest = lookup(seedloc(i) % dest, xmap(2))
         seedloc(i) % dest = lookup(seedloc(i) % dest, xmap(3))
         seedloc(i) % dest = lookup(seedloc(i) % dest, xmap(4))
         seedloc(i) % dest = lookup(seedloc(i) % dest, xmap(5))
         seedloc(i) % dest = lookup(seedloc(i) % dest, xmap(6))
         seedloc(i) % dest = lookup(seedloc(i) % dest, xmap(7))
      end do
      lowest = 1
      do i = 1, numseeds1
         if (seedloc(i) % dest < seedloc(lowest) % dest) lowest = i
      end do
      res = seedloc(lowest) % dest
   end function dopartone

   ! crunch very slowly for part two of this problem.

   function doparttwo() result(res)
      implicit none
      integer(kind=int64) :: res
      integer             :: i
      integer(kind=int64) :: j, k
      integer(kind=int64) :: lowest
      res = -1
      lowest = huge(res)
      do i = 1, numseeds2
         associate (sc => seedcnt(i)) ! seed, count
            do j = sc % seed, sc % seed + sc % count
               k = lookup(j, xmap(1))
               k = lookup(k, xmap(2))
               k = lookup(k, xmap(3))
               k = lookup(k, xmap(4))
               k = lookup(k, xmap(5))
               k = lookup(k, xmap(6))
               k = lookup(k, xmap(7))
               lowest = min(lowest, k)
            end do
         end associate
      end do
      res = lowest
   end function doparttwo

end program solution
