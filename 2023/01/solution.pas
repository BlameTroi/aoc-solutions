{ solution.pas -- trebuchet -- Troy Brumley BlameTroi@gmail.com }

program solution;

{ expects to be compiled with the following two mode flags and command
line/fpc.cfg options of -Ciort -Sgix -Sh- for full checks, allowing
exceptions, goto, inlining, and prefer classic strings. }
{$longstrings off}            { to be sure, use AnsiString or pchar explicitly }
{$booleval on}                { standard pascal expects this }

type
  integer = int64;            { by default ints are 64 bits this decade}


{ advent of code 2023 day 01 - you're getting launched from a trebuchet!

  you're getting launched at a target from a trebuchet, extract
  calibration values from some rather messy input. }


type

   { a processed line from the input dataset. all alphabetic characters
     are lower case in the original. }

   tPayload = record
                 digits : string;   { any and all digits from input }
                 value  : integer;  { calibration value from first digit and last digit }
                 first  : string;   { as found allowing for words }
                 last   : string;
                 wordy  : integer;  { value allowing for text digits }
              end;

   { the input dataset for the days' problem, one per line of input. the
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


var

  textualdigits : array[0 .. 9] of string;


{$I aocinput.p }
{$I substr.p }
{$I strbegins.p }


{ initialize global storage }

procedure init;

begin
   textualdigits[0] := 'zero';
   textualdigits[1] := 'one';
   textualdigits[2] := 'two';
   textualdigits[3] := 'three';
   textualdigits[4] := 'four';
   textualdigits[5] := 'five';
   textualdigits[6] := 'six';
   textualdigits[7] := 'seven';
   textualdigits[8] := 'eight';
   textualdigits[9] := 'nine';
end;


{ extract decimal digits from a string. }

function onlydigits(s : string) : string;

var
   i : integer;
   d : string;

begin
   d := '';
   for i := 1 to length(s) do
      if s[i] in ['0' .. '9'] then
         d := d + s[i];
   onlydigits := d
end;


{ parse for part one of the problem is to extract all the digit
  characters in order from the string and take the first digit
  as tens digit and the last digit as the units value. note that
  a digit can be both the first and the last digit in the string. }

procedure parseone(    rec    : string;
                   var digits : string;
                   var value  : integer);

begin
   digits := onlydigits(rec);
   value :=
   ((ord(digits[1]) - ord('0')) * 10) +
   (ord(digits[length(digits)]) - ord('0'))
end;


{ parse for part two introduces the wrinkle that digits could be
  spelled out (and overlap as in fiveight) in a string. otherwise
  it's the same idea as part one: value is first * 10 + last. }

procedure parsetwo(    s     : string;
                   var first : string;
                   var last  : string;
                   var wordy : integer);
var
   i, j, v : integer;
   t       : string;

begin
   first := '';
   last := '';
   wordy := -1;
   i := 0;
   while (i < length(s)) and (first = '') do begin
      i := i + 1;
      if s[i] in ['0'..'9'] then begin
         first := s[i];
         v := ord(s[i]) - ord('0');
         break
      end; { in 0..9 }
      t := substr(s, i, length(s));
      for j := 0 to 9 do
         if strbegins(t, textualdigits[j]) then begin
            first := textualdigits[j];
            v := j;
            break
         end { if strbegins }
   end; { while hunting first }
   wordy := v * 10;
   while i < length(s) do begin
      i := i + 1;
      if s[i] in ['0'..'9'] then begin
         last := s[i];
         v := ord(s[i]) - ord('0');
         continue
      end; { in 0..9 }
      t := substr(s, i, length(s));
      for j := 0 to 9 do
         if strbegins(t, textualdigits[j]) then begin
            last := textualdigits[j];
            v := j;
            break
         end { if strbegins }
   end; { while hunting last }
   { if no other digit found, use first again }
   if last = '' then
      last := first;
   wordy := wordy + v;
end;


{ load the dataset into memory and perform the parses for both part
  one and two. they are straightforward and are easiest to do here. }

function load : pData;

var
   first, prior, curr : pData;
   i                  : integer;
   s                  : string;

begin
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
      with curr^.pld do begin
         parseone(curr^.rec, digits, value);
         parsetwo(curr^.rec, first, last, wordy);
      end;
      if first = nil then
         first := curr
      else
         prior^.fwd := curr;
      prior := curr
   end; { while not eof }
   load := first;
end;


{ take the value of when the first and last digit of an input record
  are viewed as a number. sum these values. }

function partone(pd : pData) : integer;

begin
   partone := 0;
   while pd <> nil do
      with pd^.pld do begin
         partone := partone + value;
         pd := pd^.fwd
      end; { with pd^ }
end;


{ digits are also character strings (two vs 2) so while the objective
  is to find the first and last digits of an input record, a different
  approach is needed.

  as textual digits can overlap (nineight) the question arises 'is that
  nine, eight, or nine and eight'. this is not specified in the problem
  statement but since only the first and last are needed, we can take
  from either end and ignore the middle. }

function parttwo(pd : pData) : integer;

begin
   parttwo := 0;
   while pd <> nil do
      with pd^.pld do begin
         parttwo := parttwo + wordy;
         pd := pd^.fwd
      end; { with pd^ }
end;


{ mainline }

var
   p1answer, p2answer : integer;
   dataset            : pData;

begin
   aocopen;
   init;
   dataset := load;
   p1answer := partone(dataset);
   writeln('part one answer : ', p1answer);
   p2answer := parttwo(dataset);
   writeln('part two answer : ', p2answer);
   aocclose;
end.

