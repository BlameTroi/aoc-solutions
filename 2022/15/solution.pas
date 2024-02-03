{ solution.pas -- beacon exclusion zone -- Troy Brumley BlameTroi@gmail.com }

program solution;

{ expects to be compiled with the following two mode flags and command
  line/fpc.cfg options of -Ciort -Sgix -Sh- for full checks, allowing
  exceptions, goto, inlining, and prefer classic strings. }

{$LONGSTRINGS OFF}            { to be sure, use AnsiString or pchar explicitly }
{$BOOLEVAL ON}                { standard pascal expects this }

type
   integer = int64;            { by default ints are 64 bits this decade}

{========================================================================}
{ advent of code 2022 day 15 -- beacon exclusion zone                    }
{                                                                        }
{ given sensors that can identify the closest beacon, cast a net of      }
{ sensors to map beacons. then determine where a beacon can not be in a  }
{ specific row.                                                          }
{========================================================================}


const
   MAXDIM = 30; { only 23 in the test data, but always pad }

type
   tsensor = record
                x, y   : integer;       { location }
                bx, by : integer;       { detected beacon location }
                dist   : integer;       { its distance }
                tydist : integer;       { would x, ty be in range? }
                tyminx : integer;       { x crosses target y at min }
                tymaxx : integer;       { and max }
                nx, ny : integer;       { coordinates of 'diamond' points at range+1 }
                ex, ey : integer;       { yes, some redundancy in the data }
                sx, sy : integer;       { but the target point must be at the }
                wx, wy : integer;       { intersection of two of these lines }
             end;
   trange  = record
                lo : integer;
                hi : integer;
             end;
   tline   = record
                m : integer;            { y = m*x + b, in this problem m is + or - 1 }
                x : integer;
                b : integer;
             end;

var
   sensor  : array [0..MAXDIM] of tsensor;
   sensors : integer;
   minx    : integer;
   miny    : integer;
   maxx    : integer;
   maxy    : integer;
   targety : integer;
   mintyx  : integer;
   maxtyx  : integer;
   range   : array [0..MAXDIM] of trange;
   ranges  : integer;
   line    : array [0..MAXDIM*4] of tline;

{========================================================================}
{ forward definitions and common code includes                           }
{========================================================================}

{$I aocinput.p}
{$I str2int.p}
{$I int2str.p}
{$I min.p}
{$I max.p}
{$I strbegins.p}
{$I strindex.p}


{ the always present framework }
procedure init; forward;
procedure load; forward;
procedure display; forward;
function partone : integer; forward;
function parttwo : integer; forward;


{ problem specific forwards }
procedure sortranges; forward;
function collapseranges : boolean; forward;
function mergeranges : boolean; forward;
function calcdistance(x1, y1, x2, y2 : integer) : integer; forward;
function pointstr(x, y : integer) : string; forward;
function base36(i : integer) : char; forward;

procedure slopeintercept(var m      : integer;           { slope }
                         var b      : integer;           { constant }
                             xa, ya,
                             xb, yb : integer); forward;

procedure intersection(var x              : integer;     { intersection coordinates }
                       var y              : integer;     { of the lines a-b and c-d }
                           xa, ya, xb, yb,
                           xc, yc, xd, yd : integer); forward;


{========================================================================}
{ standard mainline for advent of code                                   }
{========================================================================}

procedure mainline;
var
   i : integer;
begin
   aocopen;

   init;
   aocrewind;
   load;
   display;
   i := partone;
   writeln('part one answer : ', i);

   init;
   aocrewind;
   load;
   i := parttwo;
   writeln('part two answer : ', i);

   aocclose;
end;


