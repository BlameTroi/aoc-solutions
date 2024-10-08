/* solution.h -- aoc 2015 13 -- troy brumley */
#ifndef SOLUTION_H
#define SOLUTION_H

#include <stdbool.h>

/*
 * describe the day's problem
 *
 */


/*
 * necessary constants and definitions:
 */

/* how long may a line in the test dataset be? */

#define INPUT_LEN_MAX 255

/* it matters */

#define NIL_VALUE -1

/* attributes of one of our aunts */

#define DATA_MAX 10
#define CHIL_IX   0
#define CATS_IX   1
#define SAMO_IX   2
#define POME_IX   3
#define AKIT_IX   4
#define VISZ_IX   5
#define FISH_IX   6
#define TREE_IX   7
#define CARS_IX   8
#define PERF_IX   9

#define AUNT_MAX 500

/*
 * typedefs:
 */

/* map data keywords to indices in the facts_t data array. also
   carries an operation indicator to refine searching beyond
   equality. */

typedef struct map_t {
	char tag[16];
	int  ix;
	char op;
} map_t;

/* facts about this aunt. */

typedef struct facts_t {
	int id;
	int data[DATA_MAX];
} facts_t;

/*
 * data structures:
 */


int num_aunts;
facts_t aunts[AUNT_MAX];


/*
 * functions:
 */

/* our usuals */

void
reset_state(
        int rel
);


void
parse_line(
        const char *iline
);


/* an additional input is a set of conditions that we need to match
   against. it is a subset of the dataset line format and can be
   parsed into the facts structure for convenience. */

facts_t
parse_condition(
        const char *iline
);


/* get the index of a particular data item map entry by name */

int
get_data_ix(
        const char *s
);


/* how many aunts match the condition? */

int
satisfy_count(
        facts_t cond
);


/* does the aunt at this index satisfy the condition? */

bool
satisfies(
        facts_t cond,
        int ix
);


/* return the index of the first aunt that satisfies the condition
   starting from a particular index. */

int
satisfy_ix(
        facts_t cond,
        int start
);


/*
 * the code for each of the day's parts.
 *
 * each expects the name of an input dataset or some other string to
 * drive the part's code.
 *
 * should print the result to stdout.
 *
 * returns EXIT_SUCCESS or EXIT_FAILURE.
 */


int
part_one(
        const char *                       /* a file name */
);


int
part_two(
        const char *                       /* a file name */
);


/* end solution.h */
#endif /* SOLUTION_H */
