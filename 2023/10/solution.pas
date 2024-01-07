{ solution.pas -- pipe maze  -- Troy Brumley BlameTroi@gmail.com }

program solution;

{ expects to be compiled with the following two mode flags and command
line/fpc.cfg options of -Ciort -Sgix -Sh- for full checks, allowing
exceptions, goto, inlining, and prefer classic strings. }
{$longstrings off}            { to be sure, use AnsiString or pchar explicitly }
{$booleval on}                { standard pascal expects this }

type
  integer = int64;            { by default ints are 64 bits this decade}

{ advent of code 2023 day 10 -- pipe maze

  for part 1 of day 10 we're doing a variation of the longest path
  through a maze. while there's no exit point, find the point on the
  maze furthest from the entry point. and of course there are
  extraneous and unconnected paths in the maze.

  for part 2 of day 10 we need to count the number of points in the
  maze that are completely within the looping path. a simple flood
  fill won't work without a lot of work around the issue of parallel
  adjecent pipes opening into what might look to be an area within the
  loop but really isn't. after being stuck for several days i checked
  for hints but none of them led to a solution for me. eventually i
  saw "point in a polygon" and some fruitful searching led me to ray
  casting. }

const

{ limits from observation of data sets
  testone.txt is small (5, 5) but has bogus pipes
  testtwo.txt has the same loop but no bogus pipes
  testten.txt (21, 12)
  testfoura/b (9, 11)
  ... a/b same answer but the arrangment has a need
      to fill between parallel pipes to get all the
      outside points. }

{$define xTESTING }
{$define xTESTONE }
{$ifdef TESTING }
{$ifdef TESTONE }
  MAXROWS = 9;
  MAXCOLS = 11;
{$else}
  MAXROWS = 9;
  MAXCOLS = 11;
{$endif}
{$else}
  MAXROWS = 140;
  MAXCOLS = 140;
{$endif}
{$I constchars.p }


{$I aocinput.p }


{ glyphs representing a node in the maze in the maze. }
           { origin = 'S'   origin, must infer shape  }
           { ground = '.'   ornt  side     ornt  side }
           { ns     = '|'   vert  both                }
           { ew     = '-'                  horz  both }
           { ne     = 'L'   vert  left     horz  bott }
           { nw     = 'J'   vert  right    horz  bott }
           { sw     = '7'   vert  right    horz  top  }
           { se     = 'F'   vert  left     horz  top  }


{ ---------------------------------------------------------
  global variables are appropriate for this problem.
  --------------------------------------------------------- }

var
   { the 's' or entry node location }
   begrow,
   begcol  : integer;

   { the original input }
   dataset : array[1..MAXROWS] of string;

   { dataset loaded as an 2d array }
   glyphs  : array[1..MAXROWS, 1..MAXCOLS] of char;

   { as we learn a node's status, mark it in the
     appropriate array }
   filled  : array[1..MAXROWS, 1..MAXCOLS] of char;
   onloop  : array[1..MAXROWS, 1..MAXCOLS] of char;
   inside  : array[1..MAXROWS, 1..MAXCOLS] of char;


{ ---------------------------------------------------------
  utility
  --------------------------------------------------------- }

{ initialize global variables. }

procedure init;

var
   i, j : integer;

begin
   for i := low(dataset) to high(dataset) do
      dataset[i] := '';
   for i := low(glyphs) to high(glyphs) do
      for j := low(glyphs[i]) to high(glyphs[i]) do begin
         glyphs[i, j] := ' ';
         filled[i, j] := ' ';
         onloop[i, j] := ' ';
         inside[i, j] := ' ';
      end;
   begrow := -1; begcol := -1;
end;


{ ---------------------------------------------------------
  common accessors and checks. these all guard against out
  of bounds access and return sensible values when that
  happens.

  it's cleaner to just call most functions and have them
  return a 'naw, not here' response than to avoid calling
  them.
  --------------------------------------------------------- }

function validcoord(r, c : integer) : boolean;

begin
   validcoord := false;
   if (r < low(glyphs)) or (r > high(glyphs)) then
      exit;
   if (c < low(glyphs[r])) or (c > high(glyphs[r])) then
      exit;
   validcoord := true;
end;


{ is the point on the loop? }

function isonloop(r, c : integer) : boolean;

