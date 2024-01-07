{ solution.pas -- getting seedy -- Troy Brumley blametroi@gmail.com }
program solutiontwo;
{$longstrings off}
{$booleval on}

type
   integer = int64;

{ advent of code 2023 day 5 -- getting seedier

  the new problem for part two becomes a high volume problem so the
  'just use a big array' approach isn't the right way to do it. it
  also seemed easier to spit this into a second stand alone program. }

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
   { the goal is to find a location through the maps. the original
   value is preserved and then passed through each map as 'source',
   with the result reported in 'dest'. then cycle dest as the new
   source and keep going until the last map is processed.

   for part two, we learn that the what was viewed as all seeds in
   part one were really pairs: seed and count. the counts can be
   huge. this structure is still valid but it isn't what we'll store
   for the seed input. }
   tSeedToLocation = record
                        seed : integer;  { as originally entered }
                        dest : integer;  { result from map }
                        src  : integer;  { passed through to map }
                     end;

{ and so instead we store this input. }
   tSeed = record
              seed  : integer;
              count : integer;
           end;

{ range translation, source to destination }
   tRange = record
               begdest : integer; { first slot in destination }
               begsrc  : integer; { first slot in source }
               len     : integer; { length of each }
            end;

{ the map or index or lookup, pick your word. find the range that
  holds a number in source and its equivalent in destination. that
  number is input to the next map. }
   tMap = record
             tag       : string;
             ranges    : array[1 .. MAXRANGES] of tRange;
             numranges : 0 .. MAXRANGES;
          end;


{$I str2int.p }
{$I strbegins.p }
{$I aocinput.p }


{ rather than pass, let's put the tables as globals. }
var
   seeds    : array[1 .. MAXSEEDS] of tSeed;
   numseeds : 0 .. MAXSEEDS;
   maps     : array[1 .. MAXMAPS] of tMap;
   nummaps  : 0 .. MAXMAPS;


{ clear out globals. }

procedure init;

var
   i, j : integer;

begin
   for i := 1 to MAXSEEDS do
      with seeds[i] do begin
         seed := NULLVAL;
         count := NULLVAL;
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
         if s[p] = SPACE then
            p := p + 1;
         count := str2int(s, p);
      end;
   end;
end;


{ each 'map' consists of one or more ranges. maps are identified by a
  tag record holding the name of the map (something-to-something)
  followed by the word 'map:'. there are seven maps and they happen
  to be in the correct order as read so no link pointers are needed.

  each range is a single input record holding three numbers. the
  destination index value start, source (or input) index value start,
  and the length of the range. a map can hold multiple ranges, and a
  blank record or end of file marks the end of the current map's
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
         aochalt('unexpected input in loadmaps');
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


{ this is a rather slow approach, but it runs while i do the dishes }

function lookup(src : integer;
                map : tMap) : integer;
var
   i : integer;

begin
   lookup := src;
   with map do
      for i := 1 to numranges do
         with ranges[i] do
            if (begsrc <= src) and (src <= begsrc + len) then begin
               lookup := begdest + (src - begsrc);
               break;
            end;
end;


{ crunch very slowly for part two of this problem. }

function parttwo : integer;

var
   i, j, k   : integer;
   lowestval : integer;

begin
   lowestval := high(int64);
   for i := 1 to numseeds do
      with seeds[i] do
         for j := seed to seed + count do begin
            k := lookup(j, maps[1]);
            k := lookup(k, maps[2]);
            k := lookup(k, maps[3]);
            k := lookup(k, maps[4]);
            k := lookup(k, maps[5]);
            k := lookup(k, maps[6]);
            k := lookup(k, maps[7]);
            { don't bother trying to optimize this }
            if k < lowestval then
               lowestval := k;
         end;
   parttwo := lowestval;
end;


{ the mainline driver for a day's solution. }

procedure mainline;

var
   p2answer : integer;

begin
   aocopen;
   init;
   loadseeds;
   loadmaps;
   p2answer := parttwo;
   writeln(output, 'part two answer : ', p2answer);
   aocclose;
end;


{ minimal boot to mainline }
begin
   mainline;
end.
