/* solution.c -- aoc 2016 13 -- troy brumley */

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

#define TXBMISC_IMPLEMENTATION
#include "txbmisc.h"
#define TXBPQ_IMPLEMENTATION
#include "txbpq.h"
#define TXBQU_IMPLEMENTATION
#include "txbqu.h"

/*
 * generate a maze of a particular size using a peculiar algorithm.
 *
 * * coordinates are only positive integers.
 *
 * * Find x*x + 3*x + 2*x*y + y + y*y.
 *
 * * Add the office designer's favorite number (your puzzle input).
 *
 * * Find the binary representation of that sum; count the number of
 *   bits that are 1.
 *
 *   * If the number of bits that are 1 is even, it's an open space.
 *
 *   * If the number of bits that are 1 is odd, it's a wall.
 *
 * a sample maze for favorite number = 10 is provided.
 */

/*
 * private functions for low level maze access.
 */

static int
fnmaze(int x, int y, int magic) {
   return one_bits_in(magic + x*x + 3*x + 2*x*y + y + y*y);
}

static bool
is_valid_maze_coordinate(const maze *m, const maze_coordinate *c) {
   assert(m && c);
   return c->row >= 0 && c->row < m->rows &&
          c->col >= 0 && c->col < m->cols;
}

static bool
is_null_maze_coordinate(const maze_coordinate *c) {
   return c->row == -INT_MAX && c->col == -INT_MAX;
}

static const maze_coordinate
null_maze_coordinate(void) {
   return (maze_coordinate) {
      -INT_MAX, -INT_MAX
   };
}

static bool
equal_maze_coordinate(const maze_coordinate *a, const maze_coordinate *b) {
   assert(a && b);
   if (is_null_maze_coordinate(a) || is_null_maze_coordinate(b)) {
      return false;
   }
   return a->row == b->row && a->col == b->col;
}

/*
 * for iterating over all cell addresses. updates passed coordinate
 * advancing columns first then rows. returns false when there are no
 * more cell addresses.
 */

static bool
next_maze_coordinate(const maze *m, maze_coordinate *c) {
   assert(m && c);
   if (!is_valid_maze_coordinate(m, c)) {
      return false;
   }
   c->col += 1;
   if (c->col < m->cols) {
      return true;
   }
   c->row += 1;
   c->col = 0;
   if (c->row < m->rows) {
      return true;
   }
   return false;
}

/*
 * update a maze coordinate in place by adjusting the row and column
 * by some amount. useful for addressing adjacent cells.
 */

static maze_coordinate *
adjust_maze_coordinate(const maze_coordinate *c, int row_adj, int col_adj) {
   maze_coordinate *d = malloc(sizeof(*d));
   d->row = c->row + row_adj;
   d->col = c->col + col_adj;
   return d;
}

/*
 * returns a queue of all the open reachable adjacent cells in a maze.
 */

static qucb *
adjacent_maze_cells(const maze *m, const maze_coordinate *c) {
   qucb *ac = qu_create();
   if (!is_valid_maze_coordinate(m, c)) {
      return ac;
   }
   int adjacent_adjustments[4][2] = {
      {-1, 0},
      {0, -1}, {0, +1},
      {+1, 0},
   };
   for (int i = 0; i < 4; i++) {
      maze_coordinate *adj = adjust_maze_coordinate(
                                c,
                                adjacent_adjustments[i][0],
                                adjacent_adjustments[i][1]
                             );
      if (is_valid_maze_coordinate(m, adj)) {
         qu_enqueue(ac, adj);
      }
   }
   return ac;
}

/*
 * as in adjacent_maze_cells, but only the open cells.
 */

