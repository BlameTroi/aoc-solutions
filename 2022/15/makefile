# shell or sample makefile for gfortran
# see https://bytes.usc.edu/cs104/wiki/makefile/ for a quick reference
# attempting to do the include paths cleanly
ROOTDIR=$(HOME)/projects/aoc
PROJDIR=$(ROOTDIR)/solutions
COMPILER=gfortran
OPTIONS=-Wall -fcheck=all
INCDIR=$(PROJDIR)/commonf90
MODDIR=$(PROJDIR)/commonf90
DATDIR=$(ROOTDIR)/datasets
INCLUDES=-I$(INCDIR)
MODULES=-J$(MODDIR)
COMPILE=$(COMPILER) -c $(OPTIONS) $(INCLUDES) $(MODULES)
COMPLINK=$(COMPILER) $(OPTIONS) $(INCLUDES) $(MODULES) $(MODDIR)/*.o

default:

build: solution.f90
	$(COMPLINK) solution.f90 -o solution

test:
	./solution $(DATDIR)/2022/15/testone.txt

run:
	./solution $(DATDIR)/2022/15/dataset.txt

clean:
	rm -rf *.mod *.o
