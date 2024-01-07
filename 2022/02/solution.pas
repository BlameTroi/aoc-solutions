{ solution.pas -- rock paper scissors -- Troy Brumley blametroi@gmail.com }

{ 2022 day 02 -- rock paper scissors }

program solution;
{$longstrings off}
{$booleval on}

type
  integer = int64;


{$I aocinput.p }


type

{ ordinal types start from 0, bogus entries added to provide error
  values. }

   tChoice = (rock,
              paper,
              scissors,
              spock);
   tOutcome = (win,
               lose,
               draw,
               tilt);
   tPlayer = (them,
              me,
              who);

{ a processed line from the input dataset. if the play was empty,
  the outcome and score are invalid and should be ignored. }

   tResult = record
                play    : string;     { the play, 'A Z', etc. }
                outcome : tOutcome;   { the result, win lose draw }
                score   : integer;    { score as calculated }
             end;

{ the input dataset for the days problem, one per line of input.
  the original input line and its parsed value(s). blank lines
  are maintained for control break purposes. }

   pData = ^tData;
   tData = record
              fwd,
              bwd : pData;      { ch-ch-ch-chains }
              num : integer;    { line number in source }
              rec : string;     { original input }
              res : tResult;    { result of the round played }
           end;


{ determine the choice to be made by the player. }

function getP2Choice(play : string) : tChoice;

begin
   getP2Choice := spock;

   if play[1] = 'A' then begin { opponent rock }
      if play[3] = 'X' then { lose }
         getP2Choice := scissors
      else if play[3] = 'Y' then { draw }
         getP2Choice := rock
      else if play[3] = 'Z' then { win }
         getP2Choice := paper;
      exit;
   end;

   if play[1] = 'B' then begin { opponent paper }
      if play[3] = 'X' then { lose }
         getP2Choice := rock
      else if play[3] = 'Y' then { draw }
         getP2Choice := paper
      else if play[3] = 'Z' then { win }
         getP2Choice := scissors;
      exit;
   end;

   if play[1] = 'C' then begin { opponent scissors }
      if play[3] = 'X' then { lose }
         getP2Choice := paper
      else if play[3] = 'Y' then { draw }
         getP2Choice := scissors
      else if play[3] = 'Z' then { win }
         getP2Choice := rock;
      exit;
   end;
end;


{ sometimes a big if else is an ok approach. it breaks down soon
  enough, but it's ok for a start. translate the play two code
  back to the play one outcome. }

function getP1Outcome(play : string) : tOutcome;

begin
   getP1Outcome := tilt;
   if play = 'A X' then
      getP1Outcome := draw
   else if play = 'A Y' then
      getP1Outcome := win
   else if play = 'A Z' then
      getP1Outcome := lose
   else if play = 'B X' then
      getP1Outcome := lose
   else if play = 'B Y' then
      getP1Outcome := draw
   else if play = 'B Z' then
      getP1Outcome := win
   else if play = 'C X' then
      getP1Outcome := win
   else if play = 'C Y' then
      getP1Outcome := lose
   else if play = 'C Z' then
      getP1Outcome := draw
end;


{ how many points is a play worth? }

function getP1Points(play : string) : integer;

begin
   getP1Points := 0;
   if play = 'A X' then
      getP1Points := 4
   else if play = 'A Y' then
      getP1Points := 8
   else if play = 'A Z' then
      getP1Points := 3
   else if play = 'B X' then
      getP1Points := 1
   else if play = 'B Y' then
      getP1Points := 5
   else if play = 'B Z' then
      getP1Points := 9
   else if play = 'C X' then
      getP1Points := 7
   else if play = 'C Y' then
      getP1Points := 2
   else if play = 'C Z' then
      getP1Points := 6
end;


{ given the play directions to lose, draw, or win, convert play to the
  part one fomrat and return that score. }

function getP2Points(play : string) : integer;

begin
   getP2Points := 0;
   case getP2Choice(play) of
     rock     : play[3] := 'X';
     paper    : play[3] := 'Y';
     scissors : play[3] := 'Z'
   end;
   getP2Points := getP1Points(play);
end;


{ part one -- using the assumption that X, Y, and Z are 'my' plays,
  score each play and accumulate the total score as hte answer for
  part one. in part two the instructions are clarified and the data
  reprocessed to get a different answer. }

function partOne(pd : pData) : integer;

var
   i : integer;

begin
   partone := 0;
   i := 0;

   while pd <> nil do
      with pd^.res do begin
         i := i + 1;
         if pd^.rec <> '' then begin
            play := pd^.rec;
            outcome := getP1Outcome(play);
            score := getP1Points(play);
            partone := partone + score;
         end; { if pd^.rec <> '' }
         pd := pd^.fwd
      end; { with pd^ }
end;


{ part two -- having learned that X, Y, and Z actually mean to lose,
  draw, or win, determine your total score using the same scoring as
  part one. }

function parttwo(pd : pData) : integer;

var
   i : integer;

begin
   parttwo := 0;
   i := 0;

   while pd <> nil do
      with pd^.res do begin
         i := i + 1;
         if pd^.rec <> '' then begin
            play := pd^.rec;
            outcome := getP1Outcome(play);
            score := getP2Points(play);
            parttwo := parttwo + score;
         end; { if pd^.rec <> '' }
         pd := pd^.fwd
      end; { with pd^ }
end;


{ mainline }

procedure mainline;

var
   first,
   prior,
   curr   : pData;
   i      : integer;
   s      : string;

begin
   aocopen;

   first := nil;
   prior := nil;
   i := 0;
   while not aoceof do begin
      i := i + 1;
      s := aocread;
      new(curr);
      curr^.fwd := nil;
      curr^.bwd := prior;
      curr^.num := i;
      curr^.rec := s;
      if first = nil then
         first := curr
      else
         prior^.fwd := curr;
      prior := curr
   end; { while not eof }

   i := partOne(first);
   writeln('part one answer : ', i);

   i := partTwo(first);
   writeln('part two answer : ', i);

   aocclose;

end;

{ minimal boot to mainline }
begin
  mainline;
end.
