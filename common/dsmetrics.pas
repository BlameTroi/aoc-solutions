{ dsmetrics.pas -- aoc dataset report -- Troy Brumley BlameTroi@gmail.com }

program dsmetrics;

{ expects to be compiled with the following two mode flags and command
line/fpc.cfg options of -Ciort -Sgix -Sh- for full checks, allowing
exceptions, goto, inlining, and prefer classic strings. }
{$LONGSTRINGS OFF}            { to be sure, use AnsiString or pchar explicitly }
{$BOOLEVAL ON}                { standard pascal expects this }

type
   integer = int64;            { by default ints are 64 bits this decade}

{$I constchars.p }

{ scan an aoc dataset and report on its shape and composition. }

{$I min.p }
{$I max.p }
{$I aocinput.p }

var
   words, lines, characters,
   linelen, maxline, minline,
   wordlen, maxword, minword,
   wordsline, maxwordsline, minwordsline,
   freqalfa, freqnum, freqspec, freqwhit,
   i        : integer;
   lastc, c : char;
   freqmap  : array[0..255] of integer;

begin
   aocopen;
   reset(input);
   writeln('analyzing aoc dataset');

   for i := 0 to 255 do
      freqmap[i] := 0;
   freqalfa := 0; freqnum := 0; freqspec := 0; freqwhit := 0;

   words := 0; wordlen := 0; maxword := 0; minword := maxint;
   lines := 0; linelen := 0; maxline := 0; minline := maxint;
   wordsline := 0; maxwordsline := 0; minwordsline := maxint;
   characters := 0;

   lastc := NULL;
   while not aoceof do begin
      c := aocreadch;
      if c = chr(0) then begin
         { chr(0) is returned on newline. it does not count
         as a character for these metrics. }
         minline := min(linelen, minline);
         maxline := max(linelen, maxline);
         minwordsline := min(wordsline, minwordsline);
         maxwordsline := max(wordsline, maxwordsline);
         wordsline := 0;
         linelen := 0;
         lines := lines + 1;
         characters := characters + 1;
         lastc := NEWLINE;
         freqmap[ord(lastc)] := freqmap[ord(lastc)] + 1;
         continue;
      end;
      characters := characters + 1;
      linelen := linelen + 1;
      if c > space then
         wordlen := wordlen + 1;
      if (c > space) and (lastc <= space) then begin
         minword := min(wordlen, minword);
         maxword := max(wordlen, maxword);
         words := words + 1;
         wordsline := wordsline + 1;
         wordlen := 0;
      end;
      lastc := c;
      freqmap[ord(lastc)] := freqmap[ord(lastc)] + 1;
   end;

   writeln('counts...');
   writeln('           ', '  count', '   minl', '   maxl');
   writeln('   lines : ', lines : 7,  minline : 7,  maxline : 7);
   writeln('   words : ', words : 7,  minword : 7,  maxword : 7);
   writeln('per line : ', '       ', minwordsline : 7, maxwordsline : 7);
   writeln('   chars : ', characters : 7);
   writeln;
   writeln('character distribution...');
   for i := low(freqmap) to high(freqmap) do begin
      if chr(i) in ['a'..'z', 'A'..'Z'] then
         freqalfa := freqalfa + freqmap[i];
      if chr(i) in ['0'..'9'] then
         freqnum := freqnum + freqmap[i];
      if chr(i) in ['!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '-', '+', '_', '=',
                    '[', ']', '{', '}', '\', '|', '~', '`', '.', ',', '<', '>', '/', '?'] then
         freqspec := freqspec + freqmap[i];
      if chr(i) in [SPACE, NEWLINE, TAB, CRETURN] then
         freqwhit := freqwhit + freqmap[i];
   end;
   writeln(' alphabetic :', freqalfa : 7);
   writeln('     digits :', freqnum : 7);
   writeln('    special :', freqspec : 7);
   writeln('white space :', freqwhit : 7);
   writeln;
   aocclose;
end.

{ end dsmetrics.pas }
