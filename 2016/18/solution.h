/* solution.h -- aoc 2016 01 -- troy brumley */

#ifndef SOLUTION_H
#define SOLUTION_H

#include <stdbool.h>

#define SAFE '.'
#define TRAP '^'

char
upper_left(int row, int col);

char
upper_right(int row, int col);

char
upper_middle(int row, int col);

bool
is_trap(int row, int col);

void
initialize_floor_map(int rows, int cols, const char *data);

void
fill_floor_map(void);

void
count_map(int *trap, int *safe);

void
print_map(void);

#define INPUT_LINE_MAX 4096

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
