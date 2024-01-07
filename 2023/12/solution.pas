{ solution.pas -- hot springs -- Troy Brumley blametroi@gmail.com }
program solution;

{$booleval on}
{$longstrings off}

type
   integer = int64;

{ for part 1 of day 12 looks to be permutation based.

for part 2 of day 12 ... tbd. }

const
{$define xTESTING }
{$ifdef TESTING }
   MAXDIM = 6;
   MAXGROUPINGS = 7;
   MAXSPRINGS = 20;
{$else}
   MAXDIM = 1000;
   MAXGROUPINGS = 7;
   MAXSPRINGS = 20;
{$endif}


{ input format is spring flags (consts below) a space, and then a
  sequence of comma separated counts. these are the sizes of each
  contiguous group of damaged springs. these sizes are in the correct
  order on the line and groups are always separated by at least one
  operational spring.

  so ?###???????? 3,2,1 could map 10 ways, but the first grouping is
  always .###. since the leading 3 means 3 broken which will require
  working spring on either side, making the two ? tags into . tags.

  for part one, how many arrangements are possible for each input
  line. sum them up. }

const
   OPERATIONAL = '.';
   DAMAGED = '#';
   UNKNOWN = '?';

type
   { a generalized return for checking functions }
   tStatusReturn = record
                      ok  : boolean;
                      val : integer;
                      msg : string;
                   end;


{$I aocinput.p }
{$I padleft.p }
{$I str2int.p }
{$I int2str.p }
{$I strindex.p }
{$I min.p }
{$I max.p }


var
  dataset     : array[1..MAXDIM] of string;
  conditions  : array[1..MAXDIM, 1..MAXSPRINGS] of char;
  groups      : array[1..MAXDIM, 0..MAXGROUPINGS] of integer; { 0th is number of groups following }
  counts      : array[1..MAXDIM, 1..3] of integer;
  mingroups,
  maxgroups   : integer;
  numread     : integer;
  arrangements: array[1..MAXDIM] of integer;
  frequencies : array[1..MAXGROUPINGS] of integer;


{ initialize global variables. }

procedure init;

var
   i, j : integer;

begin
   for i := low(dataset) to high(dataset) do
      dataset[i] := '';
   for i := low(conditions) to high(conditions) do
      for j := low(conditions[low(conditions)]) to high(conditions[low(conditions)]) do
         conditions[i, j] := ' ';
   for i := low(groups) to high(groups) do
      for j := low(groups[low(groups)]) to high(groups[low(groups)]) do
         groups[i, j] := 0;
   for i := low(counts) to high(counts) do
      for j := low(counts[i]) to high(counts[i]) do
         counts[i, j] := 0;
   mingroups := MAXGROUPINGS + 1;
   maxgroups := -1;
   for i := 1 to high(frequencies) do
      frequencies[i] := 0;
end;


{ how many of c do we find in the condition report }

function countchars(n : integer; { subscript into conditions }
                    c : char     { character to search }
                    ) : integer;
var
   i : integer;

begin
   countchars := 0;
   for i := 1 to high(conditions[low(conditions)]) do
      if conditions[n, i] = c then
         countchars := countchars + 1;
end;


{ how many broken springs are there, take the sum of the group reports. }

function sumgroups(n : integer  {subscript into conditions }
                   ) : integer;
var
  i : integer;
begin
  sumgroups := 0;
  for i := 1 to groups[n, 0] do
    sumgroups := sumgroups + groups[n, i];
end;


{ poorly named, but how many runs of existing damaged springs do we
  find in the condition report. }

function countgroups(n : integer  { subscript into conditions }
                     ) : integer;
var
   i : integer;
   c : char;

begin
   countgroups := 0;
   c := chr(0);
   for i := 1 to high(conditions[low(conditions)]) do begin
      if conditions[n, i] = c then
         continue;
      if conditions[n, i] = DAMAGED then
         countgroups := countgroups + 1;
      c := conditions[n, i];
   end;
end;


{ a first attempt at testing if a condition report is valid by several
  different measures }

function isvalidcond(n : integer  { subscript into conditions }
                     ) : tStatusReturn;
var
   i, j, k : integer;
   s : string;
   c : char;

