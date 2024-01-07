{ solution.pas -- gear ratios -- Troy Brumley BlameTroi@gmail.com }

program solution;

{ expects to be compiled with the following two mode flags and command
line/fpc.cfg options of -Ciort -Sgix -Sh- for full checks, allowing
exceptions, goto, inlining, and prefer classic strings. }

{$longstrings off}            { to be sure, use AnsiString or pchar explicitly }
{$booleval on}                { standard pascal expects this }

type
   integer = int64;            { by default ints are 64 bits this decade}


{ advent of code 2023 day 03 -- 'gear ratios'.

  determine gear ratios from a jumbled schematic. for part one sum the
  part numbers found. for part two, identify gears (symbol '*' with
  qualifications) and then its gear ratio (adjacent part numbers
  multiplied). }


const
   { the input grid is square, so no need to differentiate between row
     and column in the limits. padding around the edges for simplicity
     in coding. }
   GRIDMIN = 0;
   GRIDBEG = 1;
   GRIDEND = 140;
   GRIDMAX = 141;

   { on the grid digits and symbols are surrounded by empty space. a
     symbol is anything not a digit or empty. by examination the hard
     coded set includes all symbols. }
   DIGITS = ['0' .. '9'];
   EMPTY = '.';
   SYMBOLS = ['#', '$', '%', '&', '*', '+', '/', '-', '@', '='];

   { for part two gears '*' are important. any '*' that is adjacent
     to exactly two part numbers is a gear. not one, not three, two. }
   GEAR = '*';

   { by examination there are ~1250 numbers found on the grid without
     considering if they are part numbers, and the largest run of
     digits appears to be 3, so i came up with the following
     estimates. }
   MAXNUMBERS = 1500;
   MAXDIGITS = 4;

   { a quick frequency count showed ~370 '*' characters so here's
     a good estimate. }
   MAXGEARS = 500;


