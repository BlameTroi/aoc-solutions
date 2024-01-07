{ solution.pas -- no space left on device -- Troy Brumley blametroi@gmail.com }

program solution;
{$longstrings off}
{$booleval on}


{ advent of code 2022 day 7 -- no space left on device. given a log
  of a shell session (cd, ls, the output of same) find space on a
  file system. }


type
   integer = int64;

const
   MAXENTRIES       = 1000;     { observed max }
   FILESYSTEM_SIZE  = 70000000; { total space on device }
   NEEDED_SIZE      = 30000000; { amount of space needed }

type
   tEntry = record
               name   : string;
               isdir  : boolean;
               len    : integer;
               parent : integer;
            end;


{ global variables are fine for this }

var
   direntry   : array[0..MAXENTRIES] of tEntry;
   numentries : integer;
   curdir     : integer;
   newent     : integer;
   totalsize  : integer;
   freesize   : integer;
   targetsize : integer;


{ initialize global variables to empty }

procedure init;

var
   i : integer;

begin
   for i := 1 to MAXENTRIES do
      with direntry[i] do begin
         name := '';
         isdir := false;
         len := 0;
         parent := -1;
      end;
   with direntry[0] do begin
      name := '/';
      isdir := true;
      len := 0;
      parent := -1;
   end;
   numentries := 1;
   totalsize := 0;
   freesize := 0;
   targetsize := 0;
end;


{$I aocinput.p }
{$I str2int.p }
{$I substr.p }
{$I strbegins.p }


{ somewhat stupid length calculation for directory contents }

function lengthunder(d : integer) : integer;

var
   i : integer;

begin
   lengthunder := 0;
   for i := 0 to numentries - 1 do begin
      if direntry[i].parent <> d then
         continue;
      if direntry[i].isdir then
         direntry[i].len := lengthunder(i);
      lengthunder := lengthunder + direntry[i].len;
   end;
end;


{ chase up the parents to build the full path name. }

function fullpath(d : integer) : string;

begin
   if direntry[d].parent = -1 then
      fullpath := direntry[d].name
   else
      fullpath := fullpath(direntry[d].parent) + direntry[d].name + '/'
end;


{ watch the input stream and report the position of the first run of
  non-repeating characters. the run length is START_PACKET_LEN. }

function partone : integer;

var
   i : integer;
   p : integer;
   s : string;
   t : string;

begin
   init;
   aocrewind;
   i := 0;
   curdir := -1;
   while not aoceof do begin
      s := aocread;
      if s[1] = '#' then
         continue;
      if s = '$ cd /' then begin
         curdir := 0;
         continue;
      end;
      if curdir < 0 then begin
         writeln(' *** error *** directory not set ', s);
         continue;
      end;
      if s = '$ cd ..' then begin
         curdir := direntry[curdir].parent;
         if curdir < 0 then
            writeln(' *** error *** invalid directory parent ', s);
         continue;
      end;
      if strbegins(s, '$ cd ') then begin
         t := substr(s, 6, length(s));
         for i := 0 to numentries - 1 do begin
            if (direntry[i].name = t) and direntry[i].isdir and (direntry[i].parent = curdir) then begin
               curdir := i;
               s := 'ok';
               break;
            end;
         end;
         if s <> 'ok' then
            writeln(' *** error *** no matching directory found ', t);
         continue;
      end;
      if s = '$ ls' then begin
         continue;
      end;
      if strbegins(s, 'dir ') then begin
         newent := numentries;
         numentries := numentries + 1;
         direntry[newent].name := substr(s, 5, length(s));
         direntry[newent].isdir := true;
         direntry[newent].parent := curdir;
         continue;
      end;
      if s[1] in ['0'..'9'] then begin
         newent := numentries;
         numentries := numentries + 1;
         direntry[newent].isdir := false;
         direntry[newent].parent := curdir;
         p := 1;
         direntry[newent].len := str2int(s, p);
         direntry[newent].name := substr(s, p+1, length(s));
         continue;
      end;
      writeln(' *** error *** not understood ', s);
   end;

   { recursively determine size of directories }
   direntry[0].len := lengthunder(0);

   { report directory sizes }
   partone := 0;
   totalsize := 0;
   for i := 0 to numentries - 1 do
      if not direntry[i].isdir then
         totalsize := totalsize + direntry[i].len
      else if direntry[i].len < 100000 then
         partone := partone + direntry[i].len;
   freesize := FILESYSTEM_SIZE - totalsize;
   targetsize := NEEDED_SIZE - freesize;
end;


{ part one did all the initial calculations. a total of 30000000
  bytes of space is needed to run the update. find the one
  directory to delete to get enough space. }

function parttwo : integer;

var
   i, m, d : integer;

begin
   m := totalsize;
   d := 0;
   for i := 1 to numentries do begin
      if direntry[i].isdir then
         if direntry[i].len >= targetsize then
            if direntry[i].len < m then begin
               m := direntry[i].len;
               d := i;
            end;
   end;
   parttwo := direntry[d].len;
end;


{ the mainline driver for a day's solution. }
procedure mainline;

var
   i : integer;

begin
   aocopen;
   i := partone;
   writeln('part one answer : ', i);
   i := parttwo;
   writeln('part two answer : ', i);
   aocclose;
end;


{ minimal boot to mainline }

begin
  mainline;
end.
