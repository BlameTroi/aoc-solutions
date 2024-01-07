{ solution.pas -- cathode ray tube -- Troy Brumley blametroi@gmail.com }

program solution;

{$longstrings off}
{$booleval on}

type
   integer = int64;

{ advent of code 2022 day 10 -- cathode ray tube

  simulate a program execution to drive a small crt display

  noop
  addx 3
  addx -5

  initial x = 1
  addx takes 2 cycles to complete, at the end of the cycles, x is
  increased by value (which can be negative) noop takes 1 cycle
  and has no other effect

  for part one we take a sum of the signal strength (x * cycles),
  and for part two we use the value of x combined with the cycle
  to determine if a pixel on the display should be lit.
 }

const
   OPC_NOP          = 'n';
   OPC_ADX          = 'a';
   OPC_INV          = '-';
   OPC_EOJ          = '|';
   MAXLEN           = 150;
   PIXEL_ON         = '#';
   PIXEL_OFF        = '.';
   SCANLINE         = 40;
   CYCLES           = 6*SCANLINE;

type
   tTrace    = record
                  opcode : char;
                  oprand : integer;
                  scycle : integer;
                  ecycle : integer;
                  sx     : integer;
                  ex     : integer;
               end;

{ global variables are fine for this }

var
   trace  : array[-1..MAXLEN] of tTrace;
   pixels : array[0..CYCLES-1] of char;


{$I str2int.p }
{$I int2str.p }
{$I padright.p }
{$I aocinput.p }


{ clear the 'screen' }

procedure cls;

var
   i : integer;

begin
   for i := 0 to CYCLES-1 do
      pixels[i] := PIXEL_OFF;
end;


{ clear out storage }

procedure init;

var
   i : integer;

begin
   for i := -1 to MAXLEN do
      with trace[i] do begin
         opcode := OPC_INV;
         oprand := 0;
         scycle := 0;
         ecycle := 0;
         sx     := 0;
         ex     := 0;
      end;
   trace[-1].sx := 1;
   trace[-1].ex := 1;
   cls;
end;


{ initial
  program
  load }

procedure load;

var
   i : integer;
   p : integer;
   s : string;

begin
   init;
   aocrewind;
   i := -1;
   while not aoceof do begin
      s := aocread;
      if (s = '') then
         continue;
      if s[1] = '#' then
         continue;
      i := i + 1;
      trace[i].opcode := s[1];
      if trace[i].opcode = OPC_ADX then begin
         p := 6;
         trace[i].oprand := str2int(s, p);
      end;
   end;
   trace[i+1].opcode := OPC_EOJ;
end;


{ this includes support for a display of the trace, not useful once
  the data has been shrouded for final runs. }

function asminst(opc : char;
                 opr : integer) : string;
var
   s : string;

begin
   case opc of
     OPC_NOP : s := ' noop';
     OPC_ADX : s := ' addx ';
     OPC_INV : s := ' ****';
     OPC_EOJ : s := ' END';
   else
      s := ' ????';
   end;
   if opc = OPC_ADX then
      s := s + int2str(opr);
   asminst := padright(s, 16);
end;


{ execute the program and update the trace to show cycles used and the
  value of x at the start and end of the instruction. }

procedure runner;

var
   i : integer;

begin
   i := -1;
   while trace[i].opcode <> OPC_EOJ do begin
      i := i + 1;
      trace[i].scycle := trace[i-1].ecycle;
      trace[i].sx := trace[i-1].ex;
      case trace[i].opcode of
        OPC_INV :
        aochalt('error! invalid opcode at ' + int2str(i));
        OPC_NOP : begin
           trace[i].ex := trace[i].sx;
           trace[i].ecycle := trace[i].scycle + 1;
        end;
        OPC_ADX : begin
           trace[i].ex := trace[i].sx + trace[i].oprand;
           trace[i].ecycle := trace[i].scycle + 2;
        end;
      end;
   end;
end;


{ find the value of x at a certain cycle time. }

function xat(c : integer) : integer;

var
   i : integer;

begin
   i := -1;
   repeat
      i := i + 1
   until (c >= trace[i].scycle) and (c <= trace[i].ecycle);
   xat := trace[i].sx;
end;


{ return the signal strength at a particular cycle time }

function sigstr(c : integer) : integer;

begin
   sigstr := xat(c) * c;
end;


{ for part one report a sum of a few signal strengths

  signal strengths is x * cycle number
  sum signal strengths at 20, 60, 80, 120, 140, 180, and 220 cycles.}

function partone  : integer;

begin
   load;
   runner;
   partone := sigstr(20) + sigstr(60) + sigstr(100) + sigstr(140) + sigstr(180) + sigstr(220);
end;


{ set pixels on and off on the 'screen', where the value of x at a
  particular cycle time coincides with the passing of a 'sprite'. }

procedure scanner;

var
   i, x, o : integer;

begin
   for i := 1 to 240 do begin
      x := xat(i);
      o := i mod 40 - 1;
      if (o > x-2) and (o < x+2) then
         pixels[i-1] := PIXEL_ON
      else
         pixels[i-1] := PIXEL_OFF;
   end;
end;


{ print the screen display for examination. }

procedure printscreen;

var
   i : integer;

begin
   writeln;
   write('screen:');
   for i := 0 to CYCLES-1 do begin
      if i mod SCANLINE = 0 then
         writeln;
      write(pixels[i]);
   end;
   writeln;
   writeln;
end;


{ for part two show what would be displayed on the screen. with each
  cycle the scan position is positioned on a pixel. if the sprite (a 3
  pixel wide area, centered by the current value of x) has any of its
  area over the a pixel at that time, the pixel is turned on. }

function parttwo : integer; { no meaningful value returned }

begin
   parttwo := 0;
   load;
   runner;
   scanner;
end;


{ the mainline driver for a day's solution. }

procedure mainline;

var
   i : integer;

begin
   aocopen;
   i := partone;
   writeln('part one answer : ', i);
   i := parttwo;
   writeln('part two answer : ', i);
   printscreen;
   aocclose;
end;


{ minimal boot to mainline }
begin
  mainline;
end.
