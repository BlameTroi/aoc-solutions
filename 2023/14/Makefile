# shell or sample makefile for fpc
# see https://bytes.usc.edu/cs104/wiki/makefile/ for a quick reference

COMPILER=fpc
OPTIONS=-glw3 -Ciort -ap -Sgix -Sh- -vewnlb
INCLUDES=
UNITS=
COMPILE=$(COMPILER) $(OPTIONS) $(INCLUDES) $(UNITS)

all: program

program: fpcerrs.pas
	$(COMPILE) fpcerrs.pas

# Delete the build directory and program
clean:
	rm -rf fpcerrs *.o
