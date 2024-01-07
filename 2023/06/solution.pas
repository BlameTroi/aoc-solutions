{ solution.pas -- wait for it  -- Troy Brumley BlameTroi@gmail.com }

{ starting shell for a solution to an advent of code problem }

program solution;

{ expects to be compiled with the following two mode flags and command
line/fpc.cfg options of -Ciort -Sgix -Sh- for full checks, allowing
exceptions, goto, inlining, and prefer classic strings. }
{$longstrings off}            { to be sure, use AnsiString or pchar explicitly }
{$booleval on}                { standard pascal expects this }

type
  integer = int64;            { by default ints are 64 bits this decade}


{ advent of code 2023 day 6 -- wait for it

  the goal is to determine how many outcomes of a 'race' exceed a
  particular distance. given the nature of the problem and data,
  simple arrays are appropriate data structures.

  The formula for part one is a quadratic equation and the solution
  can be found using forgotten albebra but the data is small enough to
  iterate so part one will be done that way.

  for part two the iteration count gets rather large, but the memory
  and processing time all fit within my constraints. }


const
{$define xxxTESTING }
{$ifdef TESTING }
   MAXRACES = 3;
   MAXTIMES = 71530;
{$else}
   MAXRACES = 4;
   MAXTIMES = 56977793;
{$endif}
   CTIME = 'Time:';
   CDISTANCE = 'Distance:';
{$I constchars.p }


{$I str2int.p }
{$I strbegins.p }
{$I strskip.p }
{$I aocinput.p }


{ the data and problem are small enough to use globals. }
var
  times     : array[1 .. MAXRACES] of integer;
  distances : array[1 .. MAXRACES] of integer;
  outcomes  : array[1 .. MAXRACES] of array[0..MAXTIMES] of integer;
  options   : array[1 .. MAXRACES] of integer;
  numraces  : integer;


{ initialize global storage }
procedure init;

var
  i, j : integer;

begin
   for i := low(times) to high(times) do begin
      times[i] := 0;
      distances[i] := 0;
      for j := low(outcomes[i]) to high(outcomes[i]) do
         outcomes[i, j] := 0;
      options[i] := 0;
   end;
   numraces := 0;
end;


{ read race times and distances. these come on two separate records
  so times first then distance. part one and two have different
  interpretations of the data so there is a separate loader for
  each part. }

procedure loadone;

var
   i, p : integer;
   s    : string;

begin
   s := aocread;
   if not strbegins(s, CTIME) then
      aochalt('invalid input, missing ' + CTIME);
   p := length(CTIME) + 1; { position to first space }
   i := 0;
   while p < length(s) do begin
      i := i + 1;
      p := strskip(s, p, SPACE);
      times[i] := str2int(s, p);
   end;
   s := aocread;
   if not strbegins(s, CDISTANCE) then
      aochalt('invalid input, missing ' + CDISTANCE);
   p := length(CDISTANCE) + 1;
   i := 0;
   while p < length(s) do begin
      i := i + 1;
      p := strskip(s, p, SPACE);
      distances[i] := str2int(s, p);
   end;
   numraces := i;
end;


{ for part one, calculate distances travelled give the wait and
  velocity rules. count the number of these distances that exceed
  the target distance from input. }

function churn(r : integer) : integer;

var
   w : integer;

begin
   churn := 0;
   for w := 0 to times[r] do begin
      outcomes[r][w] := (times[r] * w) - w * w;
      if outcomes[r][w] > distances[r] then
         options[r] := options[r] + 1;
   end;
   churn := options[r];
end;


{ read race times and distances with the view of the data for
  part two: that the spaces are bogus and there is only one
  race. }

procedure loadtwo;

var
   p    : integer;
   s, t : string;

begin
   s := aocread;
   if not strbegins(s, CTIME) then
      aochalt('invalid input, missing ' + CTIME);
   p := length(CTIME) + 1; { position to first space }
   p := strskip(s, p, SPACE); { and then first digit }
   t := '';
   while p <= length(s) do begin
      if s[p] in ['0' .. '9'] then
         t := t + s[p];
      p := p + 1;
   end;
   p := 1;
   times[1] := str2int(t, p);
   s := aocread;
   if not strbegins(s, CDISTANCE) then
      aochalt('invalid input, missing ' + CDISTANCE);
   p := length(CDISTANCE) + 1;
   p := strskip(s, p, SPACE); { and then first digit }
   t := '';
   while p <= length(s) do begin
      if s[p] in ['0' .. '9'] then
         t := t + s[p];
      p := p + 1;
   end;
   p := 1;
   distances[1] := str2int(t, p);
   numraces := 1;
end;


{ for each race, how many wait times produce greater distances than
  the record from the input? multiply those counts as the answer for
  part one. }

function partone : integer;

var
   r : integer;

begin
   init;
   aocrewind;
   loadone;
   partone := 1;
   for r := 1 to numraces do
      partone := partone * churn(r);
end;


{ for part two, it turns out there is only one race and record. the
  spaces between the input are typos. solving for this new single race
  will involve so many iterations that i'll have to remember how to
  find roots of a quadriatic equation. but first, the parabola is
  regular and at right angles to the x axis so i'll fix partone to
  do only half the iterations. }

{ ok, so with 64 bit integers and current hardware, iteration
  is fast enough on yesterday's problem and today's.

  leaving the better algorithm work for another time. }

function parttwo : integer;

begin
   init;
   aocrewind;
   loadtwo;
   parttwo := churn(1);
end;


{ mainline
}
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
