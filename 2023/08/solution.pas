{ solution.pas -- haunted wasteland -- Troy Brumley BlameTroi@gmail.com }

program solution;

{ expects to be compiled with the following two mode flags and command
line/fpc.cfg options of -Ciort -Sgix -Sh- for full checks, allowing
exceptions, goto, inlining, and prefer classic strings. }
{$longstrings off}            { to be sure, use AnsiString or pchar explicitly }
{$booleval on}                { standard pascal expects this }

type
  integer = int64;            { by default ints are 64 bits this decade}

{ advent of code 2023 day 8 -- haunted wasteland

  this is a map node traversal problem. walk from node to node
  according to the directions given and count your steps.

  for part 1 of the problem, given an instruction loop and
  a list of map nodes, how many steps are needed to go from
  node aaa to node zzz?

  for part 2 of the problem, it's a simultaneous traversal of
  nodes with slightly different instructions. count the steps
  to where all the simultaneous paths reach the same point at
  the same time.

  part 2 was a bear and i solved it outside of this framework
  but using some of the code to determine how long it would
  take each of the walks to loop. finding prime multiples and
  then multiplying all the steps sans the common gets the
  answer which is in problem.txt. }


const
{$define xTESTING }
{$define xTESTONE }
{$ifdef TESTING }
{$ifdef TESTONE }
   MAXNODES = 7;
   MAXMOVES = 100;
   MAXSIMULTANEOUS = MAXNODES;
{$else}
   MAXNODES = 8;
   MAXMOVES = 100;
   MAXSIMULTANEOUS = MAXNODES;
{$endif}
{$else}
   MAXNODES = 766;
   MAXMOVES = 1000;
   MAXSIMULTANEOUS = 100; { actually only about 6 }
{$endif}
{$I constchars.p }
   startatid = 'AAA';       { node to begin walking from }
   startsuffix = 'A';       { part two goes by node suffix }
   endatid = 'ZZZ';         { target node }
   endsuffix = 'Z';         { part two goes by node suffix }
   goleft = 'L';            { directions }
   goright = 'R';

type
   { input is a graph, format: NNN = (LLL,RRR) }
   tidstr = string[3];    { nodes ids are 3 chars }
   tNode = record
              id      : tidstr;   { node tag }
              leftid  : tidstr;   { left tag }
              rightid : tidstr;   { right tag }
              pleft   : integer;  { left cached }
              pright  : integer;  { right cached }
           end;


{ global variables are appropriate for this problem. }

var
  nodes      : array[1 .. MAXNODES] of tNode;
  numnodes   : integer;
  moves      : array[1 .. MAXMOVES] of char;
  nummoves   : integer;
  nextmove   : integer;
  simulnodes : array[1 .. MAXSIMULTANEOUS] of integer;
  numsimuls  : integer;


{$I substr.p }
{$I aocinput.p }


{ initialize global storage }
procedure init;

var
   i : integer;

begin
   for i := 1 to MAXNODES do
      with nodes[i] do begin
         id := '';
         leftid := '';
         rightid := '';
         pleft := -1;
         pright := -1;
      end;
   numnodes := 0;
   for i := 1 to MAXMOVES do
      moves[i] := ' ';
   nummoves := 0;
   nextmove := -1;
   for i := 1 to MAXSIMULTANEOUS do
      simulnodes[i] := -1;
   numsimuls := 0;
end;


{ push some code down. }

function isstartnode(i : integer) : boolean; inline;
begin
   isstartnode := nodes[i].id[3] = startsuffix
end;

function isendnode(i : integer) : boolean; inline;
begin
   isendnode := nodes[i].id[3] = endsuffix
end;


{ find all the starting nodes for a simultaneous pass through the
  graph. }

procedure buildsimulnodes;

var
   i, j : integer;

begin
   j := 0;
   for i := 1 to numnodes do
      if isstartnode(i) then begin
         j := j + 1;
         simulnodes[j] := i;
      end;
   numsimuls := j;
   writeln(output);
   writeln(output, 'found ', numsimuls, ' start nodes');
   for i := 1 to numsimuls do begin
      if (i mod 3) = 1 then
         writeln(output);
      with nodes[simulnodes[i]] do
         write(output, '[', i : 3, ' ', id, '=(', leftid, ',', rightid, ')] ');
   end;
   writeln(output);
end;


{ check the current state of the simultaneous walks to see if
  they are all at end points. }

function allatend : boolean;

var
   i : integer;

begin
   allatend := false;
   for i := 1 to numsimuls do
      if not isendnode(simulnodes[i]) then
         exit;
   allatend := true;
