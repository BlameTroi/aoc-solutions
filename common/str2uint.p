{ str2int.p -- utility functions -- Troy Brumley blametroi@gmail.com }
{$ifndef ISTR2INT }
{$define ISTR2INT }


{ convert string to integer. allows for optional leading sign.
  conversion ends at end of string or first invalid character.
  updates parameter p. }

function str2int(    s : string;
                 var p : int64) : int64;
var
   i : int64;
   c : char;
   n : boolean;

begin
   n := false;
   i := 0;
   str2int := 0;
   for i := p to length(s) do begin
      c := s[i];
      if i = p then
         if c in ['-', '+'] then begin
            n := c = '-';
            continue
         end;
      if c in ['0'..'9'] then
         str2int := str2int * 10 + ord(c) - ord('0')
      else begin
         p := i;
         break
      end
   end; { for }
   p := i;
   if n then
      str2int := 0 - str2int;
end;


{ convert string to unsigned integer. conversion ends at end of string
  or first invalid character. updates parameter p. quietly ignores a
  leading sign. }

function str2uint(    s : string;
                  var p : uint64) : uint64;
var
   i : uint64;
   c : char;

begin
   i := 0;
   str2uint := 0;
   for i := p to length(s) do begin
      c := s[i];
      if i = p then
         if c in ['-', '+'] then
            continue;
      if c in ['0'..'9'] then
         str2uint := str2uint * 10 + ord(c) - ord('0')
      else begin
         p := i;
         break
      end
   end; { for }
   p := i;
end;

{$endif}
{ end str2int.p }
