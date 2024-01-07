# Advent of Code

My on again off again work on Advent of Code. I started in 2022 with golang, didn't enjoy working in the language and stopped. I later restarted with Pascal and both 2022 and 2023 will be done in Pascal.

There's nothing special here, just me using AoC as writing prompts. The goal is to have fun and stretch my long dormant coding muscles. I will often take the path of greatest resistance. Overcoding, odd datatype choices, and repeated violation of 'dry' and 'yagni' will be common.

This is about practice and habit. I'll certainly learn other things along the way, but the goal is to keep the fingers moving writing code.


## Project Structure

root
  year
    day
    day
  common

Each year will likely be monolingual and with languages off the main sequence: Pascal, assembly, Fortran, Scheme, Cobol, and Basic are things I like to putter around in.

Directories for each day and a `common` directory to hold reuse bits. If a library of possible helper procedures becomes obvious, I may create a Pascal unit to hold them, but for now it's either copy-and-paste or {$I includes}.

Within each directory the following files will exist once the day is unlocked and solved:

problem.txt   -- the problem statement from the AoC site.
notes.txt     -- any notes I make.
solution.pas  -- the solution, it reads dataset.txt and produces ->
output.txt    -- the answer to the problems.
makefile      -- a very simple makefile to drive the compiler and test runs.


## Test Data

The site asks that we not post the full datasets in public view. I have them in a private repository. Smaller test data (extracted from the problem statement) can be found in this project or the dataset repository. The full data is always named `dataset.txt` and the tests are `testxxx.txt`.


## Notes on Style

I'm using 1980ish tooling with a very vanilla Emacs, text files, and old school structured programming. Objects? We don't need no stinking objects.

### Pascal

The compiler is Free Pascal 3.22 in with runtime checks enabled, a limited number of extensions, and usually ShortStrings. Integers are cast to 64 bits--I prefer a dash of retro with my code, but I'm not a purist.

* integers are longints (4 bytes).
* strings are classic string[255]. no trailing nulls are guaranteed. FPC AnsiStrings can be turned on explicitly, as can PChar.
* dynamic arrays are available but will be avoided.
* exceptions are available but will also be avoided. If you need them for AoC you are doing it wrong.
* `break` and `continue` are available in loops, `exit` is available for early exit from procedures and functions, but I steadfastly refuse to use `result`.
* boolean evaluation short circuiting is disabled as it's non-standard in a way that is visible in the code.


## Licensing and copyright

All my code is released with the unlicense. To whatever extent a copyright is needed, this is copyright 2023 by Troy Brumley.

Troy Brumley  
blametroi@gmail.com  
  So let it be written. So let it be done.   