static qucb *
open_adjacent_maze_cells(const maze *m, const maze_coordinate *c) {
   qucb *ac = qu_create();
   if (!is_valid_maze_coordinate(m, c)) {
      return ac;
   }
   int adjacent_adjustments[4][2] = {
      {-1, 0},
      {0, -1}, {0, +1},
      {+1, 0},
   };
   for (int i = 0; i < 4; i++) {
      maze_coordinate *adj = adjust_maze_coordinate(
                                c,
                                adjacent_adjustments[i][0],
                                adjacent_adjustments[i][1]
                             );
      if (is_valid_maze_coordinate(m, adj) && is_open(m, adj)) {
         qu_enqueue(ac, adj);
      } else {
         free(adj);
      }
   }
   return ac;
}

/*
 * map a coordinate into an offset in the backing array.
 */

static int
offset(const maze *m, const maze_coordinate *c) {
   return c->row * m->cols + c->col;
}

/*
 * lazy initialization accessor for the glyph at a maze grid cell. the
 * glyphs do double duty as they are a displayable cell state marker.
 */

static char
glyph_at(const maze *m, const maze_coordinate *c) {
   assert(is_valid_maze_coordinate(m, c));
   char g = m->grid[offset(m, c)];
   if (g) {
      return g;
   }
   int h = fnmaze(c->col, c->row, m->magic);
   m->grid[offset(m, c)] = is_even(h) ? m->open : m->wall;
   return m->grid[offset(m, c)];
}

/*
 * check cell state.
 */

bool
is_open(const maze *m, const maze_coordinate *c) {
   return glyph_at(m, c) == m->open;
}

bool
is_wall(const maze *m, const maze_coordinate *c) {
   return glyph_at(m, c) == m->wall;
}

/*
 * the main public maze api ... create, destroy, populate, print,
 * pathing.
 */

maze *
create_maze_with_glyphs(int rows, int cols, int magic, char open, char wall) {
   assert(rows > 0 && cols > 0 && open != '\0' && wall != '\0');
   maze *m = malloc(sizeof(maze));
   assert(m);
   memset(m, 0, sizeof(*m));
   m->rows = rows;
   m->cols = cols;
   m->magic = magic;
   m->open = open;
   m->wall = wall;
   m->grid = malloc((m->rows + 1) * (m->cols + 1) * sizeof(char));
   assert(m->grid);
   memset(m->grid, 0, (m->rows + 1) * (m->cols + 1) * sizeof(char));
   return m;
}

maze *
create_maze(int rows, int cols, int magic) {
   return create_maze_with_glyphs(rows, cols, magic, '.', '#');
}

void
destroy_maze(maze *m) {
   assert(m);
   memset(m->grid, 253, (m->rows + 1) * (m->cols + 1) * sizeof(char));
   free(m->grid);
   memset(m, 253, sizeof(*m));
   free(m);
}

/*
 * while the maze configuration can be calculated just in time, this
 * function will instantiate the entire maze.
 */

void
populate_maze(maze *m) {
   assert(m);
   maze_coordinate c;
   c.row = 0;
   c.col = 0;
   do {
      glyph_at(m, &c);
   } while (next_maze_coordinate(m, &c));
}

/*
 * print maze on stream *f.
 */

void
fprint_maze(FILE *f, const maze *m) {
   fprintf(f, "\nmaze %d rows %d columns, magic=%d\n\n", m->rows, m->cols, m->magic);
   fprintf(f, "    ");
   for (int x = 0; x < m->cols; x++) {
      fprintf(f, "%d", x % 10);
   }
   fprintf(f, "\n");
   maze_coordinate c;
   for (int y = 0; y < m->rows; y++) {
      c.row = y;
      fprintf(f, "%3d ", y);
      for (int x = 0; x < m->cols; x++) {
         c.col = x;
         fprintf(f, "%c", glyph_at(m, &c));
      }
      fprintf(f, "\n");
   }
   fprintf(f, "\n");
}

/*
 * print maze on stdout.
 */
void
print_maze(const maze *m) {
   fprint_maze(stdout, m);
}

/*
 * create a copy of a maze coordinate.
 */

maze_coordinate *
dup_maze_coordinate(const maze_coordinate *c) {
   maze_coordinate *d = malloc(sizeof(maze_coordinate));
   d->row = c->row;
   d->col = c->col;
   return d;
}

