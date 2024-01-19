# shell or sample makefile for fpc
# see https://bytes.usc.edu/cs104/wiki/makefile/ for a quick reference

COMPILER=fpc
OPTIONS=-glw3 -Ciort -ap -Sgix -Sh- -vewnlb
INCLUDES=-Fi../../common/
UNITS=
COMPILE=$(COMPILER) $(OPTIONS) $(INCLUDES) $(UNITS)

all: program

program: solution.pas
	$(COMPILE) solution

run:
	./solution ../../../datasets/2022/15/dataset.txt >output.txt

test:
	./solution ../../../datasets/2022/15/testone.txt >testing.txt

clean:
	rm -f solution *.o output.txt testing.txt aoc-log
