{ solution.pas -- regolith reservoir  -- Troy Brumley BlameTroi@gmail.com }

program solution;

{ expects to be compiled with the following two mode flags and command
  line/fpc.cfg options of -Ciort -Sgix -Sh- for full checks, allowing
  exceptions, goto, inlining, and prefer classic strings. }
{$LONGSTRINGS OFF}            { to be sure, use AnsiString or pchar explicitly }
{$BOOLEVAL ON}                { standard pascal expects this }

type
   integer = int64;            { by default ints are 64 bits this decade}

{ advent of code 2022 day 14 -- regolith reservoir -- falling sand will trap
  you!

  sand is falling down from a hole in the wall. the wall is hollow but has
  rock structures (ledges and vertical runs) that slow, divert, and accumulate
  the sand.

  for part one, how much sand falls before no more can accumulate?

  the origin of the sand is at 500,0. x increases to the right, and y
  increases toward the bottom.

  input traces the solid rock structures that will divert and accumulate
  sand, with straight lines from point to point.

  400,0->410,0->410,10     describes a blocky looking 7 10 units wide and
                           10 units tall.

  oh this is gonna be fun.

  the small test data of:

  498,4 -> 498,6 -> 496,6
  503,4 -> 502,4 -> 502,9 -> 494,9

  describes a small backwards L nested above and to the left of a larger
  backwards L with a serif across its top.

  24 units of sand can fall before the overflow begins.

  as with some other problems, this involves 'simultaneous' movement in
  a unit of time.

  movement rules: sand moves 1 unit down unless blocked.
                  if blocked attempt to move diagonally down and to the left.
                  if attempt fails, attempt to move diagonally down and to the right.
                  if attempt fails, sand is at rest.

                  once sand is past the lowest bound point (maximum y value)
                  the overflow has begun.
}

const
   ORIGINX = 500;   { 473 .. 527 observed }
   ORIGINY = 0;     {  13 .. 165 observed }
   MINX    = 470;   { so add a little around the edges }
   MAXX    = 530;
   MINY    = 0;
   MAXY    = 200;
   ROCK    = '#';
   AIR     = '.';
   SAND    = 'o';
   STUCK   = '_';

type
   tpoint = record
               x, y : integer;
            end;

var
   grid : array[MINX..MAXX, MINY..MAXY] of char;

{$I aocinput.p}
{$I str2int.p}
{$I int2str.p}
{$I substr.p}
{$I min.p}
{$I max.p}
{$I padleft.p}

function nullpoint : tpoint; inline;
begin
   with nullpoint do begin x := -1; y := -1; end;
end;

function isnullpoint(p : tpoint) : boolean; inline;
begin
   { or not and, allows for handling input errors }
   isnullpoint := (p.x = nullpoint.x) or (p.y = nullpoint.y);
end;

function pointfrom(    s : string;
                   var p : integer) : tpoint;
begin
   pointfrom := nullpoint;
   with pointfrom do begin
      x := str2int(s, p);
      p := p + 1;
      y := str2int(s, p);
   end;
end;

function pointstr(p : tpoint) : string;
begin
   pointstr := '(' + padleft(int2str(p.x), 3) + ',' + padleft(int2str(p.y), 3) + ')';
end;

procedure init;

var
   x, y : integer;

begin
   for x := MINX to MAXX do
      for y := MINY to MAXY do
         grid[x, y] := AIR;
end;

procedure vertical(p1, p2 : tpoint; glyph : char);

var
   y : integer;

begin
   if p1.y > p2.y then
      vertical(p2, p1, glyph)
   else
      for y := p1.y to p2.y do
         grid[p1.x, y] := glyph;
end;

procedure horizontal(p1, p2 : tpoint; glyph : char);

var
   x : integer;

begin
   if p1.x > p2.x then
      horizontal(p2, p1, glyph)
   else
      for x := p1.x to p2.x do
         grid[x, p1.y] := glyph;
end;

procedure rockledge(p1, p2 : tpoint);

begin
   if p1.x = p2.x then
      vertical(p1, p2, ROCK)
   else
      horizontal(p1, p2, ROCK);
end;

var
   x, y         : integer;
   p            : integer;
   s            : string;
   currp, nextp : tpoint;
   xrange       : array[0..1] of integer;
   yrange       : array[0..1] of integer;

begin
   init;
   aocopen;
   aocrewind;

   { ranges for display clipping }
   xrange[0] := MAXX+10; xrange[1] := MINX-10;
   yrange[0] := MAXY+10; yrange[1] := MINY-10;

   { load, track extents, and draw the ledges }
   while not aoceof do begin
      s := aocread;
      if s = '' then
         continue;
      p := 1;
      currp := nullpoint;
      while p < length(s) do begin
         { input is "x1,y1 -> x2,y2 -> x3,y3" where each pair is vertical or
           horizontal line segment. }
         nextp := pointfrom(s, p);
         xrange[0] := min(xrange[0], nextp.x);
         xrange[1] := max(xrange[1], nextp.x);
         yrange[0] := min(yrange[0], nextp.y);
         yrange[1] := max(yrange[1], nextp.y);
         if not isnullpoint(currp) then
            rockledge(currp, nextp);
         if substr(s, p, p+3) = ' -> ' then begin
            currp := nextp;
            nextp := nullpoint;
            p := p + 4;
            continue;
         end;
         if p < length(s) then
            aochalt('unexpected input at ' + int2str(p) + ' in "' + s + '"');
      end;
   end;

   { display the tracks of my tears }
   writeln;
   xrange[0] := xrange[0] - 1; xrange[1] := xrange[1] + 1;
   yrange[0] := yrange[0] - 1; yrange[1] := yrange[1] + 1;
   nextp.x := xrange[0]; nextp.y := 0;
   write(pointstr(nextp), ': ');
   for x := xrange[0] to xrange[1] do
      if x = ORIGINX then
         write('V')
      else
         write('-');
   nextp.x := xrange[1];
   writeln(' :', pointstr(nextp));
   for y := yrange[0] to yrange[1] do begin
      nextp.x := xrange[0]; nextp.y := y;
      write(pointstr(nextp), ': ');
      for x := xrange[0] to xrange[1] do
         write(grid[x, y]);
      nextp.x := xrange[1];
      writeln(' :', pointstr(nextp));
   end;
   writeln;

   { run part one ... }
   aocclose;
end.

{ end solution.pas }
