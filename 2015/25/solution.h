/* solution.h -- aoc 2015 13 -- troy brumley */
#ifndef SOLUTION_H
#define SOLUTION_H

/*
 * security codes are laid out in a triangular 'grid' where positions
 * are identified as where they lie in the triangular number sequence.
 *
 * given an arbitrary row and column, what is the code value?
 */

/*
 * upper right position value for 1,n. this is the n-th
 * number of the triangular number sequence.
 */

unsigned long
diag_end(
   int n
);

/*
 * lower left position value for n,1. while not on the triangular
 * number sequence, it has a positional value for this problem.
 */

unsigned long
diag_begin(
   int n
);

/*
 * the 'security code' for the problem is a hashing calculation. prior
 * value * factor % divisor, with a set seed value.
 */

#define SEED         20151125
#define FACTOR         252533
#define DIVISOR      33554393

unsigned long
next_code(
   unsigned long code
);

/*
 * given a row and column in the triangular number grid of security
 * codes, return the code value
 */

unsigned long
code_at(
   int row,
   int col
);


#define INPUT_LEN_MAX 255

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
