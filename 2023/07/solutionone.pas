{ solutionone.pas -- camel cards -- Troy Brumley BlameTroi@gmail.com }

program solutionone;

{ expects to be compiled with the following two mode flags and command
line/fpc.cfg options of -Ciort -Sgix -Sh- for full checks, allowing
exceptions, goto, inlining, and prefer classic strings. }
{$longstrings off}            { to be sure, use AnsiString or pchar explicitly }
{$booleval on}                { standard pascal expects this }

type
  integer = int64;            { by default ints are 64 bits this decade}

{ advent of code 2023 day 7, camel cards

  scoring a poker like card game played without suits. the parts
  are split into two files to simplify some code.
}


const
{$define xxxTESTING }
{$ifdef TESTING }
   MAXHANDS = 5;
{$else}
   MAXHANDS = 1000;
{$endif}
   MAXCARDS = 5;                 { in a hand }
   CARDRANK = '23456789TJQKA';   { ranking for part one }
{$I constchars.p }


type
   { classify the hand, from lowest to highest ranking }
   tKind = (tkUnknown,
            tkSingle,
            tkPair,
            tkTwoPair,
            tkThree,
            tkFullHouse,
            tkFour,
            tkFive);
   { rank poker hands, no suites. }
   tHand = record
              cards  : string;   { the original cards }
              bid    : integer;  { and bid }
              sorted : string;   { sorted within hand }
              uniq   : integer;  { unique ranks in hand }
              kind   : tKind;    { classify hand }
           end;


{$I aocinput.p }
{$I str2int.p }
{$I strindex.p }
{$I strswapchar.p }


{ keep the hands in global storage }
var
  hands    : array[1 .. MAXHANDS] of tHand;
  numhands : integer;


{ initialize global storage }

procedure init;

var
   i : integer;

begin
   for i := 1 to MAXHANDS do
      with hands[i] do begin
         cards := '';
         bid := 0;
         sorted := '';
         uniq := 0;
         kind := tkUnknown;
      end;
   numhands := 0;
end;


{ compare hands and return their ordering via the neg-0-pos
  convention. }

function comparehands(i, j : integer) : integer;

var
  k : integer;

begin
  comparehands := ord(hands[i].kind) - ord(hands[j].kind);
  if comparehands = 0 then begin
    k := 0;
    while (comparehands = 0) and (k < MAXCARDS) do begin
      k := k + 1;
      comparehands := strindex(CARDRANK,hands[i].cards[k]) - strindex(CARDRANK, hands[j].cards[k]);
    end;
  end;
end;


{ a very dumb bubble sort of the collection of card hands. }

procedure sorthands;

var
   i, j : integer;
   s    : boolean;
   swap : tHand;

begin
   s := true;
   while s do begin
      s := false;
      for i := 1 to MAXHANDS - 1 do begin
         j := comparehands(i, i + 1);
         if j > 0 then begin
            s := true;
            swap := hands[i];
            hands[i] := hands[i + 1];
            hands[i + 1] := swap;
         end;
      end;
   end;
end;


{ sort the cards in a hand based on their rank. }
function sortcards(s : string) : string;

var
   i : integer;
   t : string;
   c : char;

begin
   t := '';
   for i := 1 to length(s) do
      t := t + s[i];
   c := t[1];
   while c <> SPACE do begin
      C := SPACE;
      for i := 1 to length(t) - 1 do
         if strindex(CARDRANK, t[i]) > strindex(CARDRANK, t[i+1]) then begin
            c := t[i];
            t[i] := t[i+1];
            t[i+1] := c;
         end;
   end;
   sortcards := t;
end;


{ how many unique ranks are in the hand. quickly identifies
  a couple of hand types and provides a filter to simplify
  checking for the rest. }

function countranks(s : string) : integer;

var
   i, j : integer;
   cs   : set of char;

begin
   cs := [];
   for i := 1 to length(s) do
      include(cs, s[i]);
   j := 0;
   for i := 1 to length(CARDRANK) do
      if CARDRANK[i] in cs then
         j := j + 1;
   countranks := j;
end;


{ determine the kind of hand (pair, full house, etc). }

function whatkind(uniq   : integer;
                  sorted : string) : tKind;
var
   j, k, l : integer;
   c       : char;

