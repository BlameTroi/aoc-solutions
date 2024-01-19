{ solution.pas -- regolith reservoir  -- Troy Brumley BlameTroi@gmail.com }

program solution;

{ expects to be compiled with the following two mode flags and command
  line/fpc.cfg options of -Ciort -Sgix -Sh- for full checks, allowing
  exceptions, goto, inlining, and prefer classic strings. }
{$LONGSTRINGS OFF}            { to be sure, use AnsiString or pchar explicitly }
{$BOOLEVAL ON}                { standard pascal expects this }


type
   integer = int64;            { by default ints are 64 bits this decade}


{=====================================================================}
{ advent of code 2022 day 14 -- regolith reservoir -- falling sand    }
{ will trap you!                                                      }
{                                                                     }
{ sand is falling down from a hole in the wall. the wall is hollow    }
{ but has rock structures (ledges and vertical runs) that slow,       }
{ divert, and accumulate the sand.                                    }
{                                                                     }
{ for part one, how much sand falls before no more can accumulate?    }
{                                                                     }
{ the origin of the sand is at 500,0. x increases to the right, and y }
{ increases toward the bottom.                                        }
{                                                                     }
{ input traces the solid rock structures that will divert and         }
{ accumulate sand, with straight lines from point to point.           }
{                                                                     }
{ 400,0->410,0->410,10 describes a blocky looking 7 10 units wide and }
{                          10 units tall.                             }
{                                                                     }
{ oh this is gonna be fun.                                            }
{                                                                     }
{ the small test data of:                                             }
{                                                                     }
{ 498,4 -> 498,6 -> 496,6 503,4 -> 502,4 -> 502,9 -> 494,9            }
{                                                                     }
{ describes a small backwards L nested above and to the left of a     }
{ larger backwards L with a serif across its top.                     }
{                                                                     }
{ 24 units of sand can fall before the overflow begins.               }
{                                                                     }
{ as with some other problems, this involves 'simultaneous' movement  }
{ in a unit of time.                                                  }
{                                                                     }
{ movement rules: sand moves 1 unit down unless blocked. if blocked   }
{                 attempt to move diagonally down and to the left. if }
{                 attempt fails, attempt to move diagonally down and  }
{                 to the right. if attempt fails, sand is at rest.    }
{                                                                     }
{                 once sand is past the lowest bound point (maximum y }
{                 value) the overflow has begun.                      }
{=====================================================================}




{========================================================================}
{ literals should be named constants most of the time                    }
{========================================================================}

const
   ORIGINX = 500;   { 473 .. 527 observed }
   ORIGINY = 0;     {  13 .. 165 observed }
   MINX    = 0;
   MAXX    = 1000;
   MINY    = 0;
   MAXY    = 200;
   ROCK    = '#';
   AIR     = '.';
   SAND    = 'o';


{========================================================================}
{ our domain types are points and queues                                 }
{========================================================================}

type
   tpoint  = record
                x, y : integer;
             end;



{========================================================================}
{ global variables are ok for most application state in advent of code   }
{========================================================================}

var
   grid   : array[MINX..MAXX, MINY..MAXY] of char;
   xrange : array[0..1] of integer;
   yrange : array[0..1] of integer;
   abyss  : integer;


