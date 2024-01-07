{ solution.pas --  -- Troy Brumley BlameTroi@gmail.com }

program solution;

{ expects to be compiled with the following two mode flags and command
line/fpc.cfg options of -Ciort -Sgix -Sh- for full checks, allowing
exceptions, goto, inlining, and prefer classic strings. }
{$longstrings off}            { to be sure, use AnsiString or pchar explicitly }
{$booleval on}                { standard pascal expects this }

type
  integer = int64;            { by default ints are 64 bits this decade}

{ advent of code 2022 day 11 --
  general comment about the problem/solution }

const
{$define xTESTING }
{$ifdef TESTING }
  MAXDATA = 90;
{$else}
  MAXDATA = 140;
{$endif}


  { any domain specific types created for the problem/solution }

  { global variables, these are appropriate for most advent of code problems }



{$I aocinput.p }


{
  initialize global storage
}
procedure init;
begin
end;


{
  load the problem dataset
}
procedure load;
begin
   aocrewind;;
end;


{
  part one driver
}
function partone : integer;
begin
   init;
   load;
   partone := 0;
end;


{
  part two driver
}
function parttwo : integer;
begin
   init;
   load;
   parttwo := 0;
end;


{
  mainline
}

var
   i : integer;

begin
   aocopen;
   i := partone;
   writeln('part one: ', i);
   i := parttwo;
   writeln('part two: ', i);
   aocclose;
end.

