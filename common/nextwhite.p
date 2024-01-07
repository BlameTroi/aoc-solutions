{ nextwhite.p -- utility functions -- Troy Brumley blametroi@gmail.com }
{$ifndef INEXTWHITE }
{$define INEXTWHITE }

{ counts on constchars.p but does not include }

{ find the position of the next whitespace character in a string
  starting at position p. updates p to be the return value. returns
  -1 if no whitespace characters are found. }

function nextwhite(    s : string;
                   var p : integer) : integer;
var
  i : integer;

begin
   nextwhite := -1;
   for i := p to length(s) do
      if s[i] in [SPACE, NEWLINE, CRETURN, TAB] then begin
         nextwhite := i;
         break
      end;
   p := nextwhite;
end;

{$endif}
{ end nextwhite.p }
