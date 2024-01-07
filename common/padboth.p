{ padboth.p -- utility functions -- Troy Brumley blametroi@gmail.com }
{$ifndef IPADBOTH }
{$define IPADBOTH }

{ pad a string with blanks on both sides, favoring the right side. }

function padboth(s : string;
                 i : integer) : string;

begin
   padboth := s;
   while length(padboth) < i do begin
      padboth := padboth + ' ';
      if length(padboth) < i then
         padboth := ' ' + padboth
   end { while }
end;

{$endif}
{ end padboth.p }
