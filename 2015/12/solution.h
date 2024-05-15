/* solution.h -- aoc 2015 06 -- troy brumley */
#ifndef SOLUTION_H
#define SOLUTION_H

/*
 * parsing numbers out of a json string.
 *
 */


/* s is a valid json string with no whitespace. remove any array with
   an attribute value of a. s is updated in place by writing blanks
   over the arrays as they are removed. also returns the number of
   arrays scrubbed. */

int
scrubObjects(
   char *s,             /* string buffer */
   char *a              /* attribute that triggers scrub */
);

/* s is a valid json string which may contain whitespace (as after
   being modified by scrubObjects). returns the sum of the numbers
   found in the string. */

int
sumNumbers(
   char *from,          /* start in string buffer */
   char *to             /* end in string buffer, or NULL end of string */
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
   char *                       /* a file name */
);


int
partTwo(
   char *                       /* a file name */
);


/* end solution.h */
#endif /* SOLUTION_H */
