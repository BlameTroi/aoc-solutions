{ strskip.p -- skip past run of a specific character -- troy brumley
  blametroi@gmail.com }

{$ifndef ISTRSKIP }
{$define ISTRSKIP }

{ advance through a string until a character that isn't c is
  found. returns -1 on error. }

function strskip(s : string;
                 p : integer;
                 c : char) : integer;

begin
   for strskip := p to length(s) do
      if s[strskip] <> c then
         exit;
   strskip := -1;
end;

{$endif}
{ end strskip.p }
