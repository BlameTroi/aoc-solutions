{ solution.pas -- treetop tree house -- Troy Brumley blametroi@gmail.com }

program solution;
{$longstrings off}
{$booleval on}


{ advent of code 2022 day 08 treetop tree house. given an 'orchard'
  of trees of varying heights, count trees based on their visibility
  or the quality of their scenic views. }


type
   integer = int64;

const
   MAXSIDE = 99;            { observed max }

{$I max.p }
{$I aocinput.p }


{ global variables are fine for this. the orchard is mapped into an
  array with some padding to simplify checking edge cases. }
var
   orchard    : array[0..MAXSIDE+1, 0..MAXSIDE+1] of char;
   sidelen    : integer;
   numvisible : integer;


{ initialize global variables to empty. orchard has extra rows
  and columns around the border to simplify visibility check. }

procedure init;

var
  i, j : integer;

begin
   for i := 0 to MAXSIDE+1 do
      for j := 0 to MAXSIDE+1 do
         orchard[i, j] := chr(0);
   sidelen := 0;
   numvisible := 0;
end;


{ check to see if a tree is visible from the four cardinal directions. }

function visible(r : integer;
                 c : integer) : boolean;
var
   i, j       : integer;
   t          : char;
   directions : integer;

begin
   directions := 4;
   t := orchard[r, c];
   for i := 0 to r-1 do
      if orchard[i, c] >= t then begin
         directions := directions - 1;
         break;
      end;
   { optimization here would be if was visible then done }
   for i := sidelen+1 downto r+1 do
      if orchard[i, c] >= t then begin
         directions := directions - 1;
         break;
      end;
   for j := 0 to c-1 do
      if orchard[r, j] >= t then begin
         directions := directions - 1;
         break;
      end;
   for j := sidelen+1 downto c+1 do
      if orchard[r, j] >= t then begin
         directions := directions - 1;
         break;
      end;
   visible := directions > 0;
end;


{ for part two, calculate the scenic score as how many trees can be
  seen before the view is blocked by a tree. trees on the edges are
  immediately blocked at one. multiply the number of trees in each
  direction, what is the highest scenic score? }

function scenicscore(r : integer;
                     c : integer) : integer;
var
   i, j   : integer;
   sn, ss,
   se, sw : integer;
   t      : char;

begin
   t := orchard[r, c];
   sn := 0; ss := 0; se := 0; sw := 0;
   for i := r-1 downto 1 do begin
      sn := sn + 1;
      if orchard[i, c] >= t then
         break;
   end;
   for i := r+1 to sidelen do begin
      ss := ss + 1;
      if orchard[i, c] >= t then
         break;
   end;
   for j := c-1 downto 1 do begin
      sw := sw + 1;
      if orchard[r, j] >= t then
         break;
   end;
   for j := c+1 to sidelen do begin
      se := se + 1;
      if orchard[r, j] >= t then
         break;
   end;
   scenicscore := sn * ss * se * sw;
end;


{ load the orchard and then look for visible trees. }

function partone : integer;

var
   i, j : integer;

begin
   init;
   aocrewind;
   i := 1; j := 1;

   while not eof(input) do begin
      if eoln(input) then begin
         readln(input);
         i := i + 1; j := 1;
         continue;
      end;
      read(input, orchard[i, j]);
      j := j + 1;
   end;

   sidelen := i - 1;
   numvisible := 0;
   for i := 1 to sidelen do
      for j := 1 to sidelen do
         if visible(i, j) then
            numvisible := numvisible + 1;
   partone := numvisible;
end;


{ part one loaded the orchard, scan to find the highest possible
  'scenic score'. knowing that trees on the sides have at least one
  scenic score of 0, there's no need to process them. }

function parttwo : integer;

var
   i, j : integer;
   s    : integer;

begin
   s := 0;
   for i := 2 to sidelen-1 do
      for j := 2 to sidelen-1 do
         s := max(s, scenicscore(i, j));
   parttwo := s;
end;


{ the mainline driver for a day's solution. }

procedure mainline;

var
   i : integer;

begin
   aocopen;
   init;
   i := partone;
   writeln('part one answer : ', i);
   i := parttwo;
   writeln('part two answer : ', i);
   aocclose;
end;


{ minimal boot to mainline }

begin
  mainline;
end.
