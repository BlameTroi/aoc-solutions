/* solution.h -- aoc 2015 06 -- troy brumley */
#ifndef SOLUTION_H
#define SOLUTION_H

/*
 * describe the day's problem
 *
 */


/*
 * how long can one input line be?
 */

#define INPUT_LEN_MAX 255


/*
 * data structures:
 */

/* none at this time */

/*
 * functions:
 */


/* how long would this string be in memory as a compiler would store
   it under (most) of the backslash-escape conventions */

int
compiledStringLength(
                     char *
                     );


/* how long is the string as uncompiled program text. this includes
   quotes but not whitespace */

int
sourceStringLength(
                   char *
                   );


/*
 * the code for each of the day's parts.
 *
 * each expects the name of an input dataset or
 * some other string to drive the part's code.
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
