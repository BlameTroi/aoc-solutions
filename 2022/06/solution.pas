{ solution.pas -- tuning trouble -- Troy Brumley blametroi@gmail.com }

program solution;
{$longstrings off}
{$booleval on}

type
   integer = int64;

{$I aocinput.p }


{ advent of code 2022 day 6 -- tuning trouble
  identify parts of a message buffer. }


const
   START_PACKET_LEN  = 4;
   START_MESSAGE_LEN = 14;

{ global variables are fine for this }

var
   packetbuffer  : array[1..START_PACKET_LEN] of char;
   messagebuffer : array[1..START_MESSAGE_LEN] of char;


{ initialize global variables to empty }

procedure init;

var
   i : integer;

begin
   for i := 1 to START_PACKET_LEN do
      packetbuffer[i] := chr(0);
   for i := 1 to START_MESSAGE_LEN do
      messagebuffer[i] := chr(0);
end;


{ any repeats in buffer? characters are written into the buffers
  (both packet and message) in a circle, so the buffers are just
  big enough to hold the start markers. }

function packetbuffermark : boolean;

var
   i, j : integer;
   c    : char;

begin
   packetbuffermark := true;
   for i := 1 to START_PACKET_LEN do begin
      if packetbuffer[i] = chr(0) then begin
         packetbuffermark := false;
         break;
      end;
      c := packetbuffer[i];
      for j := 1 to START_PACKET_LEN do begin
         if i = j then
            continue;
         if c = packetbuffer[j] then begin
            packetbuffermark := false;
            exit;
         end;
      end;
   end;
end;


{ watch the input stream and report the position of the first run
  of non-repeating characters. the run length is START_PACKET_LEN.
  this is the first problem i've seen where the input isn't split
  into lines. }

function partone : integer;

var
   i : integer;
   c : char;

begin
   init;
   aocrewind;
   i := 0;
   while (not aoceof) and (not packetbuffermark) do begin
      if aoceol then
         break;
      read(input, c);
      i := i + 1;
      if not (c in ['a'..'z']) then
         break;
      packetbuffer[(i mod START_PACKET_LEN) + 1] := c;
   end;
   partone := i;
end;


{ message buffer mark is longer than the packet buffer mark, but
otherwise treated the same. }

function messagebuffermark : boolean;

var
   i, j : integer;
   c    : char;

begin
   messagebuffermark := true;
   for i := 1 to START_MESSAGE_LEN do begin
      if messagebuffer[i] = chr(0) then begin
         messagebuffermark := false;
         break;
      end;
      c := messagebuffer[i];
      for j := 1 to START_MESSAGE_LEN do begin
         if i = j then
            continue;
         if c = messagebuffer[j] then begin
            messagebuffermark := false;
            exit;
         end;
      end;
   end;
end;


{ same idea as part one but look for a message buffer marker of
  fourteen distinct characters. the processing is otherwise
  identical to partone and these could be combined so the
  input stream is only passed once, but that's not needed for
  this application. }

function parttwo : integer;

var
   i : integer;
   c : char;

begin
   init;
   aocrewind;
   i := 0;
   while (not aoceof) and (not messagebuffermark) do begin
      if aoceol then
         break;
      c := aocreadch;
      i := i + 1;
      if not (c in ['a'..'z']) then
         break;
      messagebuffer[(i mod START_MESSAGE_LEN) + 1] := c;
   end;
   parttwo := i;
end;


{ the mainline driver for a day's solution. }

procedure mainline;

var
   i : integer;

begin
   aocopen;
   init;
   i := partone;
   writeln('part one answer : ', i);
   i := parttwo;
   writeln('part two answer : ', i);
   aocclose;
end;


{ minimal boot to mainline }
begin
   mainline;
end.
