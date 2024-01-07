{ max.p -- utility functions -- Troy Brumley blametroi@gmail.com }
{$ifndef IIMAX }
{$define IIMAX }

{ largest of two integers }

function max(a, b : integer) : integer; inline;

begin
   if a > b then max := a else max := b
end;

{$endif}
{ end max.p }
