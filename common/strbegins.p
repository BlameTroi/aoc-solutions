{ strbegins.p -- utility functions -- Troy Brumley blametroi@gmail.com }
{$ifndef ISTRBEGINS }
{$define ISTRBEGINS }

{ does string s begin with string t? }

function strbegins(s : string;
                   t : string) : boolean;
var
   i : integer;

begin
   strbegins := false;
   if length(s) < length(t) then
      exit;
   if length(s) = length(t) then begin
      strbegins := s = t;
      exit
   end;
   for i := 1 to length(t) do
      if s[i] <> t[i] then
         exit;
   strbegins := true;
end;

{$endif}
{ end strbegins.p }
