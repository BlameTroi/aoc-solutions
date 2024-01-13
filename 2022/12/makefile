# fpc makefile for advent of code
# -O3 will sometimes throw "variable does not appear to be
# initialized" even when the code is OK. I haven't determined the
# pattern.

COMPILER=fpc
OPTIONS=-glw3 -Ciort -ap -Sgix -Sh- -vewnlb
ROPTIONS=-g- -ap -Sgix -Sh- -O3 -vewnlb
INCLUDES=-Fi../../common/
UNITS=
COMPILE=$(COMPILER) $(OPTIONS) $(INCLUDES) $(UNITS)
RCOMPILE=$(COMPILER) $(ROPTIONS) $(INCLUDES) $(UNITS)

all: program

program: solution.pas
	$(COMPILE) solution

release:
	$(RCOMPILE) solution

run:
	./solution ../../../datasets/2022/12/dataset.txt >output.txt

test:
	./solution ../../../datasets/2022/12/testone.txt >testing.txt

# Delete the build directory and program
clean:
	rm -rf solution aoc-log *.o