type
   { our grid is just characters, a lot of characters }
   tGrid = array[GRIDMIN .. GRIDMAX, GRIDMIN .. GRIDMAX] of char;

   { a possible part number on the grid. }
   tNumber = record
                row,
                col : GRIDMIN .. GRIDMAX;
                len : integer;
                str : array[1 .. MAXDIGITS] of char;
                val : integer;
                adj : boolean;
             end;
   tNumbers = array[1 .. MAXNUMBERS] of tNumber;

   { gears found on the grid and the count of parts around them. }
   tGear = record
              row,                     { location on grid }
              col : GRIDMIN .. GRIDMAX;
              adj : integer;           { adjacent grids with # }
              pix : array[1 .. 8] of integer; { part number index }
           end;
   tGears = array[1 .. MAXGEARS] of tGear;

   { a processed line from the input dataset. }
   tPayload = record
                 original : string;   { nothing needed here yet }
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


{ load the problem dataset }
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
   aocrewind;
   while not aoceof do begin
      i := i + 1;
      s := aocread;
      new(curr);
      curr^.fwd := nil;
      curr^.bwd := prior;
      curr^.num := i;
      curr^.rec := s;
      curr^.pld.original := s;
      if first = nil then
         first := curr
      else
         prior^.fwd := curr;
      prior := curr
   end; { while not eof }
   load := first;
end;


{ are any of the adjacent grid cells holding a symbol? }

function adjacency(g : tGrid;
                   r : integer;
                   c : integer) : boolean;

begin
   adjacency := (g[r-1, c-1] in SYMBOLS) or (g[r-1,c] in SYMBOLS) or (g[r-1,c+1] in SYMBOLS) or
                (g[r, c-1] in SYMBOLS)   or                          (g[r, c+1] in SYMBOLS) or
                (g[r+1, c-1] in SYMBOLS) or (g[r+1,c] in SYMBOLS) or (g[r+1, c+1] in SYMBOLS);
end;


{ for part one examine each number found on the grid, determine if
  it is adjacent to a symbol, and if so add its value to the answer.

  the information on the numbers, their location, and adjacency, are
  persisted in an array for possible use in part two. }

function partone(var g  : tGrid;
                 var n  : tNumbers;
                 var x  : tGears) : integer;
var
   i, j, k, r, c : integer;

begin
   partone := 0;
   { initialize numbers array }
   for i := low(n) to high(n) do
      with n[i] do begin
         row := GRIDMIN;
         col := GRIDMIN;
         len := 0;
         for j := low(str) to high(str) do
            str[j] := EMPTY;
         val := 0;
         adj := false;
      end; { with }
   { initialize the gears array }
   for i := low(x) to high(x) do
      with x[i] do begin
         row := 0;
         col := 0;
         adj := 0;
         for k := 1 to 8 do
            pix[k] := 0;
      end;
   { scan the grid, find digit runs, and note them in the numbers
     array. as a digit is identified, check its adjencent cells
     for symbols. }
   i := 0;
   for r := GRIDBEG to GRIDEND do begin
      c := GRIDMIN; { not gridbeg }
      while c < GRIDMAX do begin
         c := c + 1;
         if not (g[r, c] in DIGITS) then
            continue;
         i := i + 1; { log the number }
         with n[i] do begin
            row := r;
            col := c;
            len := 0;
            adj := false;
            while g[r, c] in DIGITS do begin
               len := len + 1;
               str[len] := g[r, c];
               adj := adj or adjacency(g, r, c);
               c := c + 1;
            end; { while g[r,c] }
            val := 0;
            for j := 1 to len do
               val := val * 10 + ord(str[j]) - ord('0');
         end; { with }
      end; { while c }
   end; { for }
   { sum the values of part numbers adjacent to parts }
   for j := 1 to i do
      with n[j] do
         if adj then
            partone := partone + val;
end;


{ are any of the adjacent grid cells holding a part number? count
  the grid cells. }
function adjparts(g : tGrid;
                  r : integer;
                  c : integer) : integer;
begin
   adjparts := 0;
   if g[r-1, c-1] in DIGITS then
      adjparts := adjparts + 1;
   if g[r-1,   c] in DIGITS then
      adjparts := adjparts + 1;
   if g[r-1, c+1] in DIGITS then
      adjparts := adjparts + 1;
   if g[  r, c-1] in DIGITS then
      adjparts := adjparts + 1;
   if g[  r, c+1] in DIGITS then
      adjparts := adjparts + 1;
   if g[r+1, c-1] in DIGITS then
      adjparts := adjparts + 1;
   if g[r+1,   c] in DIGITS then
      adjparts := adjparts + 1;
   if g[r+1, c+1] in DIGITS then
      adjparts := adjparts + 1;
end;


function partingrid(    g  : tGrid;
                        r  : integer;
                        c  : integer;
                        n  : tNumbers;
                    var ix : integer  { index of part no }
                        ) : boolean;
var
   i : integer;

begin
   if not (g[r,c] in DIGITS) then begin
      ix := 0;
      partingrid := false;
      exit
   end;
   for i := low(n) to high(n) do
      with n[i] do begin
         if row <> r then
            continue;
         if c < col then
            continue;
         if c > col+len then
            continue;
         partingrid := true;
         ix := i;
         break;
      end;
end;


{ for part two all gears must be found. a gear is a '*' symbol
  adjacent to exactly two parts. sum the part numbers of those
  parts. }

function parttwo(var g  : tGrid;
                 var n  : tNumbers;
                 var x  : tGears) : integer;
var
   i, j, k, r, c, pn1, pn2, pn3 : integer;

begin
   parttwo := 0;

   { scan the grid, find potential gears. a gear is a '*' grid
     cell adjacent to exactly two part numbers. first check is
     just for any possible numbers. a second pass will be made
     to finalize the choices. }
   i := 0;
   for r := GRIDBEG to GRIDEND do begin
      c := GRIDMIN; { not gridbeg }
      while c < GRIDMAX do begin
         c := c + 1;
         if g[r, c] <> GEAR then
            continue;
         i := i + 1; { log the number }
         with x[i] do begin
            row := r;
            col := c;
            adj := adjparts(g, r, c);
         end;
      end; { while c }
   end; { for }

   for j := 1 to i do
      with x[j] do begin
         if adj > 1 then begin
            partingrid(g, row-1, col-1, n, pix[1]);
            partingrid(g, row-1, col  , n, pix[2]);
            partingrid(g, row-1, col+1, n, pix[3]);
            partingrid(g, row  , col-1, n, pix[4]);
            partingrid(g, row  , col+1, n, pix[5]);
            partingrid(g, row+1, col-1, n, pix[6]);
            partingrid(g, row+1, col  , n, pix[7]);
            partingrid(g, row+1, col+1, n, pix[8]);
            pn1 := 0; pn2 := 0; pn3 := 0;
            for k := 1 to 8 do
               if pix[k] <> 0 then begin
                  pn1 := pix[k];
                  break;
               end;
            for k := 1 to 8 do
               if (pix[k] <> 0) and (pix[k] <> pn1) then begin
                  pn2 := pix[k];
                  break;
               end;
            if pn2 = 0 then
               continue;
            for k := 1 to 8 do
               if (pix[k] <> 0) and (pix[k] <> pn1) and (pix[k] <> pn2) then begin
                  pn3 := pix[k];
                  break;
               end;
            if pn3 <> 0 then begin
               { more than two parts found }
               continue;
            end;
            pn1 := n[pn1].val;
            pn2 := n[pn2].val;
            parttwo := parttwo + (pn1 * pn2);
         end
      end;
end;


{ populate the grid. could be done interleaved with load
  or partone, but let's be clean. }
function populate(pd : pData) : tGrid;

var
   i, j : integer;
   g    : tGrid;

begin
   { iniitalize the grid }
   for i := GRIDMIN to GRIDMAX do
      for j := GRIDMIN to GRIDMAX do
         g[i,j] := EMPTY;

   i := 0;
   while pd <> nil do
      with pd^.pld do begin
         i := i + 1;
         for j := GRIDBEG to GRIDEND do
            if j <= length(pd^.pld.original) then
               g[i,j] := pd^.pld.original[j];
         pd := pd^.fwd
      end; { with pd^ }

   populate := g;
end;


{
mainline
}

var
   grid    : tGrid;
   numbers : tNumbers;
   gears   : tGears;
   dataset : pData;
   i       : integer;

begin
   aocopen;
   dataset := load;
   grid := populate(dataset);
   i := partone(grid, numbers, gears);
   writeln('part one: ', i);
   i := parttwo(grid, numbers, gears);;
   writeln('part two: ', i);
   aocclose;
end.
