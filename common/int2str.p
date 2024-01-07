{ int2str.p -- utility functions -- Troy Brumley blametroi@gmail.com }
{$ifndef IINT2STR }
{$define IINT2STR }

{ convert integer to string. if the input is negative a minus
  sign '-' will prefix the result. }

function int2str(i : integer) : string;

var
  n : boolean;

begin
   n := i < 0;
   i := abs(i);
   int2str := '';
   while i > 0 do begin
      int2str := chr(ord('0') + (i mod 10)) + int2str;
      i := i div 10
   end;
   if int2str = '' then
      int2str := '0';
   if n then
      int2str := '-' + int2str;
end;

{$endif}
{ end int2str.p }
