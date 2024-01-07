{ charset2str.p -- utility functions -- Troy Brumley blametroi@gmail.com }

{$ifndef ICHARSET2STR }
{$define ICHARSET2STR }

{$I int2str.p }
{ expects type tCharSet = set of char; }

{ returns a set of characters formated as a displayable string that
  could be used in pascal source but it does not yet collapse ranges. }

function charset2str(cs : tCharSet) : string;

var
  i : integer;

begin
  charset2str := '[ ';
  for i := 0 to 255 do
    if chr(i) in cs then begin
      if length(charset2str) <> 2 then        { first time? }
        charset2str := charset2str + ', ';
      if (i < 32) or (i > 127) then { printable? }
        charset2str := charset2str + 'chr(' + int2str(i) + ')'
      else
        charset2str := charset2str + '''' + chr(i) + '''';
    end;
  charset2str := charset2str + ' ]';
  charset2str := charset2str;
end;

{$endif}
{ end charset2str.p }
