{ gridset2str.p -- utility functions -- Troy Brumley blametroi@gmail.com }
{$ifndef IGRIDSET2STR }
{$define IGRIDSET2STR }

{$I int2str.p }

{ expects type tGridset, a set of integers. constants tgridnummin
  and max are one less and one more than the actual legal values
  of beg and end. this is a convenience for looping and breaks. }

{ returns a string in pascalish syntax of the grid set with runs
  properly compressed. }

function gridset2str(gs : tGridSet) : string;

var
   i       : tGridNum;
   runlen  : tGridNum;
   inrun   : boolean;

begin
   gridset2str := '[ ';
   runlen := 0;
   inrun := false;
   for i := tGridNumMin to tGridNumMax do begin
      { check for end of run }
      if not (i in gs) then begin
         if not inrun then
            continue;
         inrun := false;
         if runlen > 1 then
            gridset2str := gridset2str + '-' + int2str(i - 1);
         runlen := 0;
         continue;
      end; { run break check }
      { if not in run, output, and assume a run starts }
      if not inrun then begin
         if length(gridset2str) <> 2 then
            gridset2str := gridset2str + ', ';
         gridset2str := gridset2str + int2str(i);
         inrun := true;
         runlen := 1;
         continue;
      end; { if not inrun }
      runlen := runlen + 1;
   end; { for }
   gridset2str := gridset2str + ' ]';
   gridset2str := gridset2str;
end;

{$endif}
{ end gridset2str.p }
