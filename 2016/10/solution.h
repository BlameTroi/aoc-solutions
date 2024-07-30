/* solution.h -- aoc 2016 10 -- troy brumley */

#ifndef SOLUTION_H
#define SOLUTION_H

#define INPUT_LINE_MAX 4096

#include <stdbool.h>
#include "txbfs.h"

/*
 * the graph is initialized by sending values to the bots.
 */

#define VALUES_MAX    75
#define BIN_MAX       20
#define BOTS_MAX     210

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
