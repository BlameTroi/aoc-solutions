{ solution.pas -- monkey business -- Troy Brumley BlameTroi@gmail.com }

program solution;

{ expects to be compiled with the following two mode flags and command
line/fpc.cfg options of -Ciort -Sgix -Sh- for full checks, allowing
exceptions, goto, inlining, and prefer classic strings. }
{$LONGSTRINGS OFF}            { to be sure, use AnsiString or pchar explicitly }
{$BOOLEVAL ON}                { standard pascal expects this }


{ advent of code 2022 day 11 -- monkey business

  which monkeys have the most important stuff? }

type
   integer = int64;

const
   { input markers in expected order }
   cmonkey    = 'Monkey ';                        { n: }
   cstart     = '  Starting items: ';             { n (, n)* }
   coper      = '  Operation: new = old ';        { [+*] n }
   ctest      = '  Test: divisible by ';          { n }
   cift       = '    If true: throw to monkey ';  { n }
   ciff       = '    If false: throw to monkey '; { n }
   cold       = ' old';
{$define xxxTESTING }
{$ifdef TESTING }
   MAXITEMS   = 10;             { 10 test, 36 live }
   LASTMONKEY = 3;              { 0-3 test, 0-7 live }
{$else}
   MAXITEMS   = 36;             { 36 live }
   LASTMONKEY = 7;              { 0-7 live }
{$endif}
   MINROUNDS  = 20;             { for both test and live }
   MAXROUNDS  = 10000;          { for both test and live }
   CRELIEF    = 3;              { a scaling value for min rounds }


type
   tmonkey = record
                id          : char;        { will use index }
                numitems    : integer;      { how many items }
                priority    : array[1..MAXITEMS] of integer;
                operation   : char;        { + or * }
                opervalue   : integer;      { 0 means old, otherwise integer }
                testdivisor : integer;      { priority test value}
                throwtrue   : integer;      { index of true dest }
                throwfalse  : integer;      { index of false dest}
                examined    : integer;      { how many items examined }
             end;

var
   monkey : array [0..LASTMONKEY] of tmonkey;


{$I aocinput.p }
{$I str2int.p }
{$I strbegins.p }
{$I int2str.p }


{ initialize }

procedure init;

var
   i, j : integer;

begin
   for i := 0 to LASTMONKEY do
      with monkey[i] do begin
         id := ' ';
         numitems := 0;
         for j := 1 to MAXITEMS do
            priority[j] := 0;
         operation := ' ';
         opervalue := 0;
         testdivisor := 0;
         throwtrue := 0;
         throwfalse := 0;
         examined := 0;
      end;
end;


{ load and parse }

procedure load;

var
   i, j, p : integer;
   s       : string;

begin
   aocrewind;
   i := 0;
   while not aoceof do begin
      s := aocreadexpecting(cmonkey);
      with monkey[i] do begin
         id := s[length(cmonkey) + 1];

         s := aocreadexpecting(cstart);
         p := length(cstart) + 1;
         j := 0;
         while p < length(s) do begin
            j := j + 1;
            priority[j] := str2int(s, p);
            p := p + 2; { skip ', ' }
         end;
         numitems := j;

         s := aocreadexpecting(coper);
         operation := s[length(coper) + 1];
         p := length(coper) + 3; { skip past ' + ' }
         if s[p] in ['0'..'9'] then
            opervalue := str2int(s, p)
         else
            opervalue := 0; { signifies old priority }

         s := aocreadexpecting(ctest);
         p := length(ctest) + 1; { skip to digits }
         testdivisor := str2int(s, p);

         s := aocreadexpecting(cift);
         p := length(cift) + 1;
         throwtrue := str2int(s, p);

         s := aocreadexpecting(ciff);
         p := length(ciff) + 1;
         throwfalse := str2int(s, p);

         if not aoceof then
            s := aocreadexpecting('');
      end;
      i := i + 1;
   end;
end;


{ report on the monkeys and the items }

procedure report;

var
   i, j : integer;
   s    : string;

