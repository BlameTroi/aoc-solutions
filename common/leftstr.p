{ leftstr.p -- utility functions -- troy brumley blametroi@gmail.com }
{$ifndef ILEFTSTR }
{$define ILEFTSTR }

{ return the leftmost l characters of a string }

function leftstr(s : string;
                 l : integer) : string;
var
   i : integer;

begin
   leftstr := s;
   if length(s) <= l then
      exit;
   leftstr := '';
   for i := 1 to l do
      leftstr := leftstr + s[i]
end;

{$endif}
{ end leftstr.p }
