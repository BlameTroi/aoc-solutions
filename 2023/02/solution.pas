{ solution.pas -- cube colors  -- Troy Brumley BlameTroi@gmail.com }

program solution;

{ expects to be compiled with the following two mode flags and command
line/fpc.cfg options of -Ciort -Sgix -Sh- for full checks, allowing
exceptions, goto, inlining, and prefer classic strings. }
{$longstrings off}            { to be sure, use AnsiString or pchar explicitly }
{$booleval on}                { standard pascal expects this }

type
   integer = int64;            { by default ints are 64 bits this decade}

{ advent of code 2023 day 02 -- cube conundrum
  drawing colored cubes from a bag, how many draws can be made.

  input format:

  game #: sample (;sample)*

  sample is # color (, # color)*
  color is red | blue | green
  # is an integer }


const
   MAXSAMPLES = 10;


type

   tSample = record                { record of cubes drawn }
                red   : integer;   { counted by color      }
                green : integer;
                blue  : integer;
             end;

{ a processed line from the input dataset. }

   tPayload = record
                 original : string;    { see input desc above }
                 gameno   : integer;   { unique id }
                 count    : integer;   { of samples }
                 samples  : array [1 .. MAXSAMPLES] of tSample;
              end;

{ the input dataset for the days problem, one per line of input. the
  original input line and its parsed value(s). blank lines are
  maintained for control break purposes. }

   pData = ^tData;
   tData = record
              fwd,              { a singly linked list would be }
              bwd : pData;      { enough, but you never know }
              num : integer;    { line number in source }
              rec : string;     { original input }
              pld : tPayload;   { what does it mean stimpy? }
           end;


{$I aocinput.p }
{$I max.p }
{$I str2int.p }


{ parse the input record (see description above) into a payload for part one. }

procedure parseone(var pld : tPayload);

var
   i, p : integer;

begin
   with pld do begin
      for i := 1 to MAXSAMPLES do
         with samples[i] do begin
            red := 0;
            blue := 0;
            green := 0
         end; {with samples[i] }
      p := 6;                  { position after 'game ' }
      gameno := str2int(original, p);
      count := 0;
      while (p <= length(original)) and (count <= MAXSAMPLES) and (original[p] in [';', ':']) do begin
         count := count + 1;    { next sample slot }
         repeat
            p := p + 2;          { skip [:;]\s }
            i := str2int(original, p); { get number cubes }
            p := p + 1;          { skip to color }
            case original[p] of
              'r' : begin
                 samples[count].red := i;
                 p := p + 3; { skip ' red' }
              end; { 'r' }
              'g' : begin
                 samples[count].green := i;
                 p := p + 5; { skip 'green' }
              end;
              'b' : begin
                 samples[count].blue := i;
                 p := p + 4; {skip 'blue' }
              end;
            end; { case }
         until original[p] <> ',';
      end { while pos/count/check delimiter }
   end
end;


{ load and parse the problem dataset }

function load : pData;

var
   first, prior, curr : pData;
   i                  : integer;
   s                  : string;

begin
   aocrewind;
   first := nil;
   prior := nil;
   curr := nil;
   i := 0;
   while not aoceof do begin
      i := i + 1;
      s := aocread;
      new(curr);
      curr^.fwd := nil;
      curr^.bwd := prior;
      curr^.num := i;
      curr^.rec := s;
      curr^.pld.original := s;
      parseone(curr^.pld);
      if first = nil then
         first := curr
      else
         prior^.fwd := curr;
      prior := curr
   end; { while not eof }
   load := first;
end;


{ for part one, the game draws of input are checked to see if they
  would be possible from a bag loaded with only 12 red cubes, 13
  green cubes, and 14 blue cubes. The answer for part one is the
  sum of the game ids of possible games. }

function partone(pd : pData) : integer;

var
   bag  : tSample;
   i, j : integer;

begin
   partone := 0;
   bag.red := 12;    { sample for part one test }
   bag.green := 13;
   bag.blue := 14;
   i := 0;
   while pd <> nil do
      with pd^.pld do begin
         i := i + 1;
         partone := partone + gameno;   { assume this passes }
         for j := 1 to count do
            if (bag.red < samples[j].red) or
               (bag.green < samples[j].green) or
               (bag.blue < samples[j].blue) then begin
                  partone := partone - gameno; { assumption wrong }
                  break;
               end;
         pd := pd^.fwd
      end; { with pd^ }
end;


{ for part two, determine the least number of cubes can satisfy a
  game's draws. for this set, multiply the number of cubes together,
  and return the sum. }

function parttwo(pd : pData) : integer;

var
   bag         : tSample;
   power, i, j : integer;

begin
   parttwo := 0;
   i := 0;
   while pd <> nil do
      with pd^.pld do begin
         i := i + 1;
         bag.red := 0;       { initialize for min }
         bag.green := 0;
         bag.blue := 0;
         for j := 1 to count do begin
            bag.red := max(bag.red, samples[j].red);
            bag.green := max(bag.green, samples[j].green);
            bag.blue := max(bag.blue, samples[j].blue);
         end;
         power := bag.red * bag.green * bag.blue;
         parttwo := parttwo + power;
         pd := pd^.fwd
      end; { with pd^ }
end;


{ mainline }

var
   i       : integer;
   dataset : pData;

begin
   aocopen;
   dataset := load;
   i := partone(dataset);
   writeln('part one: ', i);
   i := parttwo(dataset);
   writeln('part two: ', i);
   aocclose;
end.
