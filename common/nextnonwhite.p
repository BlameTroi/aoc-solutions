{ nextnonwhite.p -- utility functions -- Troy Brumley blametroi@gmail.com }
{$ifndef INEXTNONWHITE }
{$define INEXTNONWHITE }
{ counts on constchars.p but does not include}

{ find the position of the next non whitespace character in a string
  starting at position p. updates p to the same value as the function
  return. returns -1 if no non whitespace characters are found. }

function nextnonwhite(    s : string;
                      var p : integer) : integer;
var
  i : integer;

begin
   nextnonwhite := -1;
   for i := p to length(s) do
      if not(s[i] in [SPACE, NEWLINE, CRETURN, TAB]) then begin
         nextnonwhite := i;
         break
      end;
   p := nextnonwhite;
end;

{$endif}
{ end nextnonwhite.p }
