{ solution.pas -- cosmic expansion  -- Troy Brumley BlameTroi@gmail.com }

program solution;

{ expects to be compiled with the following two mode flags and command
line/fpc.cfg options of -Ciort -Sgix -Sh- for full checks, allowing
exceptions, goto, inlining, and prefer classic strings. }
{$longstrings off}            { to be sure, use AnsiString or pchar explicitly }
{$booleval on}                { standard pascal expects this }

type
  integer = int64;            { by default ints are 64 bits this decade}

{ advent of code 2023 day 11 -- cosmic expansion
  for part 1 of day 11 is a sum of distances between
  "galaxies" in an expanding "universe." the expansion
  rules are relatively simple: each empty row or column
  becomes two. integer distances and "stair step" walks
  between galaxies should be kept in mind.

  for part 2 of day 11 ... after working through some of
  part 1 i halfway expect part 2 to just change that weight
  or cost of an empty row or column. so the cost to to
  cross a cell that is on a row and column both without a
  galaxy becomes 4. most obvious ways of solving part 1
  would not be at all amenable to this idea.

  when i got to part 2, i discovered i was partially right.
  the weight to cross any void row or column increased so
  much that some possible solutions to part 1 would bog
  bog down, but not as bad as my idea. }

const
{ limits from observation of data sets
  testone.txt is small (10, 10)
  testtwo.txt has yet to be created, it's a workpad
  dataset.txt is big (140, 140) }
{$define xTESTING }
{$define TESTONE }
{$ifdef TESTING }
{$ifdef TESTONE }
   MAXDIM = 10;
   MAXGALAXIES = 20;
{$else}
   MAXDIM = 10;
   MAXGALAXIES = 20;
{$endif}
{$else}
   MAXDIM = 140;
   MAXGALAXIES = 500; { actually 450 in my dataset }
{$endif}


{$I aocinput.p }
{$I int2str.p }


{ ---------------------------------------------------------
  global variables are appropriate for this problem.
  --------------------------------------------------------- }

{ in the test data, '.' is an empty grid and '#' is a galaxy. }
type
   tGalaxy = record
                tag : string[9];
                row : integer;
                col : integer;
             end;

var
   dataset     : array[1..MAXDIM] of string;
   cosmos      : array[1..MAXDIM, 1..MAXDIM] of char;
   galaxies    : array[1..MAXGALAXIES] of tGalaxy;
   numgalaxies : integer;
   gapdist     : integer;


{ initialize global variables. }

procedure init;

var
   i, j : integer;

begin
   for i := low(dataset) to high(dataset) do
      dataset[i] := '';

   for i := low(cosmos) to high(cosmos) do
      for j := low(cosmos[i]) to high(cosmos[i]) do
         cosmos[i, j] := ' ';

   for i:= low(galaxies) to high(galaxies) do
      with galaxies[i] do begin
         tag := ''; row := -1; col := -1;
      end;

   numgalaxies := 0;
   gapdist := 1;
end;


{ a handy handle, the coordinates. }

function maketag(r, c : integer) : string;

var
   s : string;

begin
   s := int2str(r);
   while length(s) < 3 do
      s := '0' + s;
   maketag := '{' + s + ',';
   s := int2str(c);
   while length(s) < 3 do
      s := '0' + s;
   maketag := maketag + s + '}';
end;


{ is a row or column devoid in the void? }

function galaxyonrow(r : integer) : boolean;

var
   c : integer;

begin
   galaxyonrow := false;
   c := low(cosmos[r]) - 1;
   while (not galaxyonrow) and (c < high(cosmos[r])) do begin
      c := c + 1;
      galaxyonrow := cosmos[r, c] = '#';
   end;
end;


{ is a row or column devoid in the void? }

function galaxyoncol(c : integer) : boolean;

var
   r : integer;

begin
   galaxyoncol := false;
   r := low(cosmos) - 1;
   while (not galaxyoncol) and (r < high(cosmos)) do begin
      r := r + 1;
      galaxyoncol := cosmos[r, c] = '#';
   end;
end;


{ return unsigned distance between rows or columns, accounting
  for the variable distance of the voids. }

function coldist(c1, c2 : integer) : integer;

begin
   coldist := 0;
   if c1 = c2 then
      exit
   else if c1 > c2 then begin
      coldist := -coldist(c2, c1);
      exit;
   end;
   while c1 < c2 do begin
      coldist := coldist + 1;
      if cosmos[low(cosmos), c1+1] in ['|', '+'] then
         coldist := coldist + gapdist;
      c1 := c1 + 1;
   end;
end;


function rowdist(r1, r2 : integer) : integer;

begin
   rowdist := 0;
   if r1 = r2 then
      exit
   else if r1 > r2 then begin
      rowdist := -rowdist(r2, r1);
   end;
   while r1 < r2 do begin
      rowdist := rowdist + 1;
      if cosmos[r1+1, low(cosmos[low(cosmos)])] in ['-', '+'] then
         rowdist := rowdist + gapdist;
      r1 := r1 + 1;
   end;
