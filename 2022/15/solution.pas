{ solution.pas -- beacon exclusion zone -- Troy Brumley BlameTroi@gmail.com }

program solution;

{ expects to be compiled with the following two mode flags and command
  line/fpc.cfg options of -Ciort -Sgix -Sh- for full checks, allowing
  exceptions, goto, inlining, and prefer classic strings. }

{$LONGSTRINGS OFF}            { to be sure, use AnsiString or pchar explicitly }
{$BOOLEVAL ON}                { standard pascal expects this }

type
   integer = int64;            { by default ints are 64 bits this decade}

{========================================================================}
{ advent of code 2022 day 15 -- beacon exclusion zone                    }
{                                                                        }
{ given sensors that can identify the closest beacon, cast a net of      }
{ sensors to map beacons. then determine where a beacon can not be in a  }
{ specific row.                                                          }
{========================================================================}


{========================================================================}
{ forward definitions and common code includes                           }
{========================================================================}

{$I aocinput.p}
{$I str2int.p}
{$I int2str.p}
{$I min.p}
{$I max.p}
{$I strbegins.p}

{ the always present framework }
procedure init; forward;
procedure load; forward;
procedure display; forward;
function partone : integer; forward;
function parttwo : integer; forward;

{ problem specific forwards }



{========================================================================}
{ standard mainline for advent of code                                   }
{========================================================================}

procedure mainline;
var
   i : integer;
begin
   aocopen;

   init;
   aocrewind;
   load;
   display;
   i := partone;
   writeln('part one answer : ', i);

   init;
   aocrewind;
   load;
   i := parttwo;
   writeln('part two answer : ', i);

   aocclose;
end;

{========================================================================}
{ part one                                                               }
{========================================================================}

function partone : integer;
begin
   partone := -1;
end;


{========================================================================}
{ part two                                                               }
{========================================================================}

function parttwo : integer;
begin
   parttwo := -1;
end;


{========================================================================}
{ the standard clear, load, and print helpers                            }
{========================================================================}

procedure init;
begin
end;

procedure load;
begin
end;

procedure display;
begin
end;

{ and just go to the real mainline }

begin
   mainline;
end.

{ end solution.pas }
