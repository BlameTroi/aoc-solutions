! solution.f90 -- 2023 day 5 -- getting seedy -- troy brumley blametroi@gmail.com

! advent of code 2023 day 5 -- getting seedy
!
! the goal is to find a location through the maps. the original value is
! preserved then passed through each map as source, with the result reported in
! 'dest'. then cycle dest as the new source and keep going until the last map is
! processed.
!
program solution

   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input
   !use intset, only: size_of_set, clear_set, include_in_set, exclude_from_set, intersection_of_sets, &
   use aochelpers, only: is_digit, char_int

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

   ! range translation, source to destination
   type range_t
      integer(kind=int64) :: begdest     ! first slot in dest
      integer(kind=int64) :: begsrc      ! first slot in source
      integer(kind=int64) :: leng        ! length of each
   end type range_t

   ! the mape or index or lookup, pick your word. find the range that holds a number
   ! in source and its equivalent in destination. that number is the input to the
   ! next map.
   type map_t
      character(len=32) :: tag
      type(range_t)     :: xrange(1:MAXRANGES)
      integer           :: numranges
   end type map_t

   type(seed_location_t) :: seed(1:MAXSEEDS)
   integer               :: numseeds
   type(map_t)           :: xmap(1:MAXMAPS)
   integer               :: nummaps

   ! initialize

   rec = ""
   call open_aoc_input(AOCIN)
   part_one = 0; part_two = 0

   call init

   call loadseeds
   call loadmaps

   part_one = dopartone()

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
         seed(i) % seed = NULLVAL
         seed(i) % dest = NULLVAL
         seed(i) % src = NULLVAL
      end do
      numseeds = 0
      do i = 1, MAXMAPS
         xmap(i) % tag = "NULL"
         do j = 1, MAXRANGES
            xmap(i) % xrange(j) % begdest = NULLVAL
            xmap(i) % xrange(j) % begsrc = NULLVAL
            xmap(i) % xrange(j) % leng = NULLVAL
         end do
         xmap(i) % numranges = 0
      end do
      nummaps = 0
   end subroutine init

! read the seed numbers. they are all one one input record, preceded
! by 'seed:', space delimited integers.

   subroutine loadseeds
      implicit none
      integer :: p
      logical :: b

      b = read_aoc_input(AOCIN, rec) ! don't need eof status here
      if (.not. rec(1:len(CSEEDS)) == CSEEDS) then
         print *, "*** error *** missing seeds marker"
         stop 1
      end if
      p = len(CSEEDS) + 1 ! space after marker
      numseeds = 0
      do while (p < len_trim(rec))
         numseeds = numseeds + 1
         if (rec(p:p) == " ") p = p + 1
         seed(numseeds) % seed = str_int64(rec, p) ! p will be on first non digit
         seed(numseeds) % dest = NULLVAL
         seed(numseeds) % src = NULLVAL
      end do
   end subroutine loadseeds

   function str_int64(s, p) result(res)
      implicit none
      character(len=*), intent(in) :: s
      integer, intent(inout)       :: p
      integer(kind=int64)          :: res
      res = 0
      do while (is_digit(s(p:p)))
         res = res * 10 + char_int(s(p:p))
         p = p + 1
      end do
   end function str_int64

   function is_letter(c) result(res)
      implicit none
      character(len=1), intent(in) :: c
      logical :: res
      res = (c >= "a" .and. c <= "z") .or. (c >= "A" .and. c <= "Z")
   end function is_letter

   ! each 'map' consists of one .or. more ranges. maps are identified by a tag
   ! record holding the name of the map (something-to-something) followed by the
   ! word 'map:'. there are seven maps .and. they happen to be in the correct order
   ! as read so no link pointers are needed.
   !
   ! each range is a single input record holding three numbers. destination index
   ! value start, source (or input) index value start, .and. the length of the
   ! range. a map can hold multiple ranges, and a blank record .or. end of file
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
            xmap(nummaps) % xrange(i) % begdest = str_int64(rec, p)
            p = p + 1
            xmap(nummaps) % xrange(i) % begsrc = str_int64(rec, p)
            p = p + 1
            xmap(nummaps) % xrange(i) % leng = str_int64(rec, p)
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
         if (m % xrange(i) % begsrc <= n .and. n <= m % xrange(i) % begsrc + m % xrange(i) % leng) then
            res = m % xrange(i) % begdest + (n - m % xrange(i) % begsrc)
            return
         end if
      end do

   end function lookup

