# Makefile -- advent of code, ansi c work

YEAR=2016
MONTH=14
DATADIR=$(HOME)/projects/aoc/datasets/$(YEAR)/$(MONTH)

CC=clang
# CFLAGS=-Wall -O3 -std=c99
CFLAGS=-fsanitize=address -Wall -g3 -O0 -std=c99
# LDFLAGS=-lm
LDFLAGS=

SRCS=driver.c solution.c unittest.c
HDRS=solution.h
OBJS=driver.o solution.o unittest.o

SRCS = driver.c solution.c unittest.c
HDRS = solution.h
OBJS = driver.o solution.o unittest.o

.PHONY: clean unit test all short long check-syntax reformat check-includes

check-syntax:
	$(CC) $(CFLAGS) -Wextra -Wpedantic -fsyntax-only $(SRCS)

reformat:
	astyle $(SRCS) $(HDRS)

check-includes:
	include-what-you-use driver.c
	include-what-you-use solution.c
	include-what-you-use unittest.c

clean:
	rm -rf *.o *.dSYM/ unittest driver

all: driver

unit: unittest

solution.o: solution.c solution.h
	$(CC) $(CFLAGS) -c solution.c

unittest.o: unittest.c solution.h solution.c
	$(CC) $(CFLAGS) -c unittest.c

unittest: unittest.o solution.o
	$(CC) $(CFLAGS) -o unittest unittest.o solution.o -lm

test:
	./unittest

driver.o: driver.c
	$(CC) $(CFLAGS) -c driver.c

driver: driver.o solution.o
	$(CC) $(CFLAGS) -o driver driver.o solution.o -lm

short:
	./driver $(DATADIR)/testone.txt

long:
	./driver $(DATADIR)/dataset.txt
