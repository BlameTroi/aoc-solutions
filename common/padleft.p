{ padleft.p -- utility functions -- Troy Brumley blametroi@gmail.com }
{$ifndef IPADLEFT }
{DEFINE IPADLEFT }

{ pad a string with blanks on the left to the specified length. }

function padleft(s : string;
                 i : integer) : string;

begin
   padleft := s;
   while length(padleft) < i do
      padleft := ' ' + padleft;
end;

{$endif}
{ end strfuncs.p }