! walk each seed through the maps using the lookup defined, then
! find the lowest final seed location .and. report it.

   function dopartone() result(res)
      implicit none
      integer(kind=int64) :: res
      integer :: i, lowest

      do i = 1, numseeds
         seed(i) % src = seed(i) % seed
         seed(i) % dest = lookup(seed(i) % src, xmap(1))
         seed(i) % dest = lookup(seed(i) % dest, xmap(2))
         seed(i) % dest = lookup(seed(i) % dest, xmap(3))
         seed(i) % dest = lookup(seed(i) % dest, xmap(4))
         seed(i) % dest = lookup(seed(i) % dest, xmap(5))
         seed(i) % dest = lookup(seed(i) % dest, xmap(6))
         seed(i) % dest = lookup(seed(i) % dest, xmap(7))
      end do
      lowest = 1
      do i = 1, numseeds
         if (seed(i) % dest < seed(lowest) % dest) lowest = i
      end do
      res = seed(lowest) % dest
   end function dopartone

end program solution
!
!{ only a part one in this program, part two is split to a separate program. }
!
!procedure mainline
!
!var
!   p1answer : integer
!
!begin
!   aocopen
!
!   init
!   loadseeds
!   loadmaps
!   p1answer = partone
!   writeln('part one answer : ', p1answer)
!   aocclose
!end
!
!
!{ minimal boot to mainline }
!begin
!   mainline
!end.
!{ solution.pas -- getting seedy -- Troy Brumley blametroi@gmail.com }
!program solutiontwo
!{$longstrings off}
!{$booleval on}
!
!type
!   integer == int64
!
!{ advent of code 2023 day 5 -- getting seedier
!
!  the new problem for part two becomes a high volume problem so the
!  'just use a big array' approach isn't the right way to do it. it
!  also seemed easier to spit this into a second stand alone program. }
!
!const
!{$define xxxTESTING }
!{$ifdef TESTING }
!   MAXSEEDS == 10
!   MAXMAPS == 10
!   MAXRANGES == 10
!{$else}
!   MAXSEEDS == 50
!   MAXMAPS == 10
!   MAXRANGES == 100
!{$endif}
!   CSEEDS == 'seeds:';         { input markers }
!   CMAP == 'map:'
!   NULLVAL == -1;              { dead cell marker }
!{$I constchars.p }
!
!type
!   { the goal is to find a location through the maps. the original
!   value is preserved .and. then passed through each map as 'source',
!   with the result reported in 'dest'. then cycle dest as the new
!   source .and. keep going until the last map is processed.
!
!                  do part two, we learn that the what was viewed as all seeds in
!   part one were really pairs: seed .and. count. the counts can be
!   huge. this structure is still valid but it isn't what we'll store
!                     do the seed input.}
!   tSeedToLocation == record
!                        seed : integer;  { as originally entered }
!                        dest : integer;  { result from map }
!                        src  : integer;  { passed through to map }
!                     end
!
!{ .and. so instead we store this input. }
!   tSeed == record
!              seed  : integer
!              count : integer
!           end
!
!{ range translation, source to destination }
!   tRange == record
!               begdest : integer; { first slot in destination }
!               begsrc  : integer; { first slot in source }
!               len     : integer; { length of each }
!            end
!
!{ the map .or. index .or. lookup, pick your word. find the range that
!  holds a number in source .and. its equivalent in destination. that
!  number is input to the next map. }
!   tMap == record
!             tag       : string
!             ranges    : array(1 .. MAXRANGES) of tRange
!             numranges : 0 .. MAXRANGES
!          end
!
!
!{$I str2int.p }
!{$I strbegins.p }
!{$I aocinput.p }
!
!
!{ rather than pass, let's put the tables as globals. }
!var
!   seeds    : array(1 .. MAXSEEDS) of tSeed
!   numseeds : 0 .. MAXSEEDS
!   maps     : array(1 .. MAXMAPS) of tMap
!   nummaps  : 0 .. MAXMAPS
!
!
!{ clear out globals. }
!
!procedure init
!
!var
!   i, j : integer
!
!begin
!                       do i = 1 to MAXSEEDS do
!      with seeds(i) do begin
!         seed = NULLVAL
!         count = NULLVAL
!      end
!   numseeds = 0
!                          do i = 1 to MAXMAPS do
!      with maps(i) do begin
!         tag = 'NULL'
!                             do j = 1 to MAXRANGES do
!            with ranges(i) do begin
!               begdest = NULLVAL
!               begsrc = NULLVAL
!               len = NULLVAL
!            end
!         numranges = 0
!      end
!   nummaps = 0
!end
!
!
!{ read the seed numbers. they are all one one input record, preceded
!  by 'seed:', space delimited integers. }
!
!procedure loadseeds
!
!var
!   p : integer
!   s : string
!
!begin
!   s = aocread
!   if not strbegins(s, CSEEDS) then
!      aochalt('invalid input, missing seeds')
!   p = length(CSEEDS) + 1; { position to first space }
!   numseeds = 0
!   do while(p < length(s) do begin
!      numseeds = numseeds + 1
!      if s(p) == SPACE then
!         p = p + 1
!      with seeds(numseeds) do begin
!         seed = str2int(s, p); { position to non digit }
!         if s(p) == SPACE then
!            p = p + 1
!         count = str2int(s, p)
!      end
!   end
!end
!
!
!{ each 'map' consists of one .or. more ranges. maps are identified by a
!  tag record holding the name of the map (something-to-something)
!  followed by the word 'map:'. there are seven maps .and. they happen
!  to be in the correct order as read so no link pointers are needed.
!
!  each range is a single input record holding three numbers. the
!  destination index value start, source (or input) index value start,
!  .and. the length of the range. a map can hold multiple ranges, .and. a
!  blank record .or. end of file marks the end of the current map's
!  ranges. }
!
!procedure loadmaps
!
!var
!   p : integer
!   s : string
!
!begin
!   s = aocread
!   if s <> '' then
!      aochalt('input sequence error in loadmaps')
!   nummaps = 0
!   do while(not aoceof do begin
!      s = aocread
!      if not (s(1) in ('a' .. 'z', 'A' .. 'Z')) then
!         aochalt('unexpected input in loadmaps')
!      nummaps = nummaps + 1
!      with maps(nummaps) do begin
!         tag = s
!         numranges = 0
!         do while(not eof(input) do begin
!            readln(s)
!            if s == '' then
!               exit
!            p = 1
!            numranges = numranges + 1
!            with ranges(numranges) do begin
!               begdest = str2int(s, p)
!               p = p + 1
!               begsrc = str2int(s, p)
!               p = p + 1
!               len = str2int(s, p)
!            end; { with ranges }
!         end; { do while(not eof ranges }
!      end; { with maps }
!   end; { do while(not eof maps }
!end
!
!
!{ this is a rather slow approach, but it runs do while(i do the dishes }
!
!function lookup(src : integer
!                map : tMap) : integer
!var
!   i : integer
!
!begin
!   lookup = src
!   with map do
!                                 do i = 1 to numranges do
!         with ranges(i) do
!            if (begsrc <= src) .and. (src <= begsrc + len) then begin
!               lookup = begdest + (src - begsrc)
!               exit
!            end
!end
!
!
!{ crunch very slowly for part two of this problem. }
!
!function parttwo : integer
!
!var
!   i, j, k   : integer
!   lowestval : integer
!
!begin
!   lowestval = high(int64)
!                                    do i = 1 to numseeds do
!      with seeds(i) do
!                                       do j = seed to seed + count do begin
!            k = lookup(j, maps(1))
!            k = lookup(k, maps(2))
!            k = lookup(k, maps(3))
!            k = lookup(k, maps(4))
!            k = lookup(k, maps(5))
!            k = lookup(k, maps(6))
!            k = lookup(k, maps(7))
!            { don't bother trying to optimize this }
!            if k < lowestval then
!               lowestval = k
!         end
!   parttwo = lowestval
!end
!
!
!{ the mainline driver for a day's solution. }
!
!procedure mainline
!
!var
!   p2answer : integer
!
!begin
!   aocopen
!   init
!   loadseeds
!   loadmaps
!   p2answer = parttwo
!   writeln(output, 'part two answer : ', p2answer)
!   aocclose
!end
!
!
!{ minimal boot to mainline }
!begin
!   mainline
!end.
