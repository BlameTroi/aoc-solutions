{ solution.pas -- distress signal -- Troy Brumley BlameTroi@gmail.com }

program solution;

{ expects to be compiled with the following two mode flags and command
  line/fpc.cfg options of -Ciort -Sgix -Sh- for full checks, allowing
  exceptions, goto, inlining, and prefer classic strings. }
// {$LONGSTRINGS OFF}            { to be sure, use AnsiString or pchar explicitly }
{$BOOLEVAL ON}                { standard pascal expects this }

{ advent of code 2022 day 13 -- distress signal

  for part one, identify communication packets that are correctly
  ordered. the packets are lists and this looks rather lispish in
  lean, but i'll stick with pascal. }

{$LONGSTRINGS ON } { <-------- the input is very wide }

const
   MAXPAIRS = 500;

type
   integer = int64;            { by default ints are 64 bits this decade}


{$I aocinput.p }
{$I str2int.p }
{$I int2str.p }


{ the only way i've been able to get an algorithm that doesn't offend
  my sensibilities is to get closer and closer to a lisp list. i guess
  they knew what they were doing way back then. a list is at least two
  nodes. the last node is always empty. as the problem for advent of
  code is dealing with lists of integers, the value payload is just an
  integer. }


type
   patom = ^tatom;
   tatom = record
              isnull : integer;   { 0 = false, 1 = true }
              value  : integer;
           end;

   plist = ^tlist;
   tlist = record
              atom   : patom;
              nested : plist;
              next   : plist;
           end;


{ list node manipulators }

{ create a new list ground/nil node }

function newempty : plist;
begin
   new(newempty);
   with newempty^ do begin
      atom := nil;
      nested := nil;
      next := nil;
   end;
end;


{ a list can contain another list }

procedure setnested(xs, ys : plist);
begin
   with xs^ do begin
      if atom <> nil then begin
         writeln(stderr, '*** assigning nested to atom node ***');
         halt;
      end;
      nested := ys;
   end;
end;


function getnested(xs : plist) : plist;
begin
   getnested := xs^.nested;
end;


{ a list can contain an atomic value, in this application an integer }

procedure setvalue(xs : plist; n : integer);
begin
   with xs^ do begin
      if nested <> nil then begin
         writeln(stderr, '*** assigning value to nesting node ***');
         halt;
      end;
      { allocate storage for the atom, or reuse if one already there }
      if atom = nil then
         new(atom);
      atom^.isnull := 0;
      atom^.value := n;
   end;
end;


function getvalue(xs : plist) : integer;
begin
   getvalue := xs^.atom^.value;
end;


{ list nodes chain together for form the actual list }

procedure setnext(xs, ys : plist);
begin
   xs^.next := ys;
end;

function addnewempty(xs : plist) : plist;
begin
   xs^.next := newempty;
   addnewempty := xs^.next;
end;


{ various list node predicates }

function isempty(x : plist) : boolean; inline;
begin
   isempty := (x^.atom = nil) and (x^.nested = nil);
end;

function hasvalue(x: plist) : boolean; inline;
begin
   hasvalue := x^.atom <> nil;
end;

function hasnested(x : plist) : boolean; inline;
begin
   hasnested := x^.nested <> nil;
end;

function hasnext(x : plist) : boolean; inline;
begin
   hasnext := x^.next <> nil;
end;

function isnilorempty(x : plist) : boolean; inline;
begin
   if x = nil then
      isnilorempty := true
   else
      isnilorempty := isempty(x);
end;

function notnilorempty(x : plist) : boolean; inline;
begin
   notnilorempty := not isnilorempty(x);
end;


{ release the list any linked atom values and sub lists, clearing any
  storage before it is released. }

procedure freelist(xs : plist);

var
   nx : plist;

begin
   while xs <> nil do begin
      { hang on to next pointer }
      nx := xs^.next;

      { free chained lists }
      if hasnested(xs) then begin
         freelist(xs^.nested);
         xs^.nested := nil;
      end;

      { free value if there is one }
      if xs^.atom <> nil then begin
         xs^.atom^.isnull := 0;
         xs^.atom^.value := 0;
         dispose(xs^.atom);
         xs^.atom := nil;
      end;

      { release and advance }
      xs^.next := nil;
      dispose(xs);

      xs := nx;
   end;
end;

function listfromvalue(x : plist) : plist;
var
   y : plist;
begin
   y := newempty;
   if hasvalue(x) then
      setvalue(y, getvalue(x));
   listfromvalue := y;
end;

{ recursive helper for aslist. should be called with pos pointing at
  a list open bracket [ and will return as each list close bracket
  ] is reached. if everything is nested properly, it works. there is
  no attempt at input validation. }

function aslistr(    str : string;
                 var pos : integer) : plist;
var
   xs, ys : plist;
   n      : integer;

begin
   { guards and special cases }
   if length(str) < 2 then begin
      aslistr := nil;
      exit;
   end;

   if (str[pos] = '[') and (str[pos+1] = ']') then begin
      pos := pos + 2;
      aslistr := newempty;
      exit;
   end;

   if (str[pos] <> '[') or (str[length(str)] <> ']') then begin
      aslistr := nil;
      pos := pos + 1;
      exit;
   end;

   { first list node is also function result }
   xs := newempty;
   aslistr := xs;

   { consume opening [ }
   pos := pos + 1;

   while pos <= length(str) do begin

      { commas and spaces are white space }
      if (str[pos] = ' ') or (str[pos] = ',') then begin
         pos := pos + 1;
         continue;
      end;

      { end of the current list }
      if str[pos] = ']' then begin
         pos := pos + 1;
         break;
      end;

      { starting a nested list }
      if str[pos] = '[' then begin
         ys := aslistr(str, pos);
         setnested(xs, ys);
         xs := addnewempty(xs);
         continue;
      end;

      { add atom to list, only ints supported }
      if str[pos] in ['0'..'9'] then begin
         n := str2int(str, pos);
         setvalue(xs, n);
         xs := addnewempty(xs);
         continue;
      end;

      { error }
      writeln(stderr, 'error ', str[pos], ' at ', pos);
      pos := pos + 1;

   end;

end;


{ from a list of integers and nested lists of integers in string form,
  create a navigable set of nodes. the ugly details are in a recursive
  helper. }

function aslist(str : string) : plist;
var
   pos : integer;

begin
   pos := 1;
   aslist := aslistr(str, pos);
end;


{ given a list of nodes, return a string representation. }

function printlist(xs : plist) : string;

begin
   printlist := '[';
   while not isempty(xs) do begin               { end of (sub) list reached }
      if hasnested(xs) then begin               { sub list }
         printlist := printlist + printlist(getnested(xs));
         if not isempty(xs^.next) then
            printlist := printlist + ',';
         xs := xs^.next;
         continue;
      end;
      if hasvalue(xs) then begin                { or atom }
         printlist := printlist + int2str(getvalue(xs));
         if not isempty(xs^.next) then
            printlist := printlist + ',';
         xs := xs^.next;
         continue;
      end;
      { error }
      writeln(stderr, 'invalid list construction');
      break;
   end;
   printlist := printlist + ']';
end;


{ global storage for the aoc part of this }

type
   tpair =  record
               left    : plist;
               right   : plist;
               inorder : integer;
            end;

var
   pair       : array[1 .. MAXPAIRS] of tpair;
   packet     : array[1 .. MAXPAIRS*2] of plist;
   numpairs   : integer;
   numpackets : integer;


{ clear out the arrays for parts one and two }

procedure init;

begin
   for numpairs := 1 to MAXPAIRS do
      with pair[numpairs] do begin
         left := nil;
         right := nil;
         inorder := 0;
         packet[numpairs*2 - 1] := nil;
         packet[numpairs*2] := nil;
      end;
   numpairs := 0;
   numpackets := 0;
end;


{ compare two lists using the rules for part one and return via the
  negative zero positive convention.

  compare the lists item by item, with the following behaviors for
  nested lists:

  a longer list comes after an otherwise equivalent list

  if one item is an integer and the other is a list, promote the
  integer to a list and then compare as above.

  once you know an item is out of order, you are done. }

function comparelist(x1, x2 : plist) : integer;

var
   d  : integer;
   tx : plist;

begin
   { assume out of order }
   comparelist := +high(int64);

   while notnilorempty(x1) and notnilorempty(x2) do begin
      if hasvalue(x1) and hasvalue(x2) then begin
         d := getvalue(x1) - getvalue(x2);
         if d <> 0 then begin
            comparelist := d;
            exit;
         end;
         x1 := x1^.next; x2 := x2^.next;
         continue;
      end;
      if hasnested(x1) and hasnested(x2) then begin
         d := comparelist(getnested(x1), getnested(x2));
         if d <> 0 then begin
            comparelist := d;
            exit;
         end;
         x1 := x1^.next; x2 := x2^.next;
         continue;
      end;
      if hasvalue(x1) and hasnested(x2) then begin
         tx := listfromvalue(x1);
         d := comparelist(tx, getnested(x2));
         freelist(tx);
         if d <> 0 then begin
            comparelist := d;
            exit;
         end;
         x1 := x1^.next; x2 := x2^.next;
         continue;
      end;
      if hasnested(x1) and hasvalue(x2) then begin
         tx := listfromvalue(x2);
         d := comparelist(getnested(x1), tx);
         freelist(tx);
         if d <> 0 then begin
            comparelist := d;
            exit;
         end;
         x1 := x1^.next; x2 := x2^.next;
         continue;
      end;
      aochalt('invalid list structure! abort in compare');
      break;
   end;
   { if both lists are exhausted, 'equal', if left empty, in order, if right empty, out of order }
   if isnilorempty(x1) and isnilorempty(x2) then
      comparelist := 0
   else if isnilorempty(x1) then
      comparelist := -high(int64)
   else
      comparelist := +high(int64);
end;


{ read and parse into two arrays for parts one and two. part two wants
  the lists sorted, so since we're already doing a compare to check
  for pair ordering, lay each pair into the second array in order to
  save a few compare and swaps later on. }

procedure load;

var
   i, j    : integer;
   s, t, u : string;

begin
   aocrewind;
   i := 0; j := -1;
   while not aoceof do begin
      i := i + 1; j := j + 2;
      s := aocread;
      t := aocread;
      u := aocread;
      if u <> '' then
         aochalt('*** error in input ***');
      with pair[i] do begin
         left := aslist(s);
         right := aslist(t);
         inorder := comparelist(left, right);
         if inorder < 1 then begin        { a very minor optimization }
            packet[j] := left;
            packet[j+1] := right;
         end else begin
            packet[j] := right;
            packet[j+1] := left;
         end;
      end;
   end;
   numpairs := i;
   numpackets := i*2;
end;


{ a not so quick sort of the packets into ascending order }

procedure sort;

var
   i : integer;
   f : boolean;
   s : plist;

begin
   f := true;
   while f do begin
      f := false;
      for i := 1 to numpackets-1 do
         if comparelist(packet[i], packet[i+1]) > 0 then begin
            f := true;
            s := packet[i];
            packet[i] := packet[i+1];
            packet[i+1] := s;
         end;
   end;
end;


{ find a particular list value in the packets }

function indexof(x : plist) : integer;

var
   i : integer;

begin
   indexof := -1;
   for i := 1 to numpackets do
      if comparelist(packet[i], x) = 0 then begin
         indexof := i;
         break;
      end;
end;


{ mainline }

var
   i        : integer;
   partone  : integer;
   parttwo  : integer;
   pk1, pk2 : integer;

begin
   aocopen;
   init;
   load;
   partone := 0;
   for i := 1 to numpairs do begin
      if pair[i].inorder < 1 then
         partone := partone + i;
   end;
   packet[numpackets+1] := aslist('[[2]]');
   packet[numpackets+2] := aslist('[[6]]');
   numpackets := numpackets + 2;
   sort;
   pk1 := indexof(aslist('[[2]]'));
   pk2 := indexof(aslist('[[6]]'));
   parttwo := pk1 * pk2;
   writeln('part one answer : ', partone);
   writeln('part two answer : ', parttwo);
   aocclose;
end.

{ end solution.pas }
