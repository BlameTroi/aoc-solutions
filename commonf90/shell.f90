
! solution.f90 -- 2022 day 04 -- -- Troy Brumley blametroi@gmail.com

program solution

   use iso_fortran_env
   use aocinput

   implicit none

   integer, parameter  :: unitin = 10
   integer(kind=int64) :: part_one, part_two        ! results
   integer(kind=int64) :: item, i                 ! work
   integer(kind=int64) :: reclen
   character(len=1024) :: recin

   ! input file is argument 1
   call open_aoc_input(unitin)

   ! initialize
   part_one = 0; part_two = 0

   ! for part one, identify the one item type that is duplicate in each rucksack
   ! and its priority value. sum these priorities and report the total.

   read_one: do

      if (.not. read_aoc_input(unitin, recin)) exit     ! read until end of file

      count_one = count_one + 1

      reclen = len_trim(recin)
      if (reclen < 1) cycle read_one

   end do read_one

   ! for part two, consider every three elves as a group. find the item in
   ! common in all three rucksacks and sum their priorities.

   call rewind_aoc_input(unitin)

   read_two: do

      if (.not. read_aoc_input(unitin, recin)) exit     ! read until end of file

      count_two = count_two + 1

      reclen = len_trim(recin)
      if (reclen < 1) cycle read_two

   end do read_two

   ! report and close
   print *
   print *, count_one, " records read "
   print *, count_two, " groups read "
   print *, part_one, " part one "
   print *, part_two, " part two "
   print *

   call close_aoc_input(unitin)

end program solution
!! solution.pas -- camp cleanup -- Troy Brumley blametroi@gmail.com
!
!program solution;
!
!!$longstrings off
!!$booleval on
!
!
!! advent of code 2022 day 04 -- camp cleanup
!
!! find overlapping grid assignments and report
!
!
!type
!
!   integer = int64;
!
!const
!
!   tGridNumBeg = 0;     ! the type includes 'impossible'
!   tGridNumMin = 1;     ! values at either end to make
!   tGridNumMax = 99;    ! some code cleaner
!   tGridNumEnd = 100;   ! input validate against min-max
!
!type
!
!   tGridNum = tGridNumBeg .. tGridNumEnd;
!   tGridSet = set of tGridNum;
!   tGridAssignment = record
!                        spec: string;
!                        gridSet: tGridSet;
!                     end;
!
!   ! a processed line from the input dataset.
!
!   tPayload = record
!                 one,
!                 two  : tGridAssignment;
!              end;
!
!   ! the input dataset for the days problem, one per line of input.
!   ! the original input line and its parsed value(s). blank lines are
!   ! maintained for control break purposes.
!
!   pData = ^tData;
!   tData = record
!              fwd,              ! a singly linked list would be
!              bwd : pData;      ! enough, but you never know
!              num : integer;    ! line number in source
!              rec : string;     ! original input
!              pld : tPayload;   ! what does it mean stimpy?
!           end;
!
!
!!$I str2gridset.p
!!$I gridset2str.p
!!$I aocinput.p
!
!
!! parse the input line into two gridsets. each line holds two
!! assignments to grids specified as ranges: low-high,low-high
!
!function parse(s : string) : tPayload;
!
!var
!   i, j, p : integer;
!   go, gt  : tGridAssignment;
!
!begin
!   go.spec := '';
!   for i := 1 to length(s) do
!      if s[i] <> ',' then
!         go.spec := go.spec + s[i]
!      else
!         break;
!   p := 1;
!   go.gridSet := str2gridset(go.spec, p);
!   p := 1;
!   gt.spec := '';
!   for j := i + 1 to length(s) do
!      gt.spec := gt.spec + s[j];
!   gt.gridSet := str2gridset(gt.spec, p);
!   parse.one := go;
!   parse.two := gt;
!end;
!
!
!! load the dataset into memory. since dynamic arrays aren't standard
!! across pascal implementations, and i dislike the implementation in
!! free pascal, a doubly linked list is used. it's certainly overkill
!! in terms of flexibility, and wouldn't scale well with large
!! datasets, but for advent of code a linear search on post 2010
!! hardware will be more than fast enough.
!
!function load : pData;
!
!var
!   first,
!   prior,
!   curr : pData;
!   i    : integer;
!   s    : string;
!
!begin
!   first := nil;
!   prior := nil;
!   curr := nil;
!   i := 0;
!   while not eof(input) do begin
!      i := i + 1;
!      readln(input, s);
!      new(curr);
!      curr^.fwd := nil;
!      curr^.bwd := prior;
!      curr^.num := i;
!      curr^.rec := s;
!      curr^.pld := parse(s);
!      if first = nil then
!         first := curr
!      else
!         prior^.fwd := curr;
!      prior := curr
!   end; ! while not eof
!
!   load := first;
!end;
!
!
!! see load and parse for more details. count pairs where one range
!! completely contains the other.
!
!function part_one(pd : pData) : integer;
!
!begin
!   part_one := 0;
!   while pd <> nil do
!      with pd^.pld do begin
!         if (one.gridSet <= two.gridSet) or (two.gridSet <= one.gridSet) then
!            part_one := part_one + 1;
!         pd := pd^.fwd
!      end; ! with pd^.pld
!end;
!
!
!! see load and parse for more details. count pairs where there is
!! any overlap.
!
!function part_two(pd : pData) : integer;
!
!begin
!   part_two := 0;
!   while pd <> nil do
!      with pd^.pld do begin
!         if (one.gridSet * two.gridSet) <> [] then
!            part_two := part_two + 1;
!         pd := pd^.fwd
!      end; ! with pd^.pld
!end;
!
!
!! the mainline driver for a day's solution. load and then iterate and
!! display results. input and output are fixed named files.
!
!procedure mainline;
!
!var
!   dataset  : pData;
!   p1answer,
!   p2answer : integer;
!
!begin
!   aocopen;
!   dataset := load;
!   p1answer := part_one(dataset);
!   p2answer := part_two(dataset);
!   writeln('part one answer : ', p1answer);
!   writeln('part two answer : ', p2answer);
!   aocclose;
!end;
!
!
!! minimal boot to mainline
!
!begin
!  mainline;
!end.
