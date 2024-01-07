{ str2gridset.p -- utility functions -- Troy Brumley blametroi@gmail.com }
{$ifndef ISTR2GRIDSET }
{$define ISTR2GRIDSET }

{ expects type tGridset, a set of integers. constants tgridnum min
  and max are one less and one more than the actual legal values of
  beg and end. this is a convenience for looping and breaks. }

{ convert an integer range 'low-high' to a set. stops at comma or
  end of string. this input format is based on a problem dataset
  for advent of code. }

function str2gridset(    s : string;
                     var p : integer) : tGridSet;
var
   l,               { low end of range }
   h,               { high end of range }
   i,               { work }
   n  : integer;
   d  : boolean;    { delimiter seen }

begin
   l := 0;
   h := 0;
   str2gridset := [];
   d := false;
   for i := p to length(s) do
      case s[i] of
        ',' : break;       { end of this range }
        '-' : d := true;   { switch to high end }
        '0' .. '9' :       { add digit to low or high }
        if d then
           h := h * 10 + ord(s[i]) - ord('0')
        else
           l := l * 10 + ord(s[i]) - ord('0');
      else break
      end; { case }
   p := i;             { update source pointer }
   for n := l to h do
      include(str2gridset, n);
end;

{$endif}
{ end str2gridset.p }