end;


{ lookup node via id sequential search. nodes list is unordered. }

function findnode(id : tidstr) : integer;

var
   i : integer;

begin
   findnode := -1;
   for i := 1 to numnodes do
      if id = nodes[i].id then begin
         findnode := i;
         break;
      end;
end;


{ get next move. if the moves are exhausted, start over from the
  beginning of the moves list. }

function getmove : char;

begin
   getmove := moves[nextmove];
   nextmove := nextmove + 1;
   if nextmove > nummoves then
      nextmove := 1;
end;


{ read the nodes. a node record = 'NNN = (LLL, RRR)' and the
  instructions are a single long string. as this can be bigger than
  a string, a real array will be set up. }

procedure load;

var
   i : integer;
   s : string;
   c : char;

begin
   { first read the moves, watch for leading comment }
   aocrewind;
   writeln(output, 'moves');
   i := 0;
   while not aoceol do begin
      c := aocreadch;
      if c = HASH then begin
         aocflushl;
         continue;
      end;
      i := i + 1;
      moves[i] := c;
   end;
   nummoves := i;
   nextmove := 1;

   { load the nodes }
   i := 0;
   while not aoceof do begin
      i := i + 1;
      s := aocread;
      if length(s) = 0 then begin
         i := i - 1;
         continue;
      end;
      { input is fixed nnn = (lll, rrr) so parse is easy }
      with nodes[i] do begin
         id := substr(s, 1, 3);
         leftid := substr(s, 8, 8 + 3);
         rightid := substr(s, 13, 13 + 3);
         pleft := -1;
         pright := -1;
      end;
   end;
   numnodes := i;
end;


{ find the next node from the current one using the move. the nodes
  contain 'pointers' to cache the results of findnode calls. the
  dataset is small enough that this may not matter, but it's a good
  habit. }

function nextnode(curr : integer;
                  move : char) : integer;

begin
   nextnode := -1;
   if move = goleft then begin
      if nodes[curr].pleft < 1 then
         nodes[curr].pleft := findnode(nodes[curr].leftid);
      nextnode := nodes[curr].pleft
   end;
   if move = goright then begin
      if nodes[curr].pright < 1 then
         nodes[curr].pright := findnode(nodes[curr].rightid);
      nextnode := nodes[curr].pright
   end;
end;


{ walk the graph from node aaa to node zzz following the
  moves provided. }
function partone : integer;

var
   curr, next : integer;
   steps      : integer;
   move       : char;

begin
   curr := findnode(startatid);
   if curr = -1 then begin
      aocerr('starting node not found, skipping part one');
      partone := -1;
      exit;
   end;

   steps := 0;
   while nodes[curr].id <> endatid do begin
      move := getmove;
      steps := steps + 1;
      next := nextnode(curr, move);
      curr := next;
   end;
   partone := steps;
end;


{ for part two, do the moves simultaneously starting from all the
  nodes that end with 'A' until all the nodes at the end of a move end
  with 'Z'. }

function parttwo : integer;

var
  i, j   : integer;
  steps  : integer;
  move   : char;
  cycles : array[1..10] of integer;

begin
   buildsimulnodes;
   for i := 1 to numsimuls do
      cycles[i] := 0;
   steps := 0;
   { the sims will loop...some take more steps per loop than
     others. once you know how long it takes each one to
     finish, you should be able to figure out when they all
     synch. }
   while not allatend do begin
      steps := steps + 1;
      {if steps > 50 then
      aochalt('debug break'); }
      move := getmove;
      for i := 1 to numsimuls do begin
         if not isendnode(simulnodes[i]) then
            simulnodes[i] := nextnode(simulnodes[i], move);
         if cycles[i] < 1 then
            if isendnode(simulnodes[i]) then
               cycles[i] := steps;
      end;
   end;
   j := 0;
   for i := 1 to numsimuls do
      if cycles[i] = 0 then
         break
      else
         j := j + 1;
   if j = numsimuls then begin
      writeln;
      writeln('all have cycled at least once');
      for i := 1 to numsimuls do
         writeln(i : 2, cycles[i] : 20);
      writeln('break out a calculator to get the real answer');
      { aochalt('check this output'); }
   end;
   parttwo := steps;
end;


{ mainline }

var
   i : integer;

begin
   aocopen;
   aocrewind;
   init;
   load;
   i := partone;
   writeln('part one: ', i);
   i := parttwo;
   writeln('part two: ', i);
   aocclose;
end.