begin
   whatkind := tkUnknown;
   case uniq of
     1 : whatkind := tkFive;
     2 : { could be tkfour tkfullhouse }
         begin
            j := 0;
            while whatkind = tkUnknown do begin
               j := j + 1;
               k := 0;
               c := sorted[j];
               for l := 1 to length(sorted) do
                  if c = sorted[l] then
                     k := k + 1;
               if (k = 2) or (k = 3) then
                  whatkind := tkFullHouse;
               if k = 4 then
                  whatkind := tkFour;
            end;
         end;
     3 : { could be tktwopair or tkthree }
         begin
            j := 0;
            while whatkind = tkUnknown do begin
               j := j + 1;
               k := 0;
               c := sorted[j];
               for l := 1 to length(sorted) do
                  if c = sorted[l] then
                     k := k + 1;
               if k = 3 then
                  whatkind := tkThree;
               if k = 2 then
                  whatkind := tkTwoPair;
            end;
         end;
     4 : whatkind := tkpair;
     5 : whatkind := tksingle;
   end; { case }
end;


{ now that we know the kind of hand, resort the cards so
  that and single cards come at the end. }

function resortcards(kind  : tKind;
                     cards : string) : string;
var
   i, j : integer;
   c    : char;
   s    : string;

begin
   resortcards := 'ERROR';
   s := cards;
   case kind of
     tkUnknown : { should be impossible. }
                 resortcards := 'ERROR';
     tkSingle, tkFive : { sorted by card ranks. }
     resortcards := s;
     tkPair : { pair in front }
              begin
                 { move the pair to the front of of the hand. }
                 i := 1;
                 j := 0;
                 while (j = 0) and (i < length(s)) do begin
                    if s[i] = s[i+1] then
                       j := i; { start of run }
                    i := i + 1;
                 end;
                 if j <> 1 then begin
                    c := s[1];
                    s[1] := s[j];
                    s[j] := c;
                    c := s[2];
                    s[2] := s[j+1];
                    s[j+1] := c;
                 end;
                 resortcards := s
              end;
     tkThree : { triple in front }
               begin
                  { move the triple to the front of of the hand. }
                  i := 1;
                  j := 0;
                  while (j = 0) and (i < length(s)) do begin
                     if s[i] = s[i+1] then
                        j := i; { start of run }
                     i := i + 1;
                  end;
                  { if the run doesn't start at 1, it start at 2 or
                  3. if run starts at 2 (xyyyz), swap 1 with 4.
                  if it starts at 3 (xzyyy), also swap 2 and 5. }
                  if (j = 2) or (j = 3) then begin
                     s := strswapchar(s, 1, 4);
                     if j = 3 then
                        s := strswapchar(s, 2, 5);
                  end;
                  resortcards := s
               end;
     tkTwoPair : { pairs in front, low to high. }
                 begin
                    { odd card can only be in position 1, 3, or 5. 5 is
                    the desired outcome. this will bubble the odd
                    card to 5. }
                    if s[1] <> s[2] then
                       s := strswapchar(s, 1, 3);
                    if s[3] <> s[4] then
                       s := strswapchar(s, 3, 5);
                    resortcards := s;
                 end;
     tkFullHouse : { triple in front, pair last. }
                   begin
                      if s[1] <> s[3] then begin
                         { yyXXX }
                         s := strswapchar(s, 1, 4);
                         s := strswapchar(s, 2, 5);
                      end;
                      resortcards := s
                   end;
     tkFour:
            begin
               { push the singleton to the end of the hand. }
               if s[1] <> s[2] then begin
                  c := s[1];
                  s[1] := s[length(s)];
                  s[length(s)] := c;
               end;
               resortcards := s;
            end;
   end;
end;


{ read the hands and do basic parsing. }

procedure load;

var
   i, p : integer;
   s    : string;

begin
   i := 0;
   while not aoceof do begin
      i := i + 1;
      s := aocread;
      with hands[i] do begin
         cards := '';
         for p := 1 to MAXCARDS do
            cards := cards + s[p];
         sorted := sortcards(cards);
         p := p + 2; { skip to first digit of bid }
         bid := str2int(s, p);
         uniq := countranks(sorted);
         kind := whatkind(uniq, sorted);
         sorted := resortcards(kind, sorted);
      end;
   end;
   numhands := i;
end;


{ order the hands into ascending order by strength and return
  the sum of rank (subscript, but watch for ties) and bid. }

function partone : integer;

var
   i : integer;

begin
   init;
   load;
   partone := 0;
   sorthands;
   for i := 1 to numhands do
      partone := partone + (hands[i].bid * i);
end;


{ mainline }
var
   i : integer;

begin
   aocopen;
   i := partone;
   writeln('part one: ', i);
   aocclose;
end.
