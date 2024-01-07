{ primer.p -- tools to find lcm gcm gcd and such -- troy brumley blametroi@gmail.com }

{$ifndef IPRIMER }
{$define IPRIMER }


function iseven(n : integer) : boolean; inline;

begin
   iseven := n = ((n shr 1) shl 1)
end;


function isprime(n : integer) : boolean;

var
  i : integer;

begin
   if n < 4 then begin
      isprime := true;
      exit;
   end;
   if iseven(n) then begin
      isprime := false;
      exit;
   end;
   isprime := true;
   for i := 3 to int64(sqrt(n)) do
      if (n mod i) = 0 then begin
         isprime := false;
         break;
      end;
end;


function nextprime(n : integer) : integer;

begin
   while not isprime(n) do
      n := n + 1;
   nextprime := n;
end;

{$endif}
{ end primer.p }
