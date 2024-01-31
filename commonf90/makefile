# shell or sample makefile for gfortran
# see https://bytes.usc.edu/cs104/wiki/makefile/ for a quick reference
# attempting to do the include paths cleanly
ROOTDIR=$(HOME)
PROJDIR=$(ROOTDIR)/projects/aoc/solutions
COMPILER=gfortran
OPTIONS=-fdefault-integer-8 -Wall -fcheck=all
INCLUDES=-I$(PROJDIR)/commonf90
MODULES=-J$(PROJDIR)/commonf90
COMPILE=$(COMPILER) -c $(OPTIONS) $(INCLUDES) $(MODULES)

default:

aocinput: aocinput.f90
	$(COMPILE) aocinput.f90

charset: charset.f90
	$(COMPILE) charset.f90

clean:
	rm -rf *.mod *.o
