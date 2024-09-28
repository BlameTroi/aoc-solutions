# Makefile -- 2015 03

YEAR=2015
MONTH=03

CC=clang
CFLAGS=-Wall -g3 -std=c18 --pedantic-errors

SRCS = solution.c
OBJS = solution.o

DATADIR=$(HOME)/projects/aoc/datasets/$(YEAR)/$(MONTH)

.PHONY: clean unit test all short long check-syntax

check-syntax:
	$(CC) $(CFLAGS) -Wextra -Wpedantic -fsyntax-only $(SRCS)

all: solution

solution.o: solution.c
	$(CC) $(CFLAGS) -c solution.c

solution: solution.o
	$(CC) -o solution solution.o -lm

clean:
	rm -rf *.o solution solution.dSYM/

short:
	./solution $(DATADIR)/testone.txt

long:
	./solution $(DATADIR)/dataset.txt
