{ padortrunc.p -- utility functions -- Troy Brumley blametroi@gmail.com }
{$ifndef IPADORTRUNC }
{$define IPADORTRUNC }

{$I truncright.p }
{$I padright.p }

{ expand or shrink a string to length l by either padding or truncation. }

function padortrunc(s : string;
                    l : integer;
                    e : boolean) : string;

begin
   if length(s) > l then
      padortrunc := truncright(s, l, e)
   else
      padortrunc := padright(s, l)
end;

{$endif}
{ end padortrunc.p }
