# Makefile -- advent of code, ansi c work

# i've settled on minunit https://github.com/siu/minunit as a unit testing
# framework. it's a header-only distribution, which i like. breaking up my
# current shells to support this is the work for 2015 day 6. each day's
# solution will have:
#
# driver.c   -- just a main function that calls part_one and part_two in
# solution.h -- types and any function definitions needed for
# unittest.c -- any unit tests for code in
# solution.c -- where the real work happens


# where to find test data:
YEAR=2015
MONTH=16
DATADIR=$(HOME)/projects/aoc/datasets/$(YEAR)/$(MONTH)


# compiler and linker:
CC=clang
# CFLAGS=-Wall -g3 -std=c99
CFLAGS=-fsanitize=address -Wall -g3 -O0 -std=c99
# LDFLAGS=-lm
LDFLAGS=


# i don't really use these but for the sake of documentation:
SRCS=driver.c solution.c unittest.c
HDRS=solution.h
OBJS=driver.o solution.o unittest.o


# the targets you should ask for
.PHONY: clean unit test all short long


# scrub ... dSYM isn't produced by my current build script, but
# it can if you do a single file compile and link so i include
# them as a reminder.
clean:
	rm -rf *.o *.dSYM/ unittest driver


# the work to solve the problem
all: driver


# test drive
unit: unittest


# the code that does the stuff
solution.o: solution.c solution.h
	$(CC) $(CFLAGS) -c solution.c


# test harness
unittest.o: unittest.c solution.h solution.c
	$(CC) $(CFLAGS) -c unittest.c


unittest: unittest.o solution.o
	$(CC) $(CFLAGS) -o unittest unittest.o solution.o $(LDFLAGS)


# run unit tests
test:
	./unittest


# driver for actual runs
driver.o: driver.c
	$(CC) $(CFLAGS) -c driver.c


driver: driver.o solution.o
	$(CC) $(CFLAGS) -o driver driver.o solution.o $(LDFLAGS)


# run against test data
short:
	./driver $(DATADIR)/testone.txt


long:
	./driver $(DATADIR)/dataset.txt
