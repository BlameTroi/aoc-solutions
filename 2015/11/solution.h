/* solution.h -- aoc 2015 06 -- troy brumley */
#ifndef SOLUTION_H
#define SOLUTION_H

#include <stdbool.h>

/*
 * elven password generation
 */


/*
 * necessary constants and definitions:
 */

/* how long may a line in the test dataset be? */

#define INPUT_LEN_MAX 255


/*
 * functions:
 */

bool
password_p(
        const char *candidate /* a password string */
);

char *
password_increment(
        char *password
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
