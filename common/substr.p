{ substr.p -- utility functions -- troy brumley blametroi@gmail.com }
{$ifndef ISUBSTR }
{$define ISUBSTR }

{ return a copy of string s from characters p through l. }

function substr(s : string;
                p : integer;
                l : integer) : string;
var
   i : integer;

begin
   substr := '';
   for i := p to l do
      substr := substr + s[i];
end;

{$endif}
{ end substr.p }
