{ solutiontwo.pas -- camel cards -- Troy Brumley BlameTroi@gmail.com }

program solutiontwo;

{ expects to be compiled with the following two mode flags and command
line/fpc.cfg options of -Ciort -Sgix -Sh- for full checks, allowing
exceptions, goto, inlining, and prefer classic strings. }
{$longstrings off}            { to be sure, use AnsiString or pchar explicitly }
{$booleval on}                { standard pascal expects this }

type
  integer = int64;            { by default ints are 64 bits this decade}

{ advent of code 2023 day 7, camel cards

  scoring a poker like card game played without suits. the parts
  are separated into two files to simplify some code.

  this is part two. jacks become jokers but count as 0 for ties.
}


const
{$define xxxTESTING }
{$ifdef TESTING }
   MAXHANDS = 5;
{$else}
   MAXHANDS = 1000;
{$endif}
   MAXCARDS = 5;                 { in a hand }
   CARDRANK = 'J23456789TQKA';   { ranking for part one }
   JOKER    = 'J';
   { hand classification, from lowest to highest ranking }
   ckUnknown  = 0;
   ckSingle   = 1;       { you should fold }
   ckPair     = 2;       { one pair }
   ckTwoPair  = 3;       { two pair }
   ckThree    = 4;       { three of a kind }
   ckFull     = 5;       { full house, pair and three }
   ckFour     = 6;       { four of a kind }
   ckFive     = 7;       { five of a kind }
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
              jokers : integer;  { how many are there }
              kind   : integer;  { classify hand }
              joked  : string;   { after changing jokers out }
           end;


{$I aocinput.p }
{$I str2int.p }
{$I strindex.p }
{$I strswapchar.p }
{$I substr.p }
{$I max.p }


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
         kind := ckUnknown;
      end;
   numhands := 0;
end;

{ arrange cards into descending order by rank. }

function arrangecards(s : string) : string;

var
   i, j : integer;

begin
   arrangecards := '';
   for i := length(CARDRANK) downto 1 do
      for j := 1 to length(s) do
         if s[j] = CARDRANK[i] then
            arrangecards := arrangecards + s[j];
end;


{ compare hands at subscripts i and j and return their ordering via
  the neg-0-pos convention. the kind of hand (pair, full house) takes
  precedence. ties are broken with a card by card comparison as the
  cards were dealt. }

function comparehands(i, j : integer) : integer;

var
   k : integer;

begin
   comparehands := ord(hands[i].kind) - ord(hands[j].kind);
   k := 0;
   if comparehands = 0 then
      while (comparehands = 0) and (k < MAXCARDS) do begin
         k := k + 1;
         comparehands := strindex(CARDRANK, hands[i].cards[k]) - strindex(CARDRANK, hands[j].cards[k]);
      end;
end;


{ put the list of hands into ascending rank order. yes, it's
  a simple exchange sort. }

procedure sorthands;

var
   i    : integer;
   s    : boolean;
   swap : tHand;

begin
   s := true;
   while s do begin
      s := false;
      for i := 1 to MAXHANDS - 1 do
         if  comparehands(i, i + 1) > 0 then begin
            s := true;
            swap := hands[i];
            hands[i] := hands[i + 1];
            hands[i + 1] := swap;
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


{ how many unique ranks are in the hand. quickly identifies a couple
  of hand types and provides a filter to simplify checking for the
  rest. a full 5 card hand is not required for this check. this allows
  us to find the best hand before applying any jokers. }

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


{ now that we know the kind of hand, resort the cards so that all
  single cards come at the end. }

function resortcards(kind : tKind; cards : string) : string;

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


{ determine best kind of hand that can be made by applying the jokers
  intelligently. this is actually straightforward, unique card count
  combined with the number of cards yields only a few possibilities. }

function whatkindshort(uniq : integer; s : string) : integer;
begin
   whatkindshort := ckUnknown;
   case uniq of
     1 : whatkindshort := ckFive;  { xjjjj               -> xxxxx }
     2 : whatkindshort := ckFour;  { xyjjj, xxyjj, xxxyj -> xxxxy }
     3 : whatkindshort := ckThree; { xyzjj, xxyzj        -> xxxyz }
     4 : whatkindshort := ckPair;  { xyzqj               -> xxyzq }
   end;
end;


{ determine the hands kind or rank, full house, three of a kind, etc.
  this evaluates the full hand without considering jokers. the number
  of unique cards found in the hand provide a quick first pass to
  narrow choices down. from there count each card rank to finish
  figuring out the hand.

  unique ccounts of 1, 4, and 5 can each be of only one hand
  classification (five of a kind, full house, and trash hand). two
  distinct ranks are either a full house or four of a kind, while 3
  distinct ranks could be either two pair or three of a kind. }

function whatkindfull(uniq : integer; s : string) : integer;

