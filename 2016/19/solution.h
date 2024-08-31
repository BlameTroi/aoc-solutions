/* solution.h -- aoc 2016 19 -- troy brumley */

#ifndef SOLUTION_H
#define SOLUTION_H

#define INPUT_LINE_MAX 4096

#define SAMPLE 5
#define LIVE   3018458

int
josephus_adjacent(int num);

int
josephus_opposite(int num);

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
