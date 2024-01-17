{ aocinput.p -- input procedures and structures -- troy brumley blametroi@gmail.com }

{$ifndef IAOCINPUT }
{$define IAOCINPUT }

{ all advent of code dataset reads should be done via these
  procedures. the name of the input file is expected to be
  the first parameter on the command line. }

{ global control block for input dataset. }
type
   tAocInput = record
                  open    : boolean;
                  dsn     : string;
                  count   : integer;
                  rec     : string;
                  ateof   : boolean;
                  ateol   : boolean;
                  ch      : char;
                  comment : char;
                  logging : boolean;
                  logopen : boolean;
                  logfile : text;
               end;

var
   aocinput : tAocInput;

{$I strbegins.p }

procedure aochalt(msg : string); forward;

{ set the comment character }

procedure aoccomment(c : char);

begin
   aocinput.comment := c;
end;


procedure aocrewind;

begin
   reset(input);
end;


{ initialize the input tracking control block and
  open the DATASET_NAME on stdin. }

procedure aocopen;

begin
   { aoc dataset must be first parameter }
   if paramcount < 1 then
      aochalt('*** expected dataset name not found ***');
   {$I-}
   assign(input, paramstr(1));
   reset(input);
   {$I+}
   if ioresult <> 0 then
      aochalt('*** error opening ' + paramstr(1) + ' ***');

   aocinput.count := 0;
   aocinput.rec := '';
   aocinput.dsn := paramstr(1);
   aocinput.open := true;
   aocinput.ateof := false;
   aocinput.ateol := false;
   aocinput.ch := chr(0);
   aocinput.comment := chr(0);
   aocinput.logging := true;
   aocinput.logopen := false;
end;

{ wrap eof check. }

function aoceof : boolean;

begin
   aoceof := eof(input);
end;


function aoceol : boolean;

begin
   aoceol := eoln(input);
end;


{ close input }

procedure aocclose;

begin
   if aocinput.open then begin
      aocinput.open := false;
      close(input);
   end;
   if aocinput.logopen then begin
      aocinput.logopen := false;
      close(aocinput.logfile);
   end;
end;


{ flush the current input record. }

procedure aocflushl;

begin
   readln(input);
   aocinput.count := aocinput.count + 1;
end;


procedure aocnolog;
begin
   aocinput.logging := false;
end;


{ logging file for stuff that won't be preserved in normal output
  or the repository. }

procedure aoclog(s : string);

begin
   if not aocinput.open then begin
      writeln(stderr, 'log request out of sequence, ignored');
      exit;
   end;

   if aocinput.logging then begin
      if not aocinput.logopen then begin
         assign(aocinput.logfile, 'aoc-log');
         rewrite(aocinput.logfile);
         aocinput.logopen := true;
      end;
      writeln(aocinput.logfile, s);
   end;
end;


{ flush output buffer, quietly ignore if we aren't logging }

procedure aoclogflush;
begin
   if aocinput.logging then
      flush(aocinput.logfile);
end;

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


{ input reader for advent of code. use this to get individual
  characters from a line when a string is not the best answer. }

function aocreadch : char;

begin
   if not aocinput.open then
      aochalt('error: aocreadch called out of sequence');

   if aoceof then begin
      aocinput.ateof := true;
      aocreadch := chr(0);
      exit;
   end;
   if aoceol then begin
      aocinput.ateol := true;
      aocreadch := chr(0);
      aocflushl;
      exit;
   end;
   read(input, aocreadch);
   aocinput.ch := aocreadch;
end;


{ input reader for advent of code. allows tracking of record number
  for error reporting. records beginning with a hash are comments
  and not passed back. }

function aocread : string;

   function notcomment(s : string) : boolean;
   begin
      notcomment := true;
      if (aocinput.comment = chr(0)) or (length(s) = 0) then
         notcomment := true
      else
         notcomment := s[1] <> aocinput.comment;
   end;

begin
   if not aocinput.open then
      aochalt('error: aocread called out of sequence');
   repeat
      if aoceof then
         aocinput.rec := ''
      else begin
         readln(input, aocinput.rec);
         aocinput.count := aocinput.count + 1;
      end;
   until aoceof or notcomment(aocinput.rec);
   aocread := copy(aocinput.rec, 1);
end;


{ for structured input, factor checking to here. }

function aocreadexpecting(p :  string) : string;
var
   s : string;

begin
   s := aocread;
   if not strbegins(s, p) then
      aochalt('*** expected input prefix not found: "' + p + '" ***');
   aocreadexpecting := s;
end;


{$endif}
{ end aocinput.p }
