{ solutionone.pas -- getting seedy -- Troy Brumley blametroi@gmail.com }
program solutionone;
{$longstrings off}
{$booleval on}


{ advent of code 2023 day 5 -- getting seedy

  the goal is to find a location through the maps. the original value
  is preserved and then passed through each map as source, with the
  result reported in 'dest'. then cycle dest as the new source and
  keep going until the last map is processed. }

type integer = int64;

const
{$define xxxTESTING }
{$ifdef TESTING }
   MAXSEEDS = 10;
   MAXMAPS = 10;
   MAXRANGES = 10;
{$else}
   MAXSEEDS = 50;
   MAXMAPS = 10;
   MAXRANGES = 100;
{$endif}
   CSEEDS = 'seeds:';         { input markers }
   CMAP = 'map:';
   NULLVAL = -1;              { dead cell marker }

{$I constchars.p }

type
   { walk the seed through the maps }
   tSeedToLocation = record
                        seed     : integer;  { as originally entered }
                        dest     : integer;  { result from map }
                        src      : integer;  { passed through to map }
                     end;

   { range translation, source to destination. }
   tRange = record
               begdest   : integer; { first slot in dest }
               begsrc    : integer; { first slot in source }
               len       : integer; { length of each }
            end;

   { the map or index or lookup, pick your word. find the range that
     holds a number in source and its equivalent in destination. that
     number is input to the next map. }
   tMap = record
             tag    : string;
             ranges : array[1 .. MAXRANGES] of tRange;
             numranges : 0 .. MAXRANGES;
          end;

{$I aocinput.p }
{$I str2int.p }
{$I strbegins.p }


{ rather than pass, let's put the tables as globals. }
var
   seeds    : array[1 .. MAXSEEDS] of tSeedToLocation;
   numseeds : 0 .. MAXSEEDS;
   maps     : array[1 .. MAXMAPS] of tMap;
   nummaps  : 0 .. MAXMAPS;


{ zero everything }

procedure init;

var
   i, j : integer;

begin
   for i := 1 to MAXSEEDS do
      with seeds[i] do begin
         seed := NULLVAL;
         dest := NULLVAL;
         src  := NULLVAL;
      end;
   numseeds := 0;
   for i := 1 to MAXMAPS do
      with maps[i] do begin
         tag := 'NULL';
         for j := 1 to MAXRANGES do
            with ranges[i] do begin
               begdest := NULLVAL;
               begsrc := NULLVAL;
               len := NULLVAL;
            end;
         numranges := 0;
      end;
   nummaps := 0;
end;


{ read the seed numbers. they are all one one input record, preceded
  by 'seed:', space delimited integers. }

procedure loadseeds;

var
   p : integer;
   s : string;

begin
   s := aocread;
   if not strbegins(s, CSEEDS) then
      aochalt('invalid input, missing seeds');
   p := length(CSEEDS) + 1; { position to first space }
   numseeds := 0;
   while p < length(s) do begin
      numseeds := numseeds + 1;
      if s[p] = SPACE then
         p := p + 1;
      with seeds[numseeds] do begin
         seed := str2int(s, p); { position to non digit }
         dest := NULLVAL;
         src := NULLVAL;
      end;
   end;
end;


{ each 'map' consists of one or more ranges. maps are identified by
  a tag record holding the name of the map (something-to-something)
  followed by the word 'map:'. there are seven maps and they happen
  to be in the correct order as read so no link pointers are needed.

  each range is a single input record holding three numbers.
  destination index value start, source (or input) index value start,
  and the length of the range. a map can hold multiple ranges, and
  a blank record or end of file marks the end of the current map's
  ranges. }

procedure loadmaps;

var
   p : integer;
   s : string;

begin
   s := aocread;
   if s <> '' then
      aochalt('input sequence error in loadmaps');
   nummaps := 0;
   while not aoceof do begin
      s := aocread;
      if not (s[1] in ['a' .. 'z', 'A' .. 'Z']) then
         aochalt('unexpected input in loadmaps: ' + s);
      nummaps := nummaps + 1;
      with maps[nummaps] do begin
         tag := s;
         numranges := 0;
         while not eof(input) do begin
            readln(s);
            if s = '' then
               break;
            p := 1;
            numranges := numranges + 1;
            with ranges[numranges] do begin
               begdest := str2int(s, p);
               p := p + 1;
               begsrc := str2int(s, p);
               p := p + 1;
               len := str2int(s, p);
            end; { with ranges }
         end; { while not eof ranges }
      end; { with maps }
   end; { while not eof maps }
end;


{ it's an index }
function lookup(src : integer;
                map : tMap) : integer;
var
   i : integer;

begin
   lookup := src;
   with map do
      for i := 1 to numranges do
         with ranges[i] do begin
            if (begsrc <= src) and (src <= begsrc + len) then begin
               lookup := begdest + (src - begsrc);
               break;
            end;
         end;
end;


{ walk each seed through the maps using the lookup defined, then
  find the lowest final seed location and report it. }

function partone : integer;

var
   i : integer;
   lowestseed : integer;

begin
   for i := 1 to numseeds do
      with seeds[i] do begin
         src := seed;
         dest := lookup(src, maps[1]);
         dest := lookup(dest, maps[2]);
         dest := lookup(dest, maps[3]);
         dest := lookup(dest, maps[4]);
         dest := lookup(dest, maps[5]);
         dest := lookup(dest, maps[6]);
         dest := lookup(dest, maps[7]);
      end;

   lowestseed := 1;
   for i := 1 to numseeds do
      with seeds[i] do
         if dest < seeds[lowestseed].dest then
            lowestseed := i;

   partone := seeds[lowestseed].dest;
end;


{ only a part one in this program, part two is split to a separate program. }

procedure mainline;

var
   p1answer : integer;

begin
   aocopen;

   init;
   loadseeds;
   loadmaps;
   p1answer := partone;
   writeln('part one answer : ', p1answer);
   aocclose;
end;


{ minimal boot to mainline }
begin
   mainline;
end.
