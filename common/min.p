{ min.p -- utility functions -- Troy Brumley blametroi@gmail.com }
{$ifndef IMIN }
{$define IMIN }

{ smallest of two integers }

function min(a, b : integer) : integer; inline;

begin
   if a < b then min := a else min := b
end;

{$endif}
{ end min.p }
