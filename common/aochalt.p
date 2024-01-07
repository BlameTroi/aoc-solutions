{ aochalt.p -- report error and halt -- troy brumley blametroi@gmail.com }

{$ifndef IAOCHALT }
{$define IAOCHALT }
{ needs aoinput for reporting last record read. }
{$I aocinput.p }

{ error report and halt }
procedure aochalt(msg : string);
var
   r : string;
begin
   writeln(stderr, msg);
   if aocinput.open then begin
      writeln(stderr, 'last reacord read from ', aocinput.dsn, ' was: ', aocinput.count);
      writeln(stderr, 'contents: ''', aocinput.rec, '''');
   end;
   if not eof(input) then begin
      readln(input, r);
      writeln(stderr, 'next input record: ', r);
   end;
   aocclose;
   halt(-1);
end;

procedure aocerr(msg : string);
begin
   writeln(stderr, msg);
end;

{$endif}
{ end aochalt.p }
