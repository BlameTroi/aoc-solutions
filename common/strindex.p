{ strindex.p -- position in string -- troy brumley blametroi@gmail.com }
{$ifndef ISTRINDEX }
{$define ISTRINDEX }

{ find position of c in s. returns -1 if not found. }

function strindexpos(    s : string;
                     var p : integer;
                         c : char) : integer;
var
   i : integer;

begin
   strindexpos := -1;
   i := p;
   while i < length(s) do begin
      i := i + 1;
      if s[i] = c then begin
         strindexpos := i;
         p := i;
         break;
      end;
   end;
end;

function strindex(s : string; c : char) : integer;
var
   p : integer;

begin
   p := 1;
   strindex := strindexpos(s, p, c);
end;

{$endif}
{ end strindex.p }
