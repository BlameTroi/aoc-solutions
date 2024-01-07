{ solution.pas -- rope bridge -- Troy Brumley blametroi@gmail.com }

program solution;
{$longstrings off}
{$booleval on}


{ advent of code 2022 day 09 -- rope bridge

  the rope bride has broken, you need to grab the rope, but where will
  it be? simulate the path of the tail of the rope as it swings in the
  wind. }


type
   integer = int64;

const
   MAXCOORD = 250;            { observed max plus some pad }
   MAXTAILS = 9;              { how many knots after head? }
   X        = 0;              { subscript into coords }
   Y        = 1;              { subscript into coords }


{$I str2int.p }
{$I min.p }
{$I max.p }
{$I aocinput.p }


{ global variables are fine for this }

var
   { the actual minimums and maximums visited, this clips the
     reporting region but really isn't needed. }
   maxx, minx : integer;
   maxy, miny : integer;
   { subscripts of the head and the final tail in coords and the
     visitgrid arrays. }
   head       : integer;
   tail       : integer;
   { these are the coordinates of the head and trailing knots at the
     current time. }
   coords     : array[0..MAXTAILS, X..Y] of integer;
   { a history track for each of the knots counting how many times
     it visited a coordinate. }
   visitgrid  : array[0..MAXTAILS, -MAXCOORD..MAXCOORD, -MAXCOORD..MAXCOORD] of integer;


{ clear out storage }

procedure init;

var
   i, j, k : integer;

begin
   maxx := 0; minx := 0;
   maxy := 0; miny := 0;
   head := 0;                  { we track head with this }
   tail := 1;                  { placeholder }
   for i := 0 to MAXTAILS do begin
      coords[i, X] := 0;
      coords[i, Y] := 0;
      for j := -MAXCOORD to MAXCOORD do
         for k := -MAXCOORD to MAXCOORD do
            visitgrid[i, j, k] := 0;
   end;
   for i := 0 to MAXTAILS do
      visitgrid[i, 0, 0] := 1;
end;


{ rules of the tail:

  if head is ever 2 steps nsew from tail, tail moves 1 step nsew
  otherwise

  if head and tail aren't touching and aren't in same row or col
  the tail will move one step diagonally to keep up

  part one simulated a short rope, but for part two we are given a
  longer rope with which to hang ourselves. here 9 knots follow the
  head knot. 1 follows h, 2 follows 1, etc. the knot moves are
  'simultaneous' with the head moves. switch the data structure for
  the knots to an array 0..9 of coordinates x,y. }


{ are we too far away from our leader? }

function mustmove(k : integer) : boolean;

begin
   mustmove := (abs(coords[k-1, X] - coords[k, X]) > 1) or (abs(coords[k-1, Y] - coords[k, Y]) > 1);
end;


{ move the specified tail if movement is required }

procedure tailmove(k : integer);

var
   dx, dy : integer;
   mx, my : integer;

begin
   dx := coords[k-1, X] - coords[k, X];
   dy := coords[k-1, Y] - coords[k, Y];

   { on same x, move 1 closer y }
   if dx = 0 then begin
      if dy < 0 then
         coords[k, Y] := coords[k, Y] - 1
      else
         coords[k, Y] := coords[k, Y] + 1;
      exit;
   end;

   { on same y, move 1 closer x }
   if dy = 0 then begin
      if dx < 0 then
         coords[k, X] := coords[k, X] - 1
      else
         coords[k, X] := coords[k, X] + 1;
      exit;
   end;

   { not on same x or y, move 1 diagonally closer }
   if dy < 0 then
      my := -1
   else
      my := 1;
   if dx < 0 then
      mx := -1
   else
      mx := 1;
   coords[k, X] := coords[k, X] + mx;
   coords[k, Y] := coords[k, Y] + my;
end;


{ how many grid coordinates are visited by the tail as it follows the
  head around the plane. for part one the tail is one 'knot' behind
  the tail, and in part two it is nine knots behind. in each case, the
  trialing knot follows the knot immediately before it. our input is
  the movement of the head knot. }

function chaser(tails : integer) : integer;

var
   move : char;
   dist : integer;
   i, j : integer;
   k    : integer;
   p    : integer;
   s    : string;
   done : boolean;

begin
   init;
   aocrewind;
   head := 0;
   tail := tails;
   while not aoceof do begin
      s := aocread;
      if s = '' then continue;
      if s[1] = '#' then continue;
      { <M> <count> }
      move := s[1];
      p := 3;
      dist := str2int(s, p);
      case move of
        'U' : coords[head, Y] := coords[head, Y] + dist;
        'D' : coords[head, Y] := coords[head, Y] - dist;
        'R' : coords[head, X] := coords[head, X] + dist;
        'L' : coords[head, X] := coords[head, X] - dist;
      end;
      minx := min(minx, coords[head, X]);
      maxx := max(maxx, coords[head, X]);
      miny := min(miny, coords[head, Y]);
      maxy := max(maxy, coords[head, Y]);
      visitgrid[head, coords[head, X], coords[head, Y]] := visitgrid[head, coords[head, X], coords[head, Y]] + 1;
      done := false;
      while not done do begin
         done := true;
         for k := 1 to tails do
            if mustmove(k) then begin
               done := false;
               tailmove(k);
               visitgrid[k, coords[k, X], coords[k, Y]] := visitgrid[k, coords[k, X], coords[k, Y]] + 1;
            end;
      end;
   end;
   chaser := 0;
   for i := minx to maxx do
      for j := miny to maxy do
         if visitgrid[tail, i, j] <> 0 then
            chaser := chaser + 1;
end;


{ the mainline driver for a day's solution. }

procedure mainline;

var
   i : integer;

begin
   aocopen;
   i := chaser(1);
   writeln('part one answer : ', i);
   i := chaser(9);
   writeln('part two answer : ', i);
   aocclose;
end;

{ minimal boot to mainline }

begin
  mainline;
end.
