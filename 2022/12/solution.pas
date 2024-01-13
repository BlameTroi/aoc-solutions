{ solution.pas --  -- Troy Brumley BlameTroi@gmail.com }

program solution;

{ expects to be compiled with the following two mode flags and command
  line/fpc.cfg options of -Ciort -Sgix -Sh- for full checks, allowing
  exceptions, goto, inlining, and prefer classic strings. }
{$LONGSTRINGS OFF}            { to be sure, use AnsiString or pchar explicitly }
{$BOOLEVAL ON}                { standard pascal expects this }

type
   integer = int64;            { by default ints are 64 bits this decade}

{ advent of code 2022 day 12 -- hill climbing algorithm

  go from S to E without breaking out your climbing gear. this
  means you can only increase altitude one level per move. you
  can decrease any number of levels though.

  the grid is an altitude map or contour map. a is the lowest
  altitude, z is the highest. we assume that S <= a, and
  E > z from the problem statement.

  our sample input:

  Sabqponm
  abcryxxl
  accszExk
  acctuvwj
  abdefghi

  you can move only in the four cardinal directions.
}


{$I aocinput.p }
{$I int2str.p }
{$I min.p }
{$I max.p }


const
   { over-allocating here, there's no need for a tight fit for the
     smaller test file in the array, it's all small. }
   MAXCOLS = 96;
   MAXROWS = 96;


type
   pcell    = ^tcell;
   tcell    = record                   { information this point in the maze }
                 r, c       : integer; { who am i, where am i }
                 altitude   : integer; { a variable permeability wall }
                 distance   : integer;
                 n, e, s, w : boolean; { is path available }
                 glyph      : char;    { displayable }
                 move       : char;    { move logged for final pass }
              end;
   pcellref = ^tcellref;
   tcellref = record
                 cell : pcell;
                 next : pcellref;
              end;

var
   grid           : array [0..MAXROWS+1, 0..MAXCOLS+1] of tcell;  { ends are for coding convenience, data is 1..max }
   begr, begc     : integer;
   endr, endc     : integer;
   minalt, maxalt : integer;
   workq, visited : pcellref;


{ set initial state }

procedure init;

var
   r, c : integer;

begin
   { fill in the defaults }
   for r := 0 to MAXROWS+1 do
      for c := 0 to MAXCOLS+1 do begin
         grid[r, c].r := r; grid[r, c].c := c;
         with grid[r, c] do begin
            n := false; s := false; e := false; w := false;
            altitude := 100;
            glyph := '~';
            move := ' ';
            distance := high(int64);
         end;
      end;
   begr := -1; begc := -1;
   endr := -1; endc := -1;
   minalt := high(integer);
   maxalt := -high(integer);
end;


{ load the maze }

procedure load;

var
   i, j : integer;
   t    : string;

begin
   aocrewind;
   i := 0;
   while not aoceof do begin
      t := aocread;
      if t = '' then
         continue;
      i := i + 1;
      for j := 1 to length(t) do
         with grid[i, j] do begin
            r := i; c := j;
            glyph := t[j];
            if glyph = 'S' then begin
               begr := r; begc := c; altitude := 0;;
               continue;
            end;
            if glyph = 'E' then begin
               endr := r; endc := c; altitude := ord('z') - ord('a');
               continue;
            end;
            altitude := ord(glyph) - ord('a');
            minalt := min(minalt, altitude);
            maxalt := max(maxalt, altitude);
         end;
   end;
   { determine available directions based on altitudes }
   for i := 1 to MAXROWS do
      for j := 1 to MAXCOLS do
         with grid[i, j] do begin
            n := (grid[i-1, j].altitude - altitude) < 2;
            e := (grid[i, j+1].altitude - altitude) < 2;
            s := (grid[i+1, j].altitude - altitude) < 2;
            w := (grid[i, j-1].altitude - altitude) < 2;
         end;
   if (begr = -1) or (endr = -1) then
      aochalt('*** start or end node not found ***');
   workq := nil;
   visited := nil;
end;


{ release all refs on the passed list. does not clear the list anchor. }
procedure freerefs(p : pcellref);

var
   n : pcellref;

begin
   while p <> nil do begin
      n := p^.next;
      dispose(p);
      p := n;
   end;
end;


{ create a new reference to a cell in the grid. }

function makeref(row, col : integer; dist : integer) : pcellref;

begin
   new(makeref);
   makeref^.cell := @grid[row, col];
   grid[row, col].distance := dist;
   makeref^.next := nil;
end;


{ is a cell reference with these coordinates on the passed list? }

function alreadyon(p : pcellref; nr, nc : integer) : boolean;

begin
   alreadyon := true;
   while p <> nil do
      if (p^.cell^.r = nr) and (p^.cell^.c = nc) then
         exit
      else
         p := p^.next;
   alreadyon := false;
end;


{ add this reference to the end of the work queue. }
procedure addref(ref : pcellref);

var
   p : pcellref;

begin
   if workq = nil then begin
      workq := ref;
      exit;
   end;
   p := workq;
   while p^.next <> nil do
      p := p^.next;
   p^.next := ref;
end;


{ search to find the path to the end of the maze }

function search(row, col : integer) : integer;

var
   nr, nc : integer;
   curr   : pcellref;
   ref    : pcellref;

begin
   { empty work queue and visited list }
   freerefs(workq);
   workq := nil;
   freerefs(visited);
   visited := nil;

   { reset distances ? }

   { put first cell reference on workq }
   curr := makeref(row, col, 0);
   curr^.next := workq;
   workq := curr;

   { and search until workq exhausted }
   while workq <> nil do begin
      { pop }
      curr := workq;
      workq := workq^.next;

      { push }
      curr^.next := visited;
      visited := curr;

      { checking each direction. my backing data structure trapped me
        into repetition, but it's not so bad. }

      { north }
      if curr^.cell^.n then begin
         nr := curr^.cell^.r - 1; nc := curr^.cell^.c;
         if grid[nr, nc].glyph = 'E' then begin
            search := curr^.cell^.distance + 1;
            exit;
         end;
         if not (alreadyon(workq, nr, nc) or alreadyon(visited, nr, nc)) then begin
            ref := makeref(nr, nc, curr^.cell^.distance + 1);
            addref(ref);
         end;
      end;

      { east }
      if curr^.cell^.e then begin
         nr := curr^.cell^.r; nc := curr^.cell^.c + 1;
         if grid[nr, nc].glyph = 'E' then begin
            search := curr^.cell^.distance + 1;
            exit;
         end;
         if not (alreadyon(workq, nr, nc) or alreadyon(visited, nr, nc)) then begin
            ref := makeref(nr, nc, curr^.cell^.distance + 1);
            addref(ref);
         end;
      end;

      { south }
      if curr^.cell^.s then begin
         nr := curr^.cell^.r + 1; nc := curr^.cell^.c;
         if grid[nr, nc].glyph = 'E' then begin
            search := curr^.cell^.distance + 1;
            exit;
         end;
         if not (alreadyon(workq, nr, nc) or alreadyon(visited, nr, nc)) then begin
            ref := makeref(nr, nc, curr^.cell^.distance + 1);
            addref(ref);
         end;
      end;

      { west }
      if curr^.cell^.w then begin
         nr := curr^.cell^.r; nc := curr^.cell^.c - 1;
         if grid[nr, nc].glyph = 'E' then begin
            search := curr^.cell^.distance + 1;
            exit;
         end;
         if not (alreadyon(workq, nr, nc) or alreadyon(visited, nr, nc)) then begin
            ref := makeref(nr, nc, curr^.cell^.distance + 1);
            addref(ref);
         end;
      end;

      { and keep checking until workq is empty }
   end;
end;


{ mainline }
var
   i        : integer;
   row, col : integer;

begin
   aocopen;
   init;
   load;
   i := search(begr, begc);
   writeln('part one answer : ', i);
   i := high(int64);
   for row := 1 to MAXROWS do
      for col := 1 to MAXCOLS do
         if grid[row, col].glyph = 'a' then
            i := min(search(row, col), i);
   writeln('part two answer : ', i);
   aocclose;
end.

{ end solution.pas }
