{ solution.pas -- rucksack reorganization -- Troy Brumley blametroi@gmail.com }

program solution;

{ advent of code 2022 day 03 -- rucksack reorganization

  the elves have packed poorly. silly elves }

{$longstrings off}
{$booleval on}

type
   integer = int64;

   tCharSet = set of char;

   { input gives us the contents of a rucksack, which are evenly
     divided between two sides cleverly named one and two. intermediate
     work from parsing is preserved in dup and cnt. per the spec there
     should be only one item type in common between the compartments. }

   tPayload = record
                 one,              { each compartment }
                 two : string;
                 dup : tCharSet;   { set of items types in common }
                 cnt : integer;    { number of item types in common }
                 cha : string;     { the actual item type flag }
                 pri : integer;    { its priority }
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


{$I aocinput.p }
{$I str2charset.p }
{$I charset2str.p }


{ return the priority of an item code. 'a'-'z' are 1-26, and 'A'-'Z'
  are 27-52. }

function itemPriority(c : char) : integer;

begin
   if c in ['a' .. 'z'] then
      itemPriority := ord(c) - ord('a') + 1
   else if c in ['A' .. 'Z'] then
      itemPriority := ord(c) - ord('A') + 27
   else
      itemPriority := -1
end;


{ check rucksack compartments one and two for common item types.
  returns the number of item types shared between both compartments.

  updates the rucksack info with the item type(s). }

function commonItems(var pld : tPayload) : integer;

var
   i      : integer;
   inOne,
   inTwo  : tCharSet;

begin
   with pld do begin
      inOne := str2charset(one);
      inTwo := str2charset(two);
      dup := inOne * inTwo; { that's the intersection operator }
      cnt := 0;
      for i := ord('a') to ord('z') do
         if chr(i) in dup then begin
            cha := cha + chr(i);
            cnt := cnt + 1
         end;
      for i := ord('A') to ord('Z') do
         if chr(i) in dup then begin
            cha := cha + chr(i);
            cnt := cnt + 1
         end;
      commonItems := cnt
   end
end;


{ parse the record into the payload and identify the common item
  and its priority. }

procedure parse(    rec : string;
                var pld : tPayload);
var
   i, j : integer;

begin
   with pld do begin
      one := '';
      two := '';
      j := length(rec) div 2;
      for i := 1 to j do begin
         one := one + rec[i];
         two := two + rec[j + i];
      end; { for }
   end; { with }
   commonItems(pld);
   if length(pld.cha) = 1 then
      pld.pri := itemPriority(pld.cha[1]);
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
      s := aocread;
      new(curr);
      curr^.fwd := nil;
      curr^.bwd := prior;
      curr^.num := i;
      curr^.rec := s;
      parse(curr^.rec, curr^.pld);
      if first = nil then
         first := curr
      else
         prior^.fwd := curr;
      prior := curr
   end; { while not eof }
   load := first;
end;


{ parse during load has done the core work of finding the one item
  type that is duplicated in each rucksack, and its priority value.
  sum the priorities and report the total. }

function partone(pd : pData) : integer;

begin
   partone := 0;
   while pd <> nil do
      with pd^.pld do begin
         partone := partone + pri;
         pd := pd^.fwd
      end; { with pd^ }
end;


{ processing similar to part one and the loader's parse is needed for
  part two. here each group of three elves will share one item in
  common across their rucksacks.

  while the processing is similar, the parse during the load makes
  some assumptions about the data that won't work well with part two.
  a slightly different approach is used to find the priority of the
  item each trio has in common and the sum of those items is the
  answer. }

function partTwo(pd : pData) : integer;

var
   addable,
   i,
   g           : integer;
   elfOne,
   elfTwo,
   elfThree,
   inCommon    : tCharset;
   displayable : string;

begin
   parttwo := 0;
   i := 0;
   g := 0;
   while pd <> nil do begin
      i := i + 1;
      g := g + 1;
      elfOne := str2charset(pd^.rec);
      pd := pd^.fwd;
      i := i + 1;
      elfTwo := str2charset(pd^.rec);
      pd := pd^.fwd;
      i := i + 1;
      elfThree := str2charset(pd^.rec);
      inCommon := elfOne * elfTwo * elfThree;
      displayable := charset2str(inCommon);
      addable := 0;
      if length(displayable) = length('[ ''?'' ]') then
         addable := itemPriority(displayable[4]);
      parttwo := parttwo + addable;
      { bump for next loop }
      pd := pd^.fwd
   end; { while }
end;


{ the mainline driver for a day's solution. load and then iterate and
  display results. input and output are fixed named files. }

procedure mainline;

var
   dataset  : pData;
   p1answer,
   p2answer : integer;

begin
   aocopen;

   dataset := load;
   p1answer := partOne(dataset);
   p2answer := partTwo(dataset);

   writeln('part one answer : ', p1answer);
   writeln('part two answer : ', p2answer);

   aocclose;
end;


{ minimal boot to mainline }

begin
  mainline;
end.
