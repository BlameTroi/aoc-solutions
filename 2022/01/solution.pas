{ solution.pas -- advent of code starting template -- Troy Brumley blametroi@gmail.com }

{ 2022 day 01 -- calorie hoarders }

program solution;

{$longstrings off}
{$booleval on}
type
   integer = int64;

const
   TOP_N = 3;                  { for problems that involve the top or bottom 'n' items }

{ the input dataset for the days problem, one per line of input. the
  original input line and its parsed value(s). }

type
   pData = ^tData;
   tData = record
              fwd : pData;      { ch-ch-ch }
              bwd : pData;      { chains }
              num : integer;    { line number in source }
              rec : string;     { original input }
              n   : integer;    { the parsed data }
           end;

{ problems that require looking at the largest or smallest 'n' items,
  a structure and some supporting procedures to keep track of them.

  the 0 entry in the items array is a bucket for ease of updating. }

tSeeking = (largest, smallest);
   tTopOrBottom = record
                     items   : array[0..TOP_N] of integer;
                     used    : 0..TOP_N;
                     mx      : integer;
                     mn      : integer;
                     seeking : tSeeking
                  end;

{$I aocinput.p }
{$I aochalt.p }
{$I str2int.p }


{ zero out tracking }

procedure initTopOrBottom(var d : tTopOrBottom;
                              s : tSeeking;
                              v : integer);
var
   i : integer;

begin
   with d do begin
      for i := low(items) to high(items) do
         items[i] := v;
      used := 0;
      mx := 1; { yes, there is no such item yet }
      mn := 1;
      seeking := s
   end
end;


{ find the largest and smallest item subscripts. }

procedure findTopAndBottom(var d: tTopOrBottom);

var
  i : integer;

begin
   with d do
      for i := 1 to used do begin
         if items[i] < items[mn] then
            mn := i;
         if items[i] > items[mx] then
            mx := i
      end
end;


{ replace the lowest or highest value in tracking with the new value
  if it is in bounds. }

procedure updateTopOrBottom(var d : tTopOrBottom;
                                v : integer);
var
   i : integer;
   s : boolean;

begin
   with d do begin

      { first n items are basically free }

      if used < high(items) then begin
         used := used + 1;
         items[used] := v;
         exit
      end;

      findTopAndBottom(d);

      { which end do we seek? }

      if seeking = largest then begin
         if v < items[mn] then
            exit;
         items[0] := v;
         repeat
            s := false;
            for i := 0 to high(items) - 1 do
               if items[i] > items[i+1] then begin
                  v := items[i];
                  items[i] := items[i+1];
                  items[i+1] := v;
                  s := true
               end
         until not s;
      end;

      { the following 'if seeking' isn't needed, but i like the symmetry. }

      if seeking = smallest then begin
         if v > items[mx] then
            exit;
         items[0] := v;
         repeat
            s := false;
            for i := 0 to high(items) - 1 do
               if items[i] < items[i+1] then begin
                  v := items[i];
                  items[i] := items[i+1];
                  items[i+1] := v;
                  s := true
               end
         until not s;
      end;
   end;
end;


{ part one -- subtotal values and find the largest sub total. blank
  lines are the break. the accumulation of data for part two is done
  here as each subtotal is accumulated. }

function partone(    pd : pData;
                 var tb : tTopOrBottom) : integer;

var
   gt, st, mst : integer;
   i           : integer;

begin
   gt := 0;
   st := 0;
   mst := 0;

   i := 0;
   while pd <> nil do
      with pd^ do begin
         i := i + 1;
         if rec = '' then begin
            if st > mst then
               mst := st;
            updateTopOrBottom(tb, st);
            st := 0;
            pd := fwd;
            continue;
         end; { if rec = '' }
         gt := gt + n;
         st := st + n;
         pd := pd^.fwd;
      end; { with pd^ }

   if st > mst then
      mst := st;

   partone := mst;

end;


{ part two -- the total of the three largest subtotals. }

function parttwo(    pd : pData;
                 var tb : tTopOrBottom) : integer;

var
   t : integer;
   i : integer;

begin
   t := 0;
   for i := 1 to TOP_N do begin
      t := t + tb.items[i];
   end;
   parttwo := t;
end;


{ mainline }
procedure mainline;

var
   tb    : tTopOrBottom;
   first : pData;
   prior : pData;
   curr  : pData;
   i     : integer;
   p     : integer;
   s     : string;

begin
   aocopen;

   first := nil;
   prior := nil;
   i := 0;
   while not aoceof do begin
      i := i + 1;
      s := aocread;
      new(curr);
      curr^.fwd := nil;
      curr^.bwd := prior;
      curr^.num := i;
      curr^.rec := s;
      if s <> '' then
         p := 1;
      curr^.n := str2int(s, p);
      if first = nil then
         first := curr
      else
         prior^.fwd := curr;
      prior := curr
   end; { while not eof }

   initTopOrBottom(tb, largest, 0);
   i := partone(first, tb);
   writeln('part one: ', i);
   i := parttwo(first, tb);
   writeln('part two: ', i);
   aocclose;

end;

{ minimal boot to mainline }
begin
  mainline;
end.
