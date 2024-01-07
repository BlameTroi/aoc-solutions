{ strindex.p -- position in string -- troy brumley blametroi@gmail.com }
{$ifndef ISTRINDEX }
{$define ISTRINDEX }

{ find position of c in s. returns -1 if not found. }

function strindex(s : string;
                  c : char) : integer;
var
   i : integer;

begin
   strindex := -1;
   i := 0;
   while i < length(s) do begin
      i := i + 1;
      if s[i] = c then begin
         strindex := i;
         break;
      end;
   end;
end;

{$endif}
{ end strindex.p }