/*
 * print distances calculated instead of glyphs.
 */

void
fprint_distances(FILE *f, const maze *m, int *distances) {
   fprintf(f, "\nmaze %d rows %d columns, magic=%d\n\n", m->rows, m->cols, m->magic);
   fprintf(f, "    ");
   for (int x = 0; x < m->cols; x++) {
      fprintf(f, "%d", x % 10);
   }
   fprintf(f, "\n");
   maze_coordinate c;
   for (int y = 0; y < m->rows; y++) {
      c.row = y;
      fprintf(f, "%3d ", y);
      for (int x = 0; x < m->cols; x++) {
         c.col = x;
         if (is_open(m, &c)) {
            fprintf(f, " %2d", distances[offset(m, &c)]);
         } else {
            fprintf(f, "   ");
         }
      }
      fprintf(f, "\n");
   }
   fprintf(f, "\n");
}

void
print_distances(const maze *m, int *distances) {
   fprint_distances(stdout, m, distances);
}

/*
 * find the length of the shortest path between two points in a maze. uses
 * a dijkstra like approach.
 *
 * BUG: this can terminate early without reaching the to coordinate correctly.
 * by overallocating the maze, which can be done for this day's problem, i
 * get correct answers working from either end. if i allocate the maze tightly,
 * with only one extra row and column past the lower right coordinate, i
 * don't get a solution.
 *
 * in any case, not all open cells are visited, so there's more work to be
 * done with this.
 */

int
shortest_path(const maze *m, const maze_coordinate from, const maze_coordinate to) {
   int dist = -INT_MAX;
   int *distances = malloc((m->rows + 1) * (m->cols + 1) * sizeof(int));
   memset(distances, -1, (m->rows + 1) * (m->cols + 1) * sizeof(int));
   qucb *wq = qu_create();
   qu_enqueue(wq, dup_maze_coordinate(&from));
   distances[offset(m, &from)] = 0;

   while (!qu_empty(wq)) {
      maze_coordinate *c = qu_dequeue(wq);
      /* printf("%d,%d dist=%d  next=", c->row, c->col, distances[offset(m, c)]); */
      if (equal_maze_coordinate(&to, c)) {
         dist = distances[offset(m, c)];
         /* printf("done\n"); */
         free(c);
         break;
      }
      qucb *adj = open_adjacent_maze_cells(m, c);
      while (!qu_empty(adj)) {
         maze_coordinate *a = qu_dequeue(adj);
         /* printf("[%d,%d %c dist=%d]", a->row, a->col, glyph_at(m, a), distances[offset(m, a)]); */
         if (distances[offset(m, a)] < 0) {
            distances[offset(m, a)] = distances[offset(m, c)] + 1;
            qu_enqueue(wq, a);
         } else {
            free(a);
         }
      }
      /* printf("\n"); */
      qu_destroy(adj);
      free(c);
   }
   while (!qu_empty(wq)) {
      qu_dequeue(wq);
   }
   qu_destroy(wq);
   /* print_distances(m, distances); */
   free(distances);
   return dist;
}

/*
 * part one: how many steps to get from a particular point a to
 * a point b.
 */

int
part_one(
   const char *fname
) {

   maze  *m = create_maze(49, 42, 1350);
   maze_coordinate from = (maze_coordinate) {
      1, 1
   };
   maze_coordinate to = (maze_coordinate) {
      39, 31
   };

   int dist = shortest_path(m, from, to);

   printf("part one: %d\n", dist);

   return EXIT_SUCCESS;
}


/*
 * part two: how many distinct points can be reached in
 * at most fifty steps.
 */

int
part_two(
   const char *fname
) {

   maze  *m = create_maze(49, 42, 1350);
   maze_coordinate from = (maze_coordinate) {
      1, 1
   };
   maze_coordinate to = (maze_coordinate) {
      39, 31
   };

   int dist = shortest_path(m, from, to);

   printf("part two: %d\n", 0);

   return EXIT_SUCCESS;
}
