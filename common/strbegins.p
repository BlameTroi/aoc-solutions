{ strbegins.p -- utility functions -- Troy Brumley blametroi@gmail.com }
{$ifndef ISTRBEGINS }
{$define ISTRBEGINS }

{ does string s begin with string t? }

function strbeginspos(s : string;
                      p : integer;
                      t : string) : boolean;
var
   i : integer;

begin
   strbeginspos := false;
   for i := 1 to length(t) do
      if s[p - 1 + i] <> t[i] then
         exit;
   strbeginspos := true;
end;


function strbegins(s : string;
                   t : string) : boolean;
begin
   strbegins := false;
   if length(s) < length(t) then
      exit;
   if length(s) = length(t) then begin
      strbegins := s = t;
      exit
   end;
   strbegins := strbeginspos(s, 1, t);
end;

{$endif}
{ end strbegins.p }
