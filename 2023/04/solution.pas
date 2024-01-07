{ solution.pas -- scratch cards  -- Troy Brumley BlameTroi@gmail.com }

program solution;

{ expects to be compiled with the following two mode flags and command
line/fpc.cfg options of -Ciort -Sgix -Sh- for full checks, allowing
exceptions, goto, inlining, and prefer classic strings. }
{$longstrings off}            { to be sure, use AnsiString or pchar explicitly }
{$booleval on}                { standard pascal expects this }

type
   integer = int64;            { by default ints are 64 bits this decade}

{ advent of code 2023 day 4 -- scratch cards

  given a listing of scratch off cards and possible winners, score the
  cards and report the total. you later realize that the score actually
  just awards more cards to play. report the total number of scratch
  cards won under the rules. }


const
{$define xxxTESTING }
{$ifdef TESTING }
   MAXCARDS = 10;
   MAXWNOS = 5;
   MAXDNOS = 8;
{$else}
   MAXCARDS = 219;
   MAXWNOS = 10;
   MAXDNOS = 25;
{$endif}
   { input format: 'Card 9: ww ww ww ... | dd dd dd ...' }
   SPACE = ' ';
   COLON = ':';         { end of card number }
   BREAKER = ' |';      { separates winners from draws }
   CARDPFX = 'Card ';   { input marker }

type
   tDraw = 0 .. 99;      { null and 1-99 }
   tCard = record
              cno : integer;
              wno : array[1..MAXWNOS] of tDraw;
              dno : array[1..MAXDNOS] of tDraw;
              win : integer;      { wins in draws }
              pts : integer;      { points }
              cpy : integer;      { copies of card 1+ }
           end;
   tDeck = array[1..MAXCARDS] of tCard;


{$I aocinput.p }
{$I strbegins.p }
{$I str2int.p }
{$I substr.p }


{ initializers }
procedure initcard(var card : tCard);

var
   i : integer;

begin
   with card do begin
      cno := -1; { illegal cno means bad parse }
      for i := low(wno) to high(wno) do
         wno[i] := 0;
      for i := low(dno) to high(dno) do
         dno[i] := 0;
      win := 0;
      pts := 0;
      cpy := 1;           { not zero }
   end;
end;


procedure initdeck(var deck : tDeck);

var
   i : integer;

begin
   for i := 1 to MAXCARDS do
      initcard(deck[i]);
end;


{ parse the input card. two arrays, the first for winning numbers
  and the second for numbers drawn. }

procedure parsecard(s        : string;
                    var card : tCard);
var
   i, j : integer;       { subscripts }
   len  : integer;       { length of input }
   p    : integer;       { position }

begin
   with card do begin

      if not strbegins(s, CARDPFX) then
         exit;

      p := length(CARDPFX) + 1; { first digit cno }
      len := length(s);

      cno := 0;
      while (s[p] <> COLON) and (p < len) do begin
         if s[p] in ['0' .. '9'] then
            cno := cno * 10 + ord(s[p]) - ord('0');
         p := p + 1;
      end;

      p := p + 1;         { skip past colon to ' ##' }
      for i := low(wno) to high(wno) do begin
         if strbegins(substr(s, p, p + 3), BREAKER) then
            break;
         j := p + 1;
         if s[j] = SPACE then
            j := j + 1;
         wno[i] := str2int(s, j);
         p := p + 3;
      end; { for }

      p := p + length(BREAKER);     { skip to ' ##' }
      for i := low(dno) to high(dno) do begin
         if p >= len then
            break;
         j := p + 1;
         if s[j] = SPACE then
            j := j + 1;
         dno[i] := str2int(s, j);
         p := p + 3;
      end; { for }

   end; { with }
end; { parsecard }


{ score a card for part one: how many of the draws are in the winners?
  the score is 2 ** numwins-1. }

procedure scorecardone(var card : tCard);

var
   i : integer;
   s : set of tDraw;

begin
   with card do begin

      { a set is much quicker than repeated sequential
        scans }
      s := [];
      for i := low(wno) to high(wno) do
         include(s, wno[i]);

      { check draws }
      win := 0;
      pts := 0;
      for i := low(dno) to high(dno) do
         if dno[i] in s then
            win := win + 1;

      { and score }
      if win > 0 then
         pts := 1 shl (win - 1);

   end;
end;


{ score all the cards in the deck for part two }

procedure scorecardtwo(var deck : tDeck);

var
   i, j, k : integer;

begin
   for i := 1 to MAXCARDS do
      for j := 1 to deck[i].cpy do
         for k := i + 1 to i + deck[i].win do
            if k > MAXCARDS then
               break
            else
               deck[k].cpy := deck[k].cpy + 1;
end;


{ mainline -- this was done differently from prior aoc solutions. }

var
   i, j   : integer;
   deck   : array [1..MAXCARDS] of tCard;
   p1, p2 : integer;
   s      : string;

begin
   aocopen;
   initdeck(deck);

   { read deck, parse cards, and calculate score in part one }
   i := 0;
   while not aoceof do begin
      s := aocread;
      i := i + 1;
      parsecard(s, deck[i]);
      scorecardone(deck[i]);
   end;

   { cards are already parsed, calculate score in part two }
   scorecardtwo(deck);

   { calculate totals }
   p1 := 0; p2 := 0;
   for j := 1 to i do begin
      p1 := p1 + deck[j].pts;
      p2 := p2 + deck[j].cpy;
   end;

   writeln('part one: ', p1);
   writeln('part two: ', p2);
   aocclose;
end.

