/* solution.h -- aoc 2015 19 -- troy brumley */
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

/* observed longest line is 506, but i like to pad */

#include <stdbool.h>
#include <stddef.h>

#include "txblistd.h"

#define INPUT_LEN_MAX 1024

/* observed plus padding to avoid gratuitous mallocs */

#define FROM_MAX 16
#define TO_MAX 16

/* actually should fit with ease in input len max, but add in transform to max */

#define TRANSFORMED_MAX 1040

/* number of transforms allowed for, input is < 50 */

#define TRANSFORM_MAX 50

/*
 * typedefs:
 */


/* given from => to in input */

typedef struct transform_t {
   size_t from_len;
   size_t to_len;
   char from[FROM_MAX];
   char to[TO_MAX];
} transform_t;

/*
 * data structures:
 */

/* output transformations as an array of string pointers. grows dynamically by doubling in size. */

int num_transforms;
transform_t *transforms[TRANSFORM_MAX];

char base[INPUT_LEN_MAX];

int num_run;
listd_control_t transformations_list;

/*
 * functions:
 */


void
reset_state(int rel);

bool
parse_line(
   const char *iline
);

char *
transformer(
   transform_t *t,
   const char *s,
   int *pos);


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