{======================================================================}
{ part one -- where aren't the beacons                                 }
{                                                                      }
{ for part one we want to find out how many grid positions on a        }
{ particular row (y=2,000,000 in the live data, and y=10 in the test   }
{ data).                                                               }
{                                                                      }
{ a grid and count solution will work for the test data to check my    }
{ work, but the live data is looking too big for that to be practical. }
{                                                                      }
{ what we have to do is determine the x range along the target y line  }
{ that falls between the two x,y points that mark the distance to the  }
{ nearest beacon. since the distances are 'manhattan' distances the    }
{ calculation should be straight forward algebra.                      }
{                                                                      }
{======================================================================}

function partone  : integer;
var
   i, j   : integer;
   adjx   : integer;

begin

   partone := 0;

{========================================================================}
{ scan for sensors that can see the target y line, and then find the     }
{ points of intersection of a circle of radius distance-to-beacon and    }
{ the line y=targety. there are either one or two possible points, but   }
{ the problem statement says 'no ties' so a tangent line y=targety is    }
{ not possible.                                                          }
{                                                                        }
{ so range - abs(sensor y - target y) gives the adjustment to sensor x.  }
{========================================================================}
   j := 0;
   for i := 0 to sensors-1 do
      with sensor[i] do begin
         if dist > tydist then begin
            adjx := dist - abs(y - targety);
            tyminx := x - adjx;
            tymaxx := x + adjx;
            writeln('sensor ', i:2, ' at ', pointstr(x, y), ' has a range of ', dist, ' and can see along line y=', targety, ' from ', tyminx, ' to ', tymaxx);
            range[j].lo := tyminx; range[j].hi := tymaxx;
            j := j + 1;
            mintyx := min(mintyx, tyminx); maxtyx := max(maxtyx, tymaxx);
         end;
      end;
   ranges := j;

   sortranges;

   writeln;
   writeln('sorted ranges ...');
   for i := 0 to ranges-1 do begin
      writeln(range[i].lo:12, range[i].hi:12);
   end;

   writeln;
   writeln('collapsing ranges ...');
   while collapseranges do begin
      { blip }
   end;
   writeln;
   writeln('merging ranges ...');
   while mergeranges do begin
      { bloop }
   end;

   writeln;
   writeln('collapsed and merged ranges ...');
   for i := 0 to ranges-1 do begin
      writeln(range[i].lo:12, range[i].hi:12);
   end;

   { this is a wrong answer for the big data }
   writeln;
   writeln('minimum and maximum are ...');
   writeln(mintyx:12, maxtyx:12);
   writeln;
   writeln('for a maximum of ', abs(mintyx) + abs(maxtyx));
   partone := abs(mintyx) + abs(maxtyx);
end;


{==========================================================================}
{ distance calculation and testing using manhattan (rectilinear) distances }
{                                                                          }
{ distance = abs(x1 - x2) + abs(y1 - y2)                                   }
{                                                                          }
{ does a sensor exclude any points along the target y line? yes if the     }
{ perpendicular distance abs(y1 - target y) is <= the sensor's beacon      }
{ range.                                                                   }
{==========================================================================}

function calcdistance(x1, y1, x2, y2 : integer) : integer;
begin
   calcdistance := abs(x1 - x2) + abs(y1 - y2);
end;


{========================================================================}
{ sort covered ranges                                                    }
{========================================================================}

procedure sortranges;

var
   i, j   : integer;
   should : boolean;

begin
   should := true;
   while should do begin
      should := false;
      for i := 0 to ranges-2 do begin
         if range[i].lo < range[i+1].lo then
            continue;
         if range[i].lo = range[i+1].lo then
            if range[i].hi <= range[i+1].hi then
               continue;
         should := true;
         j := range[i].lo; range[i].lo := range[i+1].lo; range[i+1].lo := j;
         j := range[i].hi; range[i].hi := range[i+1].hi; range[i+1].hi := j;
      end;
   end;
end;


{========================================================================}
{ find any ranges completely contained in another and squeeze them out   }
{========================================================================}

function collapseranges : boolean;

var
   i, j : integer;

begin

   collapseranges := false;
   for i := 0 to ranges - 1 do begin
      if range[i].lo = high(int64) then continue;
      for j := 0 to ranges - 1 do begin
         if j = i then continue;
         if range[j].lo = high(int64) then continue;
         if (range[i].lo >= range[j].lo) and (range[i].hi <= range[j].hi) then begin
            { i fits inside j }
            collapseranges := true;
            range[i].lo := high(int64); range[i].hi := high(int64);
            break;
         end;
      end;
   end;

   if collapseranges then begin
      sortranges;
      for i := 0 to high(range) do
         if range[i].lo = high(int64) then begin
            ranges := i;
            break;
         end;
   end;

end;


{========================================================================}
{ merge adjacent ranges                                                  }
{========================================================================}

function mergeranges : boolean;

var
   i : integer;

begin
   mergeranges := false;

   for i := 0 to ranges-1 do begin
      if range[i].lo = high(int64) then continue;
      if range[i+1].lo = high(int64) then continue;
      if range[i+1].lo <= range[i].hi then begin
         mergeranges := true;
         range[i].hi := range[i+1].hi;
         range[i+1].lo := high(int64);
         range[i+1].hi := high(int64);
      end;
   end;

   if mergeranges then begin
      sortranges;
      for i := 0 to high(range) do
         if range[i].lo = high(int64) then begin
            ranges := i;
            break;
         end;
   end;

end;



{========================================================================}
{ part two -- where is the beacon                                        }
{                                                                        }
{ we are told that the beacon's x and y coordinates can range from 0 to  }
{ 4000000. determine the beacon's tuning frequency (x * 4000000 + y).    }
{ there is only one grid coordinate that can hold the beacon, what is    }
{ its frequency?                                                         }
{                                                                        }
{ in the small test data, the location is x=14 y=11 for a frequency of   }
{ 56000011.                                                              }
{ a brute force scan is feasible but will take forever. i'm trying to    }
{ visualize finding gaps but it isn't coming clear to me yet. one idea   }
{ to play with is sampling, just to keep me in the problem space and see }
{ what i might learn.                                                    }
{                                                                        }
{ as i think about it, unlike circles the manhattan distance gives us    }
{ diamond shapes. so there won't be the same sort of gap shapes that     }
{ exist when working with cartesian coordinates. gaps will be along      }
{ parallel diagonal or cardinal lines.                                   }
{                                                                        }
{ i spent too much time trying to find a way to rotate the whole graph   }
{ so i could workd with rectangles instead of diamonds, but actually the }
{ diamond shape is fine. since we know there is only one point that is   }
{ not covered by the sensors, it must be on a line that runs along one   }
{ of the four edges of the coverage diamond at range+1.                  }
{                                                                        }
{ the lines all have slopes of |1| so it's easy to figure out the        }
{ intersections.                                                         }
{                                                                        }
{========================================================================}

function parttwo : integer;
var
   i    : integer;
   j    : integer;
   x, y : integer;

begin
   x := 0; y := 0;
   parttwo := -1;

   { determine the end points of the open lines. all the lines have a slope of |1|. }

   { n-e, w-s, w-n, s-e are the lines }
   { n-e checks against all w-n s-e }
   { w-s checks against all w-n and w-e }
   j := 0;
   for i := 0 to sensors - 1 do
      with sensor[i] do begin
         nx := x; ny := y - dist - 1;
         ex := x + dist + 1; ey := y;
         sx := x; sy := y + dist + 1;
         wx := x - dist - 1; wy := y;
         writeln;
         writeln('sensor ', i:2);
         writeln;
         writeln('  negative slope:');
         writeln('    line n-e : ', pointstr(nx, ny), '-', pointstr(ex, ey));
         writeln('    line w-s : ', pointstr(wx, wy), '-', pointstr(sx, sy));
         writeln('  positive slope:');
         writeln('    line s-e : ', pointstr(sx, sy), '-', pointstr(ex, ey));
         writeln('    line w-n : ', pointstr(wx, wy), '-', pointstr(nx, ny));
         intersection(x, y, nx, ny, ex, ey, wx, wy, nx, ny);
         writeln('intersection should be ', pointstr(nx, ny), ' and is ', pointstr(x, y));

      end;
   writeln;
end;


{========================================================================}
{ given points on lines a-b and c-d, find their interesection. find the  }
{ y=m*x+b equation (slope intercept form) for each line and then work    }
{ from those to find the x and y coordinates of their intersection. the  }
{ data in this problem gives us lines at 45 degrees from the axis (slope }
{ |1|) so calculations can all be done with integers. i lifted the code  }
{ from a more general solution on rosetta code that used floating point  }
{ and changed the data types.                                            }
{                                                                        }
{ there is no real error checking here. the data we are given is clean.  }
{ i do double (or triple) dispatch as needed to make sure that all the   }
{ segments are given from lower x to higher x.                           }
{========================================================================}

procedure slopeintercept(var m      : integer;
                         var b      : integer;
                             xa, ya,
                             xb, yb : integer);
begin
   m := (yb - ya) div (xb - xa);
   b := ya - xa * m;
end;

procedure intersection(var x              : integer;
                       var y              : integer;
                           xa, ya, xb, yb,
                           xc, yc, xd, yd : integer);

var
   mab, mcd : integer; { slopes }
   bab, bcd : integer; { intercepts }

begin
   if xa > xb then
      { lean consistently }
      intersection(x, y, xb, yb, xa, ya, xc, yc, xd, yd)
   else if xc > xd then
      { lean consistently }
      intersection(x, y, xa, ya, xb, yb, xd, yd, xc, yc)
   else begin
      { find m and b for y = m*x + b form }
      slopeintercept(mab, bab, xa, ya, xb, yb);
      slopeintercept(mcd, bcd, xc, yc, xd, yd);
      { find intersection }
      x := (bcd - bab) div (mab - mcd);
      y := ya - xa*mab + x*mab;
   end;
end;


{========================================================================}
{ display and format helpers                                             }
{========================================================================}

function base36(i : integer) : char;
begin
   if i > 36 then
      base36 := '?'
   else if i < 0 then
      base36 := '-'
   else if i < 10 then
      base36 := chr(ord('0') + i)
   else
      base36 := chr(ord('A') + i - 10);
end;

function pointstr(x, y : integer) : string;
begin
   pointstr := '(' + int2str(x) + ',' + int2str(y) + ')';
end;

{========================================================================}
{ the standard clear, load, and print helpers                            }
{========================================================================}

procedure init;

var
   i : integer;

begin
   for i := 0 to MAXDIM do begin
      with sensor[i] do begin
         x := 0; y := 0; bx := 0; by := 0; dist := 0; tydist := 0; tyminx := 0; tymaxx := 0;
      end;
      range[i].lo := 0; range[i].hi := 0;
   end;
   sensors := 0;
   ranges := 0;
   minx := high(int64); miny := high(int64);
   maxx := -high(int64); maxy := -high(int64);
   mintyx := high(int64); maxtyx := -high(int64);
   targety := -1;
end;


{========================================================================}
{ loads the sensor and beacon data and might also create a visualization }
{ grid if i need one.                                                    }
{                                                                        }
{ sample input:                                                          }
{                                                                        }
{ Sensor at x=2, y=18: closest beacon is at x=-2, y=15                   }
{ Sensor at x=9, y=16: closest beacon is at x=10, y=16                   }
{ Sensor at x=13, y=2: closest beacon is at x=15, y=3                    }
{ Sensor at x=12, y=14: closest beacon is at x=10, y=16                  }
{ Target y=10                                                            }
{========================================================================}

procedure load;
var
   i, p : integer;
   s    : string;

begin
   i := 0;
   while not aoceof do begin
      s := aocread;
      if not strbegins(s, 'Sensor at x=') then
         break;
      sensors := sensors + 1;
      with sensor[i] do begin
         p := 1;
         strindexpos(s, p, '=');
         p := p + 1;
         x := str2int(s, p);
         strindexpos(s, p, '=');
         p := p + 1;
         y := str2int(s, p);
         minx := min(minx, x); miny := min(miny, y);
         maxx := max(maxx, x); maxy := max(maxy, y);
         strindexpos(s, p, '=');
         p := p + 1;
         bx := str2int(s, p);
         strindexpos(s, p, '=');
         p := p + 1;
         by := str2int(s, p);
         minx := min(minx, bx); miny := min(miny, by);
         maxx := max(maxx, bx); maxy := max(maxy, by);
         dist := calcdistance(x, y, bx, by);
      end;
      i := i + 1;
   end;
   if strbegins(s, 'Target y=') then begin
      p := 1;
      strindexpos(s, p, '=');
      p := p + 1;
      targety := str2int(s, p);
   end;
   for i := 0 to sensors-1 do
      with sensor[i] do
         tydist := calcdistance(x, y, x, targety);
end;


procedure display;
var
   i : integer;

begin
   writeln;
   for i := 0 to sensors-1 do
      with sensor[i] do
         writeln('sensor ', i:2, ' at ', pointstr(x, y), ' has closest beacon at ', pointstr(bx, by), ' at a distance of ', dist, ' and a minimum distance from targety of ', tydist);
   writeln;
   writeln('coordinate ranges ', pointstr(minx, miny), ' to ', pointstr(maxx, maxy));
   writeln;
   writeln('target y line = ', targety);
   writeln;
end;

{ and just go to the real mainline }

begin
   mainline;
end.

{ end solution.pas }
