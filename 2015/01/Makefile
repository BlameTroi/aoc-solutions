# Makefile -- 2015 01

YEAR=2015
MONTH=01

CC=clang
CFLAGS=-Wall -g3 -std=c18 --pedantic-errors

SRCS = solution.c
OBJS = solution.o

DATADIR=$(HOME)/projects/aoc/datasets/$(YEAR)/$(MONTH)

.PHONY: check-syntax all clean short long

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

check-syntax:
	$(CC) $(CFLAGS) -Wextra -Wpedantic -fsyntax-only $(SRCS)
