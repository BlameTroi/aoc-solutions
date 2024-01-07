{ solution.pas -- mirage maintenance -- Troy Brumley BlameTroi@gmail.com }


program solution;

{ expects to be compiled with the following two mode flags and command
line/fpc.cfg options of -Ciort -Sgix -Sh- for full checks, allowing
exceptions, goto, inlining, and prefer classic strings. }
{$longstrings off}            { to be sure, use AnsiString or pchar explicitly }
{$booleval on}                { standard pascal expects this }

type
  integer = int64;            { by default ints are 64 bits this decade}

{ advent of code 2023 day 9 -- mirage maintenance

  extrapolation of sensor readings using a pascal's triangle looking
  calculation. for part one extrapolate forward, and for part two
  extrapolate backward. }

const
{$define xxxTESTING }
{$define TESTONE }
{$ifdef TESTING }
   MAXSENSORS = 3;
   MAXREADINGS = 6;
{$else}
   MAXSENSORS = 200;
   MAXREADINGS = 21;
{$endif}
{$I constchars.p }


{ ---------------------------------------------------------
  global variables are appropriate for this problem. note
  that while sensors number from 1, the logs number from 0.
  the actual last slot is reserved for the new predicted
  reading.
  --------------------------------------------------------- }

var
  dataset    : array[1 .. MAXSENSORS] of string;
  sensorlogs : array[1 .. MAXSENSORS, 0 .. MAXREADINGS] of integer;
  scratchpad : array[0 .. MAXREADINGS, 0 .. MAXREADINGS] of integer;
  depthtrack : array[0 .. MAXREADINGS] of integer;

{ ---------------------------------------------------------
  utilitiy
  --------------------------------------------------------- }

{$I str2int.p }


{ zero the scratch pad. }

procedure zeroscratchpad;

var
   i, j : integer;

begin
   for i := low(scratchpad) to high(scratchpad) do
      for j := low(scratchpad[i]) to high(scratchpad[i]) do
         scratchpad[i, j] := 0;
end;


{ initialize global variables. }

procedure init;

var
   i, j : integer;

begin
   for i := low(dataset) to high(dataset) do
      dataset[i] := '';
   for i := low(sensorlogs) to high(sensorlogs) do
      for j := low(sensorlogs[i]) to high(sensorlogs[i]) do
         sensorlogs[i, j] := 0;
   for i := low(depthtrack) to high(depthtrack) do
      depthtrack[i] := 0;
   zeroscratchpad;
end;


{ is the whole row zero? and don't be cute! }

function scratchrowzero(r : integer) : boolean;

var
   j : integer;

begin
   scratchrowzero := false;
   for j := low(scratchpad[r]) to high(scratchpad[r]) do
      if scratchpad[r, j] <> 0 then
         exit;
   scratchrowzero := true;
end;


{$I aocinput.p }


{ ---------------------------------------------------------
  loader and support.
  --------------------------------------------------------- }

{ read the sensor logs and parse them into the matrix. }

procedure loadone;

var
   i, j : integer;
   p    : integer;

begin
   init;
   aocrewind;
   i := 0;
   while not aoceof do begin
      i := i + 1;
      dataset[i] := aocread;
      p := 1;
      j := 0;   { remember, this starts at 0 not 1 }
      while p < length(dataset[i]) do begin
         while dataset[i][p] = SPACE do
            p := p + 1;
         sensorlogs[i, j] := str2int(dataset[i], p);
         j := j + 1;
      end;
   end;
end;


{ loads the data in backwards }
procedure loadtwo;

var
   i, j : integer;
   p    : integer;

begin
   init;
   aocrewind;
   i := 0;
   while not aoceof do begin
      i := i + 1;
      dataset[i] := aocread;
      p := 1;
      { switch directions on load for part two }
      j := MAXREADINGS - 1;
      while p < length(dataset[i]) do begin
         while dataset[i][p] = SPACE do
            p := p + 1;
         sensorlogs[i, j] := str2int(dataset[i], p);
         j := j - 1;
      end;
   end;
end;


{ determine the next figure in the series by using the differences of
  the prior readings. the extrapolation uses a pascal's triangle like
  approach to the extrapolation. }

function extrapolate(s : integer): integer; { sensor number to process }

var
   i, j : integer;

begin
   { initialize scratchpad for current sensor. }
   zeroscratchpad;
   for j := low(scratchpad[0]) to high(scratchpad[0]) do
      scratchpad[0][j] := sensorlogs[s, j];

   { calculate differences, always looks back to prior line }
   { each row (i) needs one fewer column than the one above.
     the last column in the sensor log is 0 and that's the
     value we need to replace. }
   i := low(scratchpad); { sensor log }
   while i < high(scratchpad) do begin
      if scratchrowzero(i) then begin
         depthtrack[i] := depthtrack[i] + 1;
         //writeln(output, 'differences completed at ', i);
         break;
      end;
      i := i + 1;
      for j := low(scratchpad[i]) to high(scratchpad[i]) - i - 1 do
         scratchpad[i, j] := scratchpad[i-1, j+1] - scratchpad[i-1, j];
   end;
   { now work back up the pad replacing the zero difference at the
     end with the correct value. current row's tail + prior row's
     tail. }
   repeat
      j := high(scratchpad[i]) - i;
      scratchpad[i-1, j+1] := scratchpad[i, j] + scratchpad[i-1, j];
      i := i - 1;
   until i = low(scratchpad);
   extrapolate := scratchpad[low(scratchpad), high(scratchpad[0])];
end;


{ to check our work, extrapolate the next reading for each
  sensor and report the sum of the extrapolations. }

function partone : integer;
var
   i : integer;

begin
   loadone;
   for i := 1 to high(sensorlogs) do
      sensorlogs[i, high(sensorlogs[i])] := extrapolate(i);
   partone := 0;
   for i := 1 to high(sensorlogs) do
      partone := partone + sensorlogs[i, high(sensorlogs[i])];
end;


{ part two is the same as part one, but the loader inverts
  the data so we are actually extrapolating backward. }

function parttwo : integer;

var
   i : integer;

begin
   loadtwo;
   for i := 1 to high(sensorlogs) do
      sensorlogs[i, high(sensorlogs[i])] := extrapolate(i);
   parttwo := 0;
   for i := 1 to high(sensorlogs) do
      parttwo := parttwo + sensorlogs[i, high(sensorlogs[i])];
end;


{ mainline }

var
   i : integer;

begin
   init;
   aocopen;
   i := partone;
   writeln('part one: ', i);
   i := parttwo;
   writeln('part two: ', i);
   aocclose;
end.

