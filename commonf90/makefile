# shell or sample makefile for gfortran
# see https://bytes.usc.edu/cs104/wiki/makefile/ for a quick reference
# attempting to do the include paths cleanly
ROOTDIR=$(HOME)
PROJDIR=$(ROOTDIR)/projects/aoc/solutions
INCDIR=$(PROJDIR)/commonf90
MODDIR=$(PROJDIR)/commonf90
DATDIR=$(ROOTDIR)/projects/aoc/datasets
INCLUDES=-I$(INCDIR)
MODULES=-J$(MODDIR)
COMPILER=gfortran
OPTIONS=-Wall -fcheck=all
COMPILE=$(COMPILER) -c $(OPTIONS) $(INCLUDES) $(MODULES)
COMPLINK=$(COMPILER) $(OPTIONS) $(INCLUDES) $(MODULES) $(MODDIR)/*.o

default:

testhelpers: testhelpers.f90 aochelpers.*
	$(COMPLINK) testhelpers.f90

aocinput: aocinput.f90
	$(COMPILE) aocinput.f90

charset: charset.f90
	$(COMPILE) charset.f90

intset: intset.f90
	$(COMPILE) intset.f90

aochelpers: aochelpers.f90
	$(COMPILE) aochelpers.f90

clean:
	rm -rf *.mod *.o