begin
   isvalidcond.ok := false;
   isvalidcond.val := 0;
   isvalidcond.msg := 'not set';

   if countchars(n, UNKNOWN) > 0 then begin
      isvalidcond.msg := 'still has unknowns';
      exit;
   end;

   if countchars(n, DAMAGED) <> sumgroups(n) then begin
      isvalidcond.msg := 'wrong number of damaged springs';
      exit;
   end;

   if countgroups(n) <> groups[n, 0] then begin
      isvalidcond.msg := 'wrong number of damaged spring groups';
      exit;
   end;

end;


{ for the part one analysis, repeated runs of operational springs can
  be reduced to a single spring. }

function deduprow(s : string) : string;

var
   i : integer;
   c : char;

begin
   deduprow := s;
   if length(s) = 0 then
      exit;
   if strindex(s, OPERATIONAL) < 1 then
      exit;
   c := OPERATIONAL;
   i := 0;
   deduprow := '';
   while i < length(s) do begin
      i := i + 1;
      if (s[i] = OPERATIONAL) and (c = OPERATIONAL) then
         continue;
      deduprow := deduprow + s[i];
      c := s[i];
   end;
end;


{ read the spring condition records }

procedure load;

var
   i, j, p : integer;
   s : string;

begin
   i := 0;
   while not aoceof do begin
      i := i + 1;
      s := aocread;
      s := deduprow(s); { let's remove runs of redundant good springs }
      p := 0;
      while p < length(s) do begin
         p := p + 1;
         if s[p] = ' ' then
            break;
         conditions[i, p] := s[p];
         if s[p] = '.' then
            counts[i, 1] := counts[i, 1] + 1  { operational }
         else if s[p] = '#' then
            counts[i, 2] := counts[i, 2] + 1  { damaged }
         else
            counts[i, 3] := counts[i, 3] + 1; { unknown }
      end;
      j := 0;
      p := p + 1; { past space }
      while p <= length(s) do begin
         j := j + 1;
         groups[i, j] := str2int(s, p);
         p := p + 1; { past , }
      end;
      groups[i, 0] := j; { leading length }
      frequencies[j] := frequencies[j] + 1;
      mingroups := min(j, mingroups);
      maxgroups := max(j, maxgroups);
   end;
   numread := i;
end;


procedure report;

var
   i, j : integer;
   s    : string;

begin
   aoclog('');
   aoclog('*** dataset report ***');
   aoclog('');
   for i := 1 to numread do begin
      s := padleft(int2str(i), 4) + ' ';
      for j := 1 to high(conditions[i]) do
         s := s + conditions[i, j];
      s := s + ' ';
      s := s + padleft(int2str(counts[i, 1]), 3) + '(.)';
      s := s + padleft(int2str(counts[i, 2]), 3) + '(#)';
      s := s + padleft(int2str(counts[i, 3]), 3) + '(?)';
      s := s + padleft(int2str(groups[i, 0]), 3);
      for j := 1 to groups[i, 0] do
         s := s + padleft(int2str(groups[i, j]), 3);
      if sumgroups(i) = counts[i, 2] then
         s := s + ' ***groups=#***';
      if sumgroups(i) = counts[i, 2] + counts[i, 3] then
         s := s + ' ***groups=#?***';
      if sumgroups(i) = counts[i, 3] then
         s := s + ' ***groups=?***';
      aoclog(s);
   end;
   aoclog('');
   aoclog('minimum groups on row ' + int2str(mingroups));
   aoclog('maximum groups on row ' + int2str(maxgroups));
   aoclog('grouping frequency');
   for i := 1 to high(frequencies) do
      aoclog(padleft(int2str(i), 5) + padleft(int2str(frequencies[i]), 5));
   aoclog('');
   aoclog('*** end dataset report ***');
   aoclog('');
end;


{ driver for part one }

function partone : integer;

begin
   partone := 0;
end;


{ driver for part two }

function parttwo : integer;

begin
   parttwo := 0;
end;


{ mainline }

var
   i : integer;

begin
   aocopen;
   aoccomment(';');

   init;
   load;
   report;

   i := partone;
   writeln('part one answer : ', i);
   i := parttwo;
   writeln('part two answer : ', i);

   aocclose;
end.
