{ padright.p -- utility functions -- Troy Brumley blametroi@gmail.com }
{$ifndef IPADRIGHT }
{$define IPADRIGHT }

{ pad a string with blanks on the right to the specified length. }

function padright(s : string;
                  i : integer) : string;

begin
   padright := s;
   while length(padright) < i do
      padright := padright + ' ';
end;

{$endif}
{ end padright.p }
