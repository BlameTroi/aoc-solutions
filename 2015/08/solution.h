/* solution.h -- aoc 2015 08 -- troy brumley */
#ifndef SOLUTION_H
#define SOLUTION_H

/*
 * aoc 2015 day 8, matchsticks
 *
 * this is an escaped encoding problem where strings containing c-like
 * backslash escapes (\\, \", and \x??) are provided and we need to
 * determine the lengths those strings would occupy in storage after
 * compilation, and then again how long the strings are if we expand
 * the literals to a format that would compile back down to the
 * originals.
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
   const char *
);


/* how long is the string as uncompiled program text. this includes
   quotes but not whitespace */

int
sourceStringLength(
   const char *
);


/* In addition to finding the number of characters of code, you should
   now encode each code representation as a new string and find the
   number of characters of the new encoded representation, including
   the surrounding double quotes. */

char *
encodedString(
   const char *
);


/* how long is the string after it's been escape encoded? this actually
   turns out to be strlen, but i'm keeping the wrapper in place. */

int
encodedStringLength(
   const char *
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
   const char *                       /* a file name */
);


int
partTwo(
   const char *                       /* a file name */
);

/* end solution.h */
#endif /* SOLUTION_H */
