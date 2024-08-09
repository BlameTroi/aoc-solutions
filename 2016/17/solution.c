/* solution.c -- aoc 2016 xx -- troy brumley */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define TXBMD5_IMPLEMENTATION
#include "txbmd5.h"

#define TXBSB_IMPLEMENTATION
#include "txbsb.h"

#define TXBRS_IMPLEMENTATION
#include "txbrs.h"

#include "solution.h"

/* hash decoding. only the first four characters of the hash are
 * significant. they indicate the state of the corresponding door
 * (UDLR). any digit b->f means the door is open, digits 0->9 mean the
 * door is closed and now locked. */

maze_state *
create_maze(
   const char *passcode,
   int rows, int cols,
   int start_row, int start_col,
   int end_row, int end_col
) {
   maze_state *m = malloc(sizeof(*m));
   m->passcode = passcode;
   m->rows = rows;
   m->cols = cols;
   m->row = start_row;
   m->col = start_col;
   m->start_row = start_row;
   m->start_col = start_col;
   m->end_row = end_row;
   m->end_col = end_col;
   m->moves = sb_create_blksize(rows * cols * 4);
   m->are_current = false;
   return m;
}

const char *
get_current_path(
   const maze_state *m
) {
   return sb_to_string(m->moves);
}

bool
at_end(
   const maze_state *m
) {
   return m->row == m->end_row && m->col == m->end_col;
}

void
destroy_maze(
   maze_state *m
) {
   if (m->moves) {
      sb_destroy(m->moves);
   }
   free(m);
}

maze_state *
copy_maze(
   const maze_state *m
) {
   maze_state *copy = malloc(sizeof(*copy));
   memcpy(copy, m, sizeof(*copy));
   copy->moves = sb_create_blksize(m->rows * m->cols * 4);
   char *str = sb_to_string(m->moves);
   sb_puts(copy->moves, str);
   copy->are_current = false;
   free(str);
   return copy;
}

void
update_directions(maze_state *m) {
   uint8_t digest[16];
   char *source = malloc(strlen(m->passcode) + sb_length(m->moves) + 1);
   source[0] = '\0';
   strcpy(source, m->passcode);
   char *moves = sb_to_string(m->moves);
   strcat(source, moves);
   md5_string(source, digest);
   m->up = (digest[0] >> 4) > 0x0a;
   m->down = (digest[0] & 0x0f) > 0x0a;
   m->left = (digest[1] >> 4) > 0x0a;
   m->right = (digest[1] & 0x0f) > 0x0a;
   m->are_current = true;
   free(source);
   free(moves);
}

bool
is_up_open(maze_state *m) {
   if (m->are_current) {
      return m->up;
   }
   update_directions(m);
   return m->up;
}

bool
is_down_open(maze_state *m) {
   if (m->are_current) {
      return m->down;
   }
   update_directions(m);
   return m->down;
}

bool
is_left_open(maze_state *m) {
   if (m->are_current) {
      return m->left;
   }
   update_directions(m);
   return m->left;
}

bool
is_right_open(maze_state *m) {
   if (m->are_current) {
      return m->right;
   }
   update_directions(m);
   return m->right;
}

bool
can_move_up(maze_state *m) {
   return is_up_open(m) && m->row > 0;
}

bool
can_move_down(maze_state *m) {
   return is_down_open(m) && m->row < m->rows - 1;
}

bool
can_move_left(maze_state *m) {
   return is_left_open(m) && m->col > 0;
}

bool
can_move_right(maze_state *m) {
   return is_right_open(m) && m->col < m->cols - 1;
}

bool
can_move(maze_state *m, char direction) {
   switch (direction) {
   case UP:
      return can_move_up(m);
   case DOWN:
      return can_move_down(m);
   case LEFT:
      return can_move_left(m);
   case RIGHT:
      return can_move_right(m);
   default:
      assert(NULL);
   }
}

int
possible_moves(maze_state *m) {
   int res = 0;
   if (can_move_up(m)) {
      res += 1;
   }
   if (can_move_down(m)) {
      res += 1;
   }
   if (can_move_left(m)) {
      res += 1;
   }
   if (can_move_right(m)) {
      res += 1;
   }
   return res;
}

bool
do_move(maze_state *m, char direction) {
   bool ok = false;
   int row_delta = 0;
   int col_delta = 0;
   switch (direction) {
   case UP:
      ok = can_move_up(m);
      row_delta = -1;
      break;
   case DOWN:
      ok = can_move_down(m);
      row_delta = 1;
      break;
   case LEFT:
      ok = can_move_left(m);
      col_delta = -1;
      break;
   case RIGHT:
      ok = can_move_right(m);
      col_delta = 1;
      break;
   default:
      assert(NULL);
   }
   if (ok) {
      sb_putc(m->moves, direction);
      m->row += row_delta;
      m->col += col_delta;
      m->are_current = false;
   }
   return ok;
}

bool
do_move_up(maze_state *m) {
   return do_move(m, UP);
}

bool
do_move_down(maze_state *m) {
   return do_move(m, DOWN);
}

bool
do_move_left(maze_state *m) {
   return do_move(m, LEFT);
}

bool
do_move_right(maze_state *m) {
   return do_move(m, RIGHT);
}

bool
find_shortest_path(
   maze_state *m
) {
   return false;
}

/*
 * part one:
 *
 */

int
part_one(
   const char *fname
) {

   FILE *ifile = fopen(fname, "r");
   if (!ifile) {
      fprintf(stderr, "error: could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }

   char iline[INPUT_LINE_MAX];

   while (fgets(iline, INPUT_LINE_MAX - 1, ifile)) {
   }

   printf("part one: %d\n", 0);

   fclose(ifile);
   return EXIT_SUCCESS;
}


/*
 * part two:
 *
 */

int
part_two(
   const char *fname
) {
   FILE *ifile;

   ifile = fopen(fname, "r");
   if (!ifile) {
      fprintf(stderr, "error: could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }
   char iline[INPUT_LINE_MAX];

   while (fgets(iline, INPUT_LINE_MAX - 1, ifile)) {
   }

   printf("part two: %d\n", 0);

   fclose(ifile);
   return EXIT_SUCCESS;
}
