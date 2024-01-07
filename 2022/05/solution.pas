{ solution.pas -- supply stacks -- Troy Brumley blametroi@gmail.com }

program solution;
{$longstrings off}
{$booleval on}


{ advent of code 2022 day 5 -- supply stacks

  crates are stacked, they will be reorganized, what crates are
  on top when the reorganization ends? }


type
   integer = int64;

const
   MAXSTACKS = 9;    { number of stack, 1 -> 9, as observed }
   MAXDEPTH = 100;   { arbitrary }
   MAXMOVES = 600;   { as observed }

type
   { a crate stack, crates are identified by letter a-z }
   tStack = record
               depth  : integer;
               crates : array[1..MAXDEPTH] of char;
            end;
   tDock = array[0..MAXSTACKS] of tStack;  { 0 is a buffer, first live is 1 }
   tMove = record
              count     : integer;
              fromstack : integer;
              tostack   : integer;
           end;


{$I aocinput.p }
{$I str2int.p }
{$I strindex.p }


{ global variables are fine for this }

var
   dock     : tDock;
   moves    : array[1..MAXMOVES] of tMove;
   nummoves : integer;


{ clear a stack }

procedure empty(stack : integer);

var
   i : integer;

begin
   for i := 1 to MAXDEPTH do
      dock[stack].crates[i] := ' ';
   dock[stack].depth := 0;
end;


{ mountain high }

function depth(stack : integer) : integer;

begin
   depth := dock[stack].depth;
end;


{ remove the id of the crate on top of the stack. and return it.
  a blank is returned if the stack is empty with no error signal. }

function pop(stack : integer) : char;

begin
   pop := ' ';
   with dock[stack] do
      if depth > 0 then begin
         pop := crates[depth];
         crates[depth] := ' ';
         depth := depth - 1
      end
end;


{ return the id of the crate on top of the stack. a blank is returned
  if the stack is empty. the stack is not altered. }

function peek(stack : integer) : char;

begin
   peek := ' ';
   with dock[stack] do
      if depth > 0 then
         peek := crates[depth]
end;


{ push a new crate id onto a stack. errors are ignored. }

procedure push(stack : integer;
               crate : char);

begin
   with dock[stack] do begin
      depth := depth + 1;
      crates[depth] := crate
   end
end;


{ list the crates on a stack }

function crates(stack : integer) : string;

var
   i : integer;

begin
   crates := '';
   for i := 1 to dock[stack].depth do
      crates := crates + dock[stack].crates[i];
end;


{ invert a stack }

procedure invert(stack : integer);

var
   i : integer;
   s : string;

begin
   s := crates(stack);
   empty(stack);
   i := 0;
   while length(s) - i > 0 do begin
      push(stack, s[length(s) - i]);
      i := i + 1;
   end;
end;


{ list the top crates on all the live stacks }

function report : string;

var
   i : integer;
   c : char;

begin
   report := '';
   for i := 1 to MAXSTACKS do begin
      c := peek(i);
      report := report + c
   end
end;


{ initialize global variables to empty }

procedure init;

var
   i : integer;

begin
   for i := 0 to MAXSTACKS do
      empty(i);
   for i := 1 to MAXMOVES do begin
      moves[i].count := 0;
      moves[i].fromstack := 0;
      moves[i].tostack := 0;
   end;
   nummoves := 0;
end;


{ load and parse the dataset }

procedure load;

var
   i, j, k : integer;
   s       : string;

begin
   init;
   aocrewind;

   { load stacks }
   { crates are enclosed in [ ], a line without means the stacks are
     loaded. stack number is identified by position on the line. }
   i := 0;
   while not aoceof do begin
      s := aocread;
      if strindex(s, '[') < 1 then
         break;
      i := i + 1;
      j := 0;
      while j < length(s) do begin
         j := j + 1;
         if s[j] in [' ', '[', ']'] then
            continue;
         push((j div 4) + 1, s[j]);
      end;
   end; { while not eof }

   { the load pushed the crates on upside down, flip them. }
   for i := 1 to MAXSTACKS do
      invert(i);

   { load moves }
   { format is 'move <count> from <stack> to <stack>' }
   i := 0;
   while not eof(input) do begin
      s := aocread;
      if s = '' then
         continue;
      i := i + 1;
      k := 6;       { past 'move ' }
      moves[i].count := str2int(s, k);
      k := k + 6;   { past ' from ' }
      moves[i].fromstack := str2int(s, k);
      k := k + 4;   { past ' to ' }
      moves[i].tostack := str2int(s, k);
   end; { while not eof }
   nummoves := i;
end;


{ perform the moves and report which crate is on top of each stack
  when all the moves are complete. push the pop, repeatedly. }

function partone : string;

var
   i, j : integer;

begin
   load;
   for i := 1 to nummoves do
      for j := 1 to moves[i].count do
         push(moves[i].tostack, pop(moves[i].fromstack));
   partone := report;
end;


{ for part two we learn that the moves (pop-push) are multi-crate.
  since the push-pop already works, i'll just use a temporary holding
  stack to preserve ordering. }

function partTwo : string;

var
   i, j :  integer;

begin
   load;
   for i := 1 to nummoves do begin
      for j := 1 to moves[i].count do
         push(0, pop(moves[i].fromstack));
      while depth(0) > 0 do
         push(moves[i].tostack, pop(0));
   end;
   partTwo := report;
end;


{ the mainline driver for a day's solution. }

procedure mainline;

var
   s : string;

begin
   aocopen;
   s := partone;
   writeln('part one answer : ', s);
   s := parttwo;
   writeln('part two answer : ', s);
   aocclose;
end;

{ minimal boot to mainline }

begin
  mainline;
end.
