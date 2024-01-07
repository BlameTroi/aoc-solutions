{ loaditer.pas -- dataset loader and iterator -- Troy Brumley blametroi@gmail.com }
program loaditer;

{ expects to be compiled with the following two mode flags and command
line/fpc.cfg options of -Ciort -Sgix -Sh- for full checks, allowing
exceptions, goto, inlining, and prefer classic strings. }
{$longstrings off}            { to be sure, use AnsiString or pchar explicitly }
{$booleval on}                { standard pascal expects this }

type
  integer = int64;            { by default ints are 64 bits this decade}

type
   { a processed line from the input dataset. }
   tPayload = record
                 something : string;   { fill this in }
              end;
   { the input dataset for the days problem, one per line of input.
     the original input line and its parsed value(s). blank lines are
     maintained for control break purposes. }
   pData = ^tData;
   tData = record
              fwd,              { a singly linked list would be }
              bwd : pData;      { enough, but you never know }
              num : integer;    { line number in source }
              rec : string;     { original input }
              pld : tPayload;   { what does it mean stimpy? }
           end;


{ the iterator for determining the answer to a day's problem. these
  are often in two parts, but the frame's the same. as a function
  since (so far) the answers have always been integer totals.

  this function isn't meant to be executed itself, it's the copy
  and paste mine for partone and parttwo below. }

function iter(pd : pData) : integer;

var
   answer, i : integer;

begin
   answer := 0;
   i := 0;

   while pd <> nil do
      with pd^.pld do begin
         i := i + 1;
         if pd^.rec = '' then begin
            pd := pd^.fwd;
            continue
         end;
         pd := pd^.fwd
      end; { with pd^ }

   iter := answer;
end;


{ load the dataset into memory. since dynamic arrays aren't standard
  across pascal implementations, and i dislike the implementation in
  free pascal, a doubly linked list is used. it's certainly overkill
  in terms of flexibility, and wouldn't scale well with large
  datasets, but for advent of code a linear search on post 2010
  hardware will be more than fast enough. }

function load : pData;

var
   first,
   prior,
   curr   : pData;
   i      : integer;
   s      : string;

begin

   first := nil;
   prior := nil;
   curr := nil;
   i := 0;
   while not eof(input) do begin
      i := i + 1;
      readln(input, s);
      new(curr);
      curr^.fwd := nil;
      curr^.bwd := prior;
      curr^.num := i;
      curr^.rec := s;
      if first = nil then
         first := curr
      else
         prior^.fwd := curr;
      prior := curr
   end; { while not eof }

   load := first;
end;


{ hook point for the first part of a day's problem. so far each
  day has had two questions to answer, and each answer has been
  an integer.

  the framework in iter goes here and then updated to find an
  answer. }

function partone(pd : pData) : integer;

begin
   partone := 0;
end;


{ see partone }

function parttwo(pd : pData) : integer;

begin
   parttwo := 0;
end;


{ the mainline driver for a day's solution. load and then
  iterate and display results. input and output are fixed
  named files. }

procedure mainline;

var
   dataset  : pData;
   s        : string;
   p1answer,
   p2answer : integer;

begin
   aocopen;

   dataset := load;
   p1answer := partone(dataset);
   p2answer := parttwo(dataset);

   writeln('part one answer : ', p1answer);
   writeln('part two answer : ', p2answer);

   aocclose;
end;


{ minimal boot to mainline }
begin
   mainline;
end.
