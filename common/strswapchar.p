{ strswapchar.p -- swap two characters in string -- troy brumley blametroi@gmail.com }
{$ifndef ISTRSWAPCHAR }
{$define ISTRSWAPCHAR }

{ exchange characers by byte index in a string. quietly
  fails on error by returning the original string. }

function strswapchar(s : string;
                     p1, p2 : integer) : string;
var
   c : char;

begin
   strswapchar := s;
   if (p1 > length(s)) or (p2 > length(s)) or (p1 < 1) or (p2 < 1) then
      exit;
   c := strswapchar[p1];
   strswapchar[p1] := strswapchar[p2];
   strswapchar[p2] := c;
end;

{$endif}
{ end strswapchar.p }
