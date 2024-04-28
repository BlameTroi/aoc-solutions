# Makefile -- 2015 04

YEAR=2015
MONTH=04

CC=clang
CFLAGS=-Wall -g3 -std=c99

SRCS = solution.c
OBJS = solution.o

DATADIR=$(HOME)/projects/aoc/datasets/$(YEAR)/$(MONTH)

.PHONY: all clean short long

all: solution

solution.o: solution.c
	$(CC) $(CFLAGS) -c solution.c

solution: solution.o
	$(CC) -o solution solution.o -lm

clean:
	rm -rf *.o solution solution.dSYM/

short:
	./solution abcdef
	./solution pqrstuv

long:
	./solution iwrupvqb
