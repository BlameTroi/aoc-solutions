/* solution.h -- aoc 2015 06 -- troy brumley */
#ifndef SOLUTION_H
#define SOLUTION_H

/*
 * describe the day's problem
 *
 */


/*
 * necessary constants and definitions:
 */

/* how long may a line in the test dataset be? */

#define INPUT_LEN_MAX 255


/*
 * data structures:
 */


/* use typedefs */
typedef struct bogus_t {
   int x;
   int y;
} bogus_t;



/*
 * functions:
 */



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
      char *                       /* a file name */
      );


int
partTwo(
      char *                       /* a file name */
      );


/* end solution.h */
#endif /* SOLUTION_H */