{========================================================================}
{ housekeeping forward declarations so i don't have to worry about code  }
{ moving stuff around to deal with dependencies.                         }
{========================================================================}

procedure init; forward;
procedure load; forward;
function partone : integer; forward;
function parttwo : integer; forward;

procedure displaygrid; forward;
procedure horizontal(p1, p2 : tpoint; glyph : char); forward;
procedure vertical(p1, p2 : tpoint; glyph : char); forward;
procedure rockledge(p1, p2 : tpoint); forward;

function isnullpoint(p : tpoint) : boolean; forward;
function nullpoint : tpoint; forward;
function pointfrom(    s : string; var p : integer) : tpoint; forward;
function pointstr(p : tpoint) : string; forward;


{========================================================================}
{ common utilities for advent of code                                    }
{========================================================================}

{$I aocinput.p}
{$I str2int.p}
{$I int2str.p}
{$I substr.p}
{$I min.p}
{$I max.p}
{$I padleft.p}


{========================================================================}
{ mainline driver                                                        }
{========================================================================}

procedure mainline;

var
   i : integer;

begin
   aocopen;

   init;
   aocrewind;
   load;
   displaygrid;

   i := partone;
   writeln('part one answer : ', i);

   init;
   aocrewind;
   load;
   displaygrid;

   i := parttwo;
   writeln('part two answer : ', i);

   aocclose;
end;


{=========================================================================}
{ part one asks how long until sand starts overflowing the ledges         }
{                                                                         }
{ for part one we want to know how many grains of sand drop before all    }
{ the ledges overflow and sand starts falling into the abyss. i went a    }
{ bit wild with a simultaneous solution using queues since i missed that  }
{ the new grains are only spawned once the prior grain has stopped.       }
{                                                                         }
{ oh well, i'm doing this for coding prompts more than problem solving,   }
{ so the time wasn't wasted, just misdirected.                            }
{=========================================================================}

function partone : integer;

var
   done    : boolean;          { true if a grain has entered the abyss }
   falling : boolean;          { loop control flag for the fall }
   x, y    : integer;

begin
   partone := -1;              { start from -1, we want to know how many }
   done := false;              { grains it takes ~until~ we hit the abyss }

   while not done do begin

      { a new grain starts to fall }
      partone := partone + 1;
      x := ORIGINX; y := ORIGINY;
      falling := true;

      while falling and (y < abyss) do begin

         { fall straight down if there is room }
         if grid[x, y+1] = AIR then begin
            y := y + 1;
            continue;
         end;

         { can we fall to the left side? }
         if grid[x-1, y+1] = AIR then begin
            x := x - 1;
            y := y + 1;
            continue;
         end;

         { well then how about to the right side? }
         if grid[x+1, y+1] = AIR then begin
            x := x + 1;
            y := y + 1;
            continue;
         end;

         { mark the grid occupied by sand and let's drop another }
         falling := false;
         grid[x, y] := SAND;

      end;

      if y >= abyss then
         done := true;

   end;

   displaygrid;

end;


{=========================================================================}
{ part two changes the problem so that there is a floor instead of an     }
{ abyss. how many units of sand fall until the opening at 500,0 is        }
{ blocked by sand.                                                        }
{=========================================================================}

function parttwo : integer;

var
   done    : boolean;          { true if a grain has entered the abyss }
   falling : boolean;          { loop control flag for the fall }
   x, y    : integer;

begin
   parttwo := 0;              { start from 0 for part two }
   done := false;

   { lay down a floor for part two, there is no abyss }

   while not done do begin

      { a new grain starts to fall }
      parttwo := parttwo + 1;
      x := ORIGINX; y := ORIGINY;

      falling := true;
      while falling do begin

         { fall straight down if there is room }
         if grid[x, y+1] = AIR then begin
            y := y + 1;
            continue;
         end;

         { can we fall to the left side? }
         if grid[x-1, y+1] = AIR then begin
            x := x - 1;
            y := y + 1;
            continue;
         end;

         { well then how about to the right side? }
         if grid[x+1, y+1] = AIR then begin
            x := x + 1;
            y := y + 1;
            continue;
         end;

         { mark the grid occupied by sand and let's drop another }
         falling := false;
         grid[x, y] := SAND;

      end;
      grid[x, y] := SAND;
      done := (x = ORIGINX) and (y = ORIGINY);

   end;

   displaygrid;

end;


{========================================================================}
{ clear out global storage for a pass at the problem                     }
{========================================================================}

procedure init;

var
   x, y : integer;

begin

   { the wall }
   for x := MINX to MAXX do
      for y := MINY to MAXY do
         grid[x, y] := AIR;

   { ranges for display clipping }
   xrange[0] := MAXX+10; xrange[1] := MINX-10;
   yrange[0] := MAXY+10; yrange[1] := MINY-10;

   { marker for end }
   abyss := -1;

end;


{========================================================================}
{ read the ledge definitions and build the grid and ledges               }
{========================================================================}

procedure load;

var
   p     : integer;
   s     : string;
   currp : tpoint;
   nextp : tpoint;

begin

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

   { add a border around the ledges }
   xrange[0] := xrange[0] - 1; xrange[1] := xrange[1] + 1;
   yrange[0] := yrange[0] - 1; yrange[1] := yrange[1] + 1;

   { part one: when a grain reaches the abyss, we're done }
   abyss := yrange[1];
   { part two: there is a floor and the chamber is flooding }
   currp.x := low(grid); currp.y := yrange[1] + 1;
   nextp.x := high(grid); nextp.y := currp.y;
   rockledge(currp, nextp);

end;


{========================================================================}
{ functions dealing with the point datatype                              }
{========================================================================}

{ returns a new empty point }

function nullpoint : tpoint; inline;
begin
   with nullpoint do begin x := -1; y := -1; end;
end;


{ is the point empty? }

function isnullpoint(p : tpoint) : boolean; inline;
begin
   isnullpoint := (p.x = nullpoint.x) or (p.y = nullpoint.y);
end;


{ parse point coordinates from string s starting from offset p.
  p advances to the position after the point coordinates. }

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


{ displayable representation for formatted output }

function pointstr(p : tpoint) : string;
begin
   pointstr := '(' + padleft(int2str(p.x), 3) + ',' + padleft(int2str(p.y), 3) + ')';
end;


{========================================================================}
{ populate the grid with open air and ledges during load                 }
{========================================================================}

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


{========================================================================}
{ display grid with rock ledges and sand that has dropped so far         }
{========================================================================}

procedure displaygrid;
{ var }
{    x, y : integer; }
{    p    : tpoint; }
begin
   { writeln; }

   { display 500,0, the source of the sand...}
   { p.x := xrange[0]; p.y := 0; }
   { write(pointstr(p), ': '); }
   { for x := xrange[0] to xrange[1] do }
   {    if x = ORIGINX then }
   {       write('|') }
   {    else }
   {       write('='); }
   { p.x := xrange[1]; }
   { writeln(' :', pointstr(p)); }

   { now display the active grid area and border...}
   { for y := 1 to yrange[1] do begin }
   {    p.x := xrange[0]; p.y := y; }
   {    write(pointstr(p), ': '); }
   {    for x := xrange[0] to xrange[1] do }
   {       write(grid[x, y]); }
   {    p.x := xrange[1]; }
   {    writeln(' :', pointstr(p)); }
   { end; }
   { writeln; }
end;


{========================================================================}
{ mainline is really elsewhere                                           }
{========================================================================}

begin
   mainline;
end.

{ end solution.pas }
