# Makefile -- 2015 02

YEAR=2015
MONTH=02

CC=clang
CFLAGS=-Wall -g3 -std=c99

SRCS = solution.c unittest.c
OBJS = solution.o unittest.o

DATADIR=$(HOME)/projects/aoc/datasets/$(YEAR)/$(MONTH)

.PHONY: all clean short long unit test

all: solution

unit: unittest

solution.o: solution.c
	$(CC) $(CFLAGS) -c solution.c

solution: solution.o
	$(CC) -o solution solution.o -lm

clean:
	rm -rf *.o solution solution.dSYM/ unittest unittest.dSYM/

short:
	./solution $(DATADIR)/testone.txt

long:
	./solution $(DATADIR)/dataset.txt

unittest.o: unittest.c
	$(CC) $(CFLAGS) -c unittest.c

unittest: unittest.o
	$(CC) -o unittest unittest.o -lm

test:
	./unittest
