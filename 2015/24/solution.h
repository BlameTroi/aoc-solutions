/* solution.h -- aoc 2015 24 -- troy brumley */
#ifndef SOLUTION_H
#define SOLUTION_H

/*
 * allocate packages in santa's sleigh
 */

#include <stdbool.h>

#define INPUT_LEN_MAX 255

extern int *packages;
extern int *groupings;
extern int num_packages;
extern int total_weight;
extern int target_groupings;
extern int target_weight;
extern int min_packages;
extern int max_packages;


unsigned long
try_permutation(
   const char *,
   int
);

unsigned long
try_combination(
   const char *,
   int
);

bool
load_data(
   const char *,
   bool,
   int);

void
zero_data(void);

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
