# Advent of Code

My on again off again work on Advent of Code. I started in 2022 with golang, didn't enjoy working in the language and stopped. I later restarted with Pascal and both 2022 and 2023 will be done in Pascal.

There's nothing special here, just me using AoC as writing prompts. The goal is to have fun and stretch my long dormant coding muscles. I will often take the path of greatest resistance. Overcoding, odd datatype choices, and repeated violation of 'dry' and 'yagni' will be common.

This is about practice and habit. I'll certainly learn other things along the way, but the goal is to keep the fingers moving writing code.


## Progress

Year    Stars
2015    50
2016
2017
2018
2019
2020
2021
2022    30
2023    22
2024

## TODO

Some text files are in org format, some in markdown, but I'll be slowly converting verything to plain text. Neither org nor markdown really get it done for me.


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

I started out in Pascal, switched to Fortran, and then to C. I redid the Pascal code in Fortran but then switched to C and plan to stick with plain old C99 from here on out. Free Pascal is too bound up in Lazarus, Delphi isn't cross platform. While modern Fortran rocks, I'm an assembler programmer at heart, so working down at C level just feels right.


### Pascal

The compiler is Free Pascal 3.22 in with runtime checks enabled, a limited number of extensions, and usually ShortStrings. Integers are cast to 64 bits--I prefer a dash of retro with my code, but I'm not a purist.

* integers are longints (4 bytes).
* strings are classic string[255]. no trailing nulls are guaranteed. FPC AnsiStrings can be turned on explicitly, as can PChar.
* dynamic arrays are available but will be avoided.
* exceptions are available but will also be avoided. If you need them for AoC you are doing it wrong.
* `break` and `continue` are available in loops, `exit` is available for early exit from procedures and functions, but I steadfastly refuse to use `result`.
* boolean evaluation short circuiting is disabled as it's non-standard in a way that is visible in the code.


### Fortran

About half way through the 2022 and 2023 I decided to switch to Fortran. I've been meaning give Fortran a try for a while and after seeing enough of the present state of the compilers and standards I feel they are superior to Pascal for my needs. I still love Pascal, but the direction of Delphi and Lazarus is heavy objects and forms based. I'm more comfortable at lower levels.

The general approach is the same as with Pascal, but given the strong standards support I don't have to add cruft to get the language in what I consider proper shape.

There are two compilers I am alternating between: gfortran 13.2 and lfortran. lfortran is still in a pre-beta state but as gfortran and lldb on MacOs don't work without jumping through hoops, I expect to use lfortran if I have to use a debugger.


### C

I started on 2015 using `C`. At this time clang 15 is the version Apple provides. All my code is compiled with `-Wall -O0 -g3 -fsanitize=address --std=c99`.


### Editor/IDE

Emacs, eglot, clangd, fortls, treesitter, fprettify, and astyle for the win.


## Dependencies

I strive for no external (to me) dependencies. The Fortran and Pascal based solutions carry common code and shells in directories parallel with each day's code.

For `C` I'm using `minunit` (a header only unit testing harness) from https://github.com/siu/minunit. I also have started putting my own `C` common code in header only libraries that are at https://github.com/BlameTroi/txblibs. I don't add those as formal dependencies in git but the headers are in my local include directory.

## Licensing and copyright

All my code is released with the unlicense. To whatever extent a copyright is needed, this is copyright 2024 by Troy Brumley.

Troy Brumley  
blametroi@gmail.com  
  So let it be written. So let it be done.   
