/* solution.h -- aoc 2015 13 -- troy brumley */
#ifndef SOLUTION_H
#define SOLUTION_H

/*
 * split a volume among a set of containers, how many combinations
 * of containers can hold exactly that volume. obviously, using
 * all containers is not required, and likely isn't possible.
 *
 */


/*
 * necessary constants and definitions:
 */

/* how long may a line in the test dataset be? */

#define INPUT_LEN_MAX 255

/* an arbitrary upper limit. the test data has 20 */

#define CONTAINER_MAX 25


/*
 * typedefs:
 */


/*
 * data structures:
 */

int num_containers;                 /* how many do we have */
int containers[CONTAINER_MAX];      /* each container's volume. repeats possible */
int target_volume;                  /* first value of input is the volume to split */

/*
 * functions:
 */

/* the usual */

void
reset_state(
   int rel
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
partOne(
   const char *                       /* a file name */
);


int
partTwo(
   const char *                       /* a file name */
);


/* end solution.h */
#endif /* SOLUTION_H */