begin
   isonloop := false;
   if validcoord(r, c) then
      isonloop := onloop[r, c] <> ' ';
end;


{ could we go in a cardinal direction from a point? there pipe shape
  at the current point has to be able to reach that direction, and the
  point there can't be marked as part of the loop. }

function cangonorth(r, c : integer) : boolean;

begin
   cangonorth := false;
   if (not validcoord(r, c)) or (not validcoord(r-1, c)) then
      exit;
   cangonorth := (not isonloop(r-1, c)) and (glyphs[r, c]in ['|', 'L', 'J'])
end;

function cangoeast(r, c : integer) : boolean;

begin
   cangoeast := false;
   if (not validcoord(r, c)) or (not validcoord(r, c+1)) then
      exit;
   cangoeast := (not isonloop(r, c+1)) and (glyphs[r, c]in ['-', 'L', 'F'])
end;

function cangowest(r, c : integer) : boolean;

begin
   cangowest := false;
   if (not validcoord(r, c)) or (not validcoord(r, c-1)) then
      exit;
   cangowest := (not isonloop(r, c-1)) and (glyphs[r, c]in ['-', 'J', '7'])
end;

function cangosouth(r, c : integer) : boolean;

begin
   cangosouth := false;
   if (not validcoord(r, c)) or (not validcoord(r+1, c)) then
      exit;
   cangosouth := (not isonloop(r+1, c)) and (glyphs[r, c] in ['|', 'F', '7'])
end;


{ ---------------------------------------------------------
  loader and support.
  --------------------------------------------------------- }


{ determine the shape of the origin pipe by examining its
  neighbors. }

function inferorigin : char;

var
   n, s, e, w : boolean;

begin
   inferorigin := 'S';

   n := false; s := false; e := false; w := false;
   if begrow > low(glyphs) then
      n := glyphs[begrow-1, begcol] in ['7', '|', 'F'];
   if begrow < high(glyphs) then
      s := glyphs[begrow+1, begcol] in ['J', '|', 'L'];
   if begcol > low(glyphs[low(glyphs)]) then
      w := glyphs[begrow, begcol-1] in ['L', '-', 'F'];
   if begcol < high(glyphs[low(glyphs)]) then
      e := glyphs[begrow, begcol+1] in ['J', '-', '7'];

   if n and s then
      inferorigin := '|'
   else if n and w then
      inferorigin := 'J'
   else if n and e then
      inferorigin := 'L'
   else if w and e then
      inferorigin := '-'
   else if s and w then
      inferorigin := '7'
   else if s and e then
      inferorigin := 'F'

end;


{ read the maze map and parse it. }

procedure load;

var
   i, j : integer;

begin
   init;
   aocrewind;

   i := 0;
   while not aoceof do begin
      i := i + 1;
      dataset[i] := aocread;
   end;

   for i := low(glyphs) to high(glyphs) do
      for j := low(glyphs[i]) to high(glyphs[i]) do begin
         glyphs[i, j] := dataset[i][j];
         if glyphs[i, j] = 'S' then begin
            begrow := i; begcol := j;
         end;
      end;

   if begrow = -1 then
      aochalt('failed to find origin point');

   glyphs[begrow, begcol] := inferorigin;
   if glyphs[begrow, begcol] = 'S' then
      aochalt('could not resolve origin pipe shape');
end;


{ ---------------------------------------------------------
  part one

  walk around the loop from point to point. due to the loop
  nature of the pipes, we know that there are only two paths
  into (or out of) a point.

  picking a direction from the origin, walk until the end is
  reached. this is defind as there being no unvisted point
  available. the solution to the problem is 1/2 this number
  of steps.
  --------------------------------------------------------- }

{ recursive helper for the walk. }

function loopwalker(r, c : integer) : integer;

begin
   loopwalker := 1;        { count this point }
   onloop[r, c] := glyphs[r, c]; { and note that we've been visited }
   if cangonorth(r, c) then
      loopwalker := loopwalker + loopwalker(r-1, c)
   else if cangosouth(r, c) then
      loopwalker := loopwalker + loopwalker(r+1, c)
   else if cangoeast(r, c) then
      loopwalker := loopwalker + loopwalker(r, c+1)
   else if cangowest(r, c) then
      loopwalker := loopwalker + loopwalker(r, c-1);
end;


{ every journey begins with a single step. here we step into the maze
  at the origin, pick one of the two paths available, and start
  walking. }

function loopwalk : integer;

begin
   onloop[begrow, begcol] := glyphs[begrow, begcol];
   loopwalk := 1;
   if glyphs[begrow, begcol] = '|' then
      loopwalk := loopwalker(begrow-1, begcol)
   else if glyphs[begrow, begcol] = '-' then
      loopwalk := loopwalker(begrow, begcol+1)
   else if glyphs[begrow, begcol] in ['J', 'L'] then
      loopwalk := loopwalker(begrow-1, begcol)
   else if glyphs[begrow, begcol] in ['7', 'F'] then
      loopwalk := loopwalker(begrow+1, begcol)
   else
      aochalt('invalid pipe configuration at origin');
   writeln(output, 'loopwalk from origin result ', loopwalk);
end;


{ driver for part one }
function partone : integer;

var
   i : integer;

begin
   init;
   load;
   i := loopwalk;
   i := (i div 2) + (i mod 2);
   partone := i;
end;


{ ---------------------------------------------------------
  part two.

  find out how many points are inside the loop. the tricky
  part is that it is possible to seep through parallel pipes
  in which case the points are not enclosed. this is a paint
  fill sort of problem.
  --------------------------------------------------------- }


{ fill points in the maze starting from r, c. can be called with
  invalid coordinates and quietly ignores the request. it also quietly
  ignores the request if the point has been filled already or is part
  of the loop. }

procedure filler(r, c : integer);

begin
  if not validcoord(r, c) then
    exit;
  if (filled[r, c] <> ' ') or (onloop[r, c] <> ' ') then
    exit;
  filled[r, c] := 'O';
  filler(r-1, c-1); filler(r-1, c); filler(r+1, c+1);
  filler(r,   c-1);                 filler(r,   c+1);
  filler(r+1, c+1); filler(r+1, c); filler(r+1, c+1);
end;


{ any outer row or column grid that isn't on the loop can be filled.
  most of the requests after the first couple will return without
  doing anything as the nodes will have been marked filled.

  while the spec allows filling to bleed through parallel pipes in the
  maze, that's being deferred to a later step.

  when this fill is done, any nodes that are neither filled nor on the
  loop are candidates for part two. attempting to figure out the
  parallel pipes. }

procedure firstfill;

var
   r : integer;

begin
   for r := low(glyphs) to high(glyphs) do begin
      filler(r, low(glyphs[r]));
      filler(r, high(glyphs[r]));
   end;
end;


{ is this point inside the loop? use raycasting to check. }

function interior(r, c : integer) : boolean;

var
   ray       : integer;
   crossings : integer;

begin
   interior := false;
   crossings := 0;

   if (r = low(glyphs)) or (r = high(glyphs)) then
      exit;
   if (c = low(glyphs[r])) or (c = high(glyphs[r])) then
      exit;

   for ray := low(glyphs[r]) to c do begin
      if onloop[r, ray] in [' ', '-', 'J', 'L'] then
         continue;
      crossings := crossings + 1;
   end;

   interior := (crossings > 0) and ((crossings mod 2) = 1);
end;


{ check any remaining unmarked points in the maze. if it isn't filled
  or on the loop, it might be completely contained within the loop.
  use raycasting to see if it is, and if it isn't, fill it. }

function checkremaining : integer;

var
   r, c : integer;
   count : integer;

begin
   count := 0;
   for r := low(glyphs) to high(glyphs) do begin
      for c := low(glyphs[r]) to high(glyphs[r]) do begin
         if (onloop[r, c] = ' ') and (filled[r, c] = ' ') then begin
            inside[r, c] := '?';
            if interior(r, c) then begin
               count := count + 1;
               inside[r, c] := 'I';
            end;
         end;
      end;
   end;
   checkremaining := count;
end;


{ find every node that is neither filled nor one of the loop nodes. is
  there a path from it to a filled node? if so, fill it. whatever is
  left after all that must be an interior node. }

function parttwo : integer;

begin
   firstfill;
   parttwo := checkremaining;
end;

{ mainline }

var
   i : integer;

begin
   aocopen;
   i := partone;
   writeln('part one: ', i);
   i := parttwo;
   writeln('part two: ', i);
   aocclose;
end.
