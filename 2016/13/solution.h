/* solution.h -- aoc 2016 13 -- troy brumley */

#ifndef SOLUTION_H
#define SOLUTION_H

#define INPUT_LINE_MAX 4096

#include <stdbool.h>
#include <stdio.h>
#include "txbdl.h"

/*
 * a maze ing control block.
 *
 * for today's advent of code problem we don't really need backing
 * storage since the grid can be calculated, but i may want to
 * reuse this in other cases.
 *
 * rows and columns are positive, origin at upper left.
 *
 * the grid values provide a visual representation of the grid
 * cell's state. as such, they should be displayable. the defaults
 * are . for an open space and # for a wall.
 *
 * lazy initialization is used throughout.
 */

typedef struct maze_coordinate maze_coordinate;
struct maze_coordinate {
   int row; /* y oh y */
   int col; /* x */
};

typedef struct maze maze;
struct maze {
   int rows;   /* y */
   int cols;   /* x */
   int magic;  /* hash number */
   char open;  /* visuals */
   char wall;
   char *grid; /* 0..rows of 0..cols */
};

maze *
create_maze_with_glyphs(
   int rows,
   int cols,
   int magic,
   char open,
   char wall
);

maze *
create_maze(
   int rows,
   int cols,
   int magic
);

void
destroy_maze(
   maze *m
);

void
populate_maze(
   maze *m
);

bool
is_open(
   const maze *m,
   const maze_coordinate *c
);

void
fprint_maze(
   FILE *f,
   const maze *m
);

void
print_maze(
   const maze *m
);

int
shortest_path_length(
   const maze *m,
   const maze_coordinate from,
   const maze_coordinate to
);

dlcb *
cells_within_path_length(
   const maze *m,
   const maze_coordinate from,
   const maze_coordinate to,
   int max_path
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