var
   j, k, l: integer;
   c : char;

begin
   whatkindfull := ckUnknown;
   case uniq of
     1 : whatkindfull := ckFive;
     2 : begin
            j := 0;
            while whatkindfull = ckUnknown do begin
               j := j + 1;
               k := 0;
               c := s[j];
               for l := 1 to length(s) do
                  if c = s[l] then
                     k := k + 1;
               if (k = 2) or (k = 3) then
                  whatkindfull := ckFull;
               if k = 4 then
                  whatkindfull := ckFour;
            end;
         end;
     3 : begin
            j := 0;
            while whatkindfull = ckUnknown do begin
               j := j + 1;
               k := 0;
               c := s[j];
               for l := 1 to length(s) do
                  if c = s[l] then
                     k := k + 1;
               if k = 3 then
                  whatkindfull := ckThree;
               if k = 2 then
                  whatkindfull := ckTwoPair;
            end;
         end;
     4 : whatkindfull := ckPair;
     5 : whatkindfull := ckSingle;
   end; { case }
end;


{ determine the kind of hand (pair, full house, etc). the cards should
  be in order by rank. as with countranks, a full 5 card hand is not
  required for this check. this allows us to find the best hand before
  applying any jokers. }

function whatkind(uniq : integer; s : string) : integer;

begin
   if length(s) = MAXCARDS then
      whatkind := whatkindfull(uniq, s)
   else
      whatkind := whatkindshort(uniq, s);
end;


{ how many jokers are in the hand? }

function countjokers(s : string) : integer;

var
   i : integer;

begin
   countjokers := 0;
   for i := 0 to length(s) do
      if s[i] = JOKER then
         countjokers := countjokers + 1;
end;


{ build the new hand by adding jokers to maximize the hand value. }

function jokercards(s    : string;
                    kind : integer) : string;

var
   i, j : integer;

begin
   s := arrangecards(s);
   case kind of
     ckFive :
             begin                 { four jokers }
                while length(s) < MAXCARDS do { xJJJJ -> xxxxx }
                   s := s + s[1];
             end;
     ckFour :
             begin                 { three of a kind to start }
                while length(s) < MAXCARDS do
                   if s[1] = s[2] then   { xxxyJ -> xxxxy }
                      s := s[1] + s
                   else
                      s := s + s[2];      { xyyyJ -> xyyyy }
             end;
     ckFull :
             begin                 { two pair to start }
                if strindex(CARDRANK, s[1]) < strindex(CARDRANK, s[3]) then  { xxyyj -> xxyyy }
                   s := s + s[3]
                else
                   s := s[1] + s;    { xxyyj -> xxxyy }
             end;
     ckThree :
              begin                { a pair to start }
                 if s[1] = s[2] then
                    s := s[1] + s    { xxyzJ -> xxxyz }
                 else if s[2] = s[3] then
                    s := s[2] + s    { xyyzJ -> xyyyz }
                 else
                    s := s[3] + s;   { xyzzJ -> xyzzz }
              end;
     ckTwoPair :
                begin
                   j := 0;
                   for i := 1 to length(s) do
                      j := max(j, ord(s[i]));
                   s := chr(j) + s;
                   j := 0;
                   for i := 2 to length(s) do
                      j := max(j, ord(s[i]));
                   s := chr(j) + s;
                end;
     ckPair :
             begin
                j := 0;
                for i := 1 to length(s) do
                   j := max(j, ord(s[i]));
                s := chr(j) + s;
             end;
     { ckSingle is not possible, or is it? }
   end;

   jokercards := arrangecards(s);
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
      with hands[i] do begin
         s := aocread;
         cards := '';
         for p := 1 to MAXCARDS do
            cards := cards + s[p];
         p := p + 2;                 { first digit of bid }
         bid := str2int(s, p);
         sorted := arrangecards(cards);
         jokers := countjokers(cards);
         uniq := countranks(cards);
         if jokers = 0 then
            joked := sorted
         else if jokers = MAXCARDS then
            joked := 'AAAAA'
         else begin
            kind := whatkind(uniq - 1, substr(sorted, 1, MAXCARDS - jokers));
            joked := jokercards(substr(sorted, 1, MAXCARDS - jokers), kind);
         end;
         uniq := countranks(joked);
         kind := whatkind(uniq, joked);
      end;
   end;
   numhands := i;
end;


{ order the hands into ascending order by strength and return the sum
  of rank (subscript, but watch for ties) and bid. }

function parttwo : integer;

var
   i : integer;

begin
   init;
   load;
   parttwo := 0;
   sorthands;
   for i := 1 to numhands do
      parttwo := parttwo + (hands[i].bid * i);
end;


{ mainline }

var
   i : integer;

begin
   aocopen;
   i := parttwo;
   writeln('part two: ', i);
   aocclose;
end.
