{ truncright.p -- utility functions -- Troy Brumley blametroi@gmail.com }

{$ifndef ITRUNCRIGHT }
{$define ITRUNCRIGHT }

{$I leftstr.p }

{ truncate a string on its right, optionally replacing the last
  three characters with ellipsis to indicate that the string was
  truncated. }

function truncright(s : string;
                    l : integer;
                    e : boolean) : string;

var
   i : integer;
   t : string;

begin
   truncright := s;
   if length(s) <= l then
      exit;
   t := leftstr(s, l);
   if e and (l > 3) then
      for i := length(t) downto length(t) - 2 do
         t[i] := '.';
   truncright := t
end;

{$endif}
{ end truncright.p }
