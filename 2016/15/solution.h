/* solution.h -- aoc 2016 01 -- troy brumley */

#ifndef SOLUTION_H
#define SOLUTION_H

#define INPUT_LINE_MAX 4096

typedef struct disc disc;

struct disc {
   int positions;
   int start_position;
};

long
spinner(int part);

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