begin
   for i := 0 to LASTMONKEY do
      with monkey[i] do begin
         aoclog('');
         aoclog('monkey ' + id);
         s := 'holding ' + int2str(numitems) + ' of priorities [';
         for j := 1 to numitems do
            s := s + ' ' + int2str(priority[j]);
         s := s + ' ]';
         aoclog(s);
         s := 'operation is new = old ' + operation + ' ';
         if opervalue = 0 then
            s := s + 'old'
         else
            s := s + int2str(opervalue);
         aoclog(s);
         aoclog('test is divisible by ' + int2str(testdivisor));
         aoclog('when true throws to ' + int2str(throwtrue));
         aoclog('when false throws to ' + int2str(throwfalse));
         aoclog('items examined ' + int2str(examined));
      end;
   aoclog('');
   aoclogflush;
end;


{ the monkey wonders if you're worried? are you worried? the
  operation is applied and then the new priority is divided
  by the relief value and rounded down. }

procedure dooperation(m      : integer;  { monkey }
                      i      : integer;  { item number }
                      relief : integer); { how you feeling, bunky? }
var
   op, np : integer;

begin
   with monkey[m] do begin
      op := priority[i];
      if opervalue = 0 then
         np := op
      else
         np := opervalue;
      if operation = '+' then
         priority[i] := op + np
      else if operation = '*' then
         priority[i] := op * np
      else
         aochalt('*** error illegal operation ' + operation + ' for monkey ' + int2str(m));
      { take the mod of keep the value under the limit when doing prime multiples }
      if relief = CRELIEF then
         priority[i] := priority[i] div relief
      else
         priority[i] := priority[i] mod relief;
   end;
end;


{ mine! mine! add the incoming priority to the end of this
  monkey's list. }

procedure catchitem(m : integer;
                    n : integer);  { item priority value }
begin
   with monkey[m] do begin
      numitems := numitems + 1;
      priority[numitems] := n;
   end;
end;


{ the monkey makes up his mind. }

function dotest(m : integer;
                i : integer) : boolean;
begin
   with monkey[m] do
      dotest := (priority[i] mod testdivisor) = 0;
end;


{ calculate a relief value. i'm not much on number theory, but
  everything was a prime number so after thinking about it for
  a while i still had nothing and checked reddit. this is similar
  to the ghost walk problem in that we're dealing with prime
  multiples. }

function calculatedrelief : integer;

var
   i : integer;

begin
   calculatedrelief := 1;
   for i := 0 to LASTMONKEY do
      calculatedrelief := calculatedrelief * monkey[i].testdivisor;
end;

{ round and round we go. }

procedure goaround(rounds : integer;
                   relief : integer);

var
   i, j, k : integer;

begin
   for i := 1 to rounds do begin
      for j := 0 to LASTMONKEY do
         with monkey[j] do begin
            examined := examined + numitems;
            for k := 1 to numitems do
               dooperation(j, k, relief);
            for k := 1 to numitems do
               if dotest(j, k) then
                  catchitem(throwtrue, priority[k])
               else
                  catchitem(throwfalse, priority[k]);
            numitems := 0;
         end;
      aoclog('');
      aoclog('round ' + int2str(i));
      report;
   end;
end;


{ find the number of items examined by the monkeys who touched your
  stuff most frequently. bad monkeys. report the values of the top
  two multiplied together. }

function multiplytoptwo : integer;

var
   i, j, k : integer;

begin
   j := 0;  { index of biggest found }
   k := 1;  { index of second biggest found }
      if monkey[j].examined < monkey[k].examined then begin  { keep correctly ordered }
         i := j; j := k; k := i;
      end;
   for i := 2 to LASTMONKEY do begin
      if monkey[i].examined <= monkey[k].examined then       { too small }
         continue;
      if monkey[i].examined <= monkey[j].examined then       { between j and k }
         k := i
      else begin
         k := j;
         j := i;
      end;
   end;
   aoclog('top two at ' + int2str(j) + ' ' + int2str(k));
   multiplytoptwo := monkey[j].examined * monkey[k].examined;
end;


begin
   aocopen;
   aocnolog;
   init;
   load;
   report;
   goaround(MINROUNDS, CRELIEF);
   writeln('part one answer : ', multiplytoptwo);
   init;
   load;
   goaround(MAXROUNDS, calculatedrelief);
   writeln('part two answer : ', multiplytoptwo);
   aocclose;
end.

{ end solution.pas }
