/* solution.h -- aoc 2016 16 -- troy brumley */

#ifndef SOLUTION_H
#define SOLUTION_H

#define INPUT_LINE_MAX 4096

char *
dragon_expand(
   const char *a
);

char *
dragon_checksum(
   const char *a
);

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
