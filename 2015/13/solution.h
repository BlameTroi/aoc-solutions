/* solution.h -- aoc 2015 13 -- troy brumley */
#ifndef SOLUTION_H
#define SOLUTION_H

/*
 * this is another tsp problem, but we must consider two directions
 * instead of one.
 *
 */


/*
 * necessary constants and definitions:
 */

/* how long may a line in the test dataset be? */

#define INPUT_LEN_MAX 255

/* small dataset is 4, large dataset is 8 */

#define NODES_MAX 10


/*
 * typedefs:
 */


/*
 * data structures:
 */

int node_count;

char *node_names[NODES_MAX];

int node_delta[NODES_MAX][NODES_MAX];


/*
 * functions:
 */

void
reset_state(
   int rel /* free any node_names? */
);

void
parse_line(
   const char *iline /* a line of input */
);

int
node_index(
   const char *s /* name in buffer */
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
