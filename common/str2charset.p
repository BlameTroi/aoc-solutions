{ str2charset.p -- utility functions -- Troy Brumley blametroi@gmail.com }
{$ifndef ISTR2CHARSET }
{$define ISTR2ChARSET }

{ expects type tCharSet = set of char; }

{ given a string, return a set of the characters that
  make up the string. }

function str2charset(s : string) : tCharSet;

var
  i : integer;

begin
   str2charset := [];
   for i := 1 to length(s) do
      include(str2charset, s[i]);
end;

{$endif}
{ end str2charset.p }
