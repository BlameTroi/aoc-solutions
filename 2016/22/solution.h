/* solution.h -- aoc 2016 01 -- troy brumley */

#ifndef SOLUTION_H
#define SOLUTION_H

#include "txbrs.h"

#define INPUT_LINE_MAX 4096


#define X_DIM 35
#define Y_DIM 27

typedef struct loc loc;
struct loc {
   int x;
   int y;
};

typedef struct node node;
struct node {
   /*Filesystem              Size  Used  Avail  Use%
   /dev/grid/node-x0-y0     90T   69T    21T   76%
   /dev/grid/node-x0-y1     88T   71T    17T   80%
    */
   loc loc;
   int size;
   int used;
   int avail;
   int pct;
};

void
load_grid(rscb *input);


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