end;


{ walk from one galaxy to another, counting steps. }

function walker(p1, p2 : integer) : integer;

var
   r1, c1 : integer;
   r2, c2 : integer;
   dr, dc : integer;
   adj    : integer;

begin
   walker := 0;
   r1 := galaxies[p1].row; c1 := galaxies[p1].col;
   r2 := galaxies[p2].row; c2 := galaxies[p2].col;
   dr := rowdist(r1, r2);  dc := coldist(c1, c2);

   repeat

      { strategy, reduce largest gap first }
      if abs(dr) > abs(dc) then begin
         walker := walker + 1;
         if dr < 0 then adj := -1 else adj := 1;
         if cosmos[r1+adj, c1] in ['-', '+'] then
            walker := walker + gapdist;
         r1 := r1 + adj;
      end;

      { if equal advance row }
      if abs(dr) = abs(dc) then begin
         walker := walker + 1;
         if dr < 0 then adj := -1 else adj := 1;
         if cosmos[r1+adj, c1] in ['-', '+'] then
            walker := walker + gapdist;
         r1 := r1 + adj;
      end;

      { and finally do smaller }
      if abs(dr) < abs(dc) then begin
         walker := walker + 1;
         if dc < 0 then adj := -1 else adj := 1;
         if cosmos[r1, c1+adj] in ['|', '+'] then
            walker := walker + gapdist;
         c1 := c1 + adj;
      end;

      dr := rowdist(r1, r2); dc := coldist(c1, c2);

   until (dr = 0) and (dc = 0);
end;


function compgalaxies(i, j : integer) : integer;

begin
   compgalaxies := galaxies[i].row - galaxies[j].row;
   if compgalaxies <> 0 then
      exit;
   compgalaxies := galaxies[i].col - galaxies[j].col;
end;


procedure sortgalaxies;

var
   i, j : integer;
   s    : tGalaxy;

begin
   j := 0;
   while j < numgalaxies-1 do begin
      j := j + 1;
      for i := 1 to numgalaxies-j do
         if compgalaxies(i, i+1) > 0 then begin
            s.tag := galaxies[i].tag; s.row := galaxies[i].row; s.col := galaxies[i].col;
            galaxies[i].tag := galaxies[i+1].tag; galaxies[i].row := galaxies[i+1].row; galaxies[i].col := galaxies[i+1].col;
            galaxies[i+1].tag := s.tag; galaxies[i+1].row := s.row; galaxies[i+1].col := s.col;
         end;
   end;
end;


{ read the primordial universe and then big bang it to
  determine the cost to move across a row or column. }

procedure load;

var
   i, j : integer;

begin
   i := 0;
   while not eof(input) do begin
      i := i + 1;
      dataset[i] := aocread;
      for j := 1 to length(dataset[i]) do
         cosmos[i, j] := dataset[i][j]
   end;

  { update the cosmos map to mark empty rows and columns. }

   for i := low(cosmos) to high(cosmos) do
      if not galaxyonrow(i) then
         for j := low(cosmos[i]) to high(cosmos[i]) do
            if cosmos[i, j] = '.' then
               cosmos[i, j] := '-';

   for j := low(cosmos[low(cosmos)]) to high(cosmos[low(cosmos)]) do
      if not galaxyoncol(j) then
         for i := low(cosmos) to high(cosmos) do
            if cosmos[i, j] = '.' then
               cosmos[i, j] := '|'
            else if cosmos[i, j] = '-' then
               cosmos[i, j] := '+';

  { how many galaxies do we have, and where are they? }

   numgalaxies := 0;
   for i := low(cosmos) to high(cosmos) do
      for j := low(cosmos[i]) to high(cosmos[i]) do begin
         if cosmos[i, j] <> '#' then
            continue;
         numgalaxies := numgalaxies + 1;
         galaxies[numgalaxies].tag := maketag(i, j);
         galaxies[numgalaxies].row := i;
         galaxies[numgalaxies].col := j;
      end;

   sortgalaxies;
end;


{ driver for part one }
function partone : integer;

var
   i, j : integer;

begin
   partone := 0;
   gapdist := 1;
   for i := 1 to numgalaxies - 1 do
      for j := i+1 to numgalaxies do
         partone := partone + walker(i, j);
end;


{ driver for part two }

function parttwo : integer;

var
   i, j : integer;

begin
   parttwo := 0;
   gapdist := 1000000 - 1; { read the directions carefully }
   for i := 1 to numgalaxies - 1 do
      for j := i+1 to numgalaxies do
         parttwo := parttwo + walker(i, j);
end;


{
  mainline
}

var
  i : integer;

begin
   aocopen;
   init;
   load;
   i := partone;
   writeln('part one: ', i);
   i := parttwo;
   writeln('part two: ', i);
   aocclose;
end.

