/* solution.h -- aoc 2016 01 -- troy brumley */

#ifndef SOLUTION_H
#define SOLUTION_H

#include "txbmd5.h"
#include "txbsb.h"

/* your passcode is the prefix for the value to take the md5 hash of.
 * passcode + sequence of moves. */

/* a sample passcode for exposition, it doesn't work well */

#define BAD_PASSCODE "hijkl"

/* these passcodes and shortest paths were provided for verification. */

#define GOOD_PASSCODE_1 "ihgpwlah"
#define GOOD_PATH_1 "DDRRRD"
#define GOOD_LONGEST_1 370

#define GOOD_PASSCODE_2 "kglvqrro"
#define GOOD_PATH_2 "DDUDRLRRUDRD"
#define GOOD_LONGEST_2 492

#define GOOD_PASSCODE_3 "ulqzkmiv"
#define GOOD_PATH_3 "DRURDRUDDLLDLUURRDULRLDUUDDDRR"
#define GOOD_LONGEST_3 830

/* the assigned passcode for parts one and two */

#define LIVE_PASSCODE   "awrkjxxr"

/* moves made */

#define UP     'U'
#define DOWN   'D'
#define LEFT   'L'
#define RIGHT  'R'

typedef struct maze_state maze_state;
struct maze_state {
   int rows;             /* maze dimensions, 0,0 is upper left corner */
   int cols;
   int start_row;        /* starting position, usually 0,0 */
   int start_col;
   int end_row;          /* desired end position */
   int end_col;
   int row;              /* current position */
   int col;
   const char *passcode;
   sbcb *moves;          /* moves to get to this state, UDLR */
   char last_move;       /* can be extracted from moves, but meh */
   int count_moves;      /* same observation */
   bool are_current;     /* are the available directions current? */
   bool up;              /* possible directions based on moves and salt */
   bool down;
   bool left;
   bool right;
};

maze_state *
create_maze(
   const char *passcode,
   int rows, int cols,
   int start_row, int start_col,
   int end_row, int end_col
);

bool
at_end(
   const maze_state *m
);

void
get_position(
   maze_state *m,
   int *row,
   int *col
);

const char *
get_move_history(
   const maze_state *m
);

maze_state *
find_shortest_path(
   const maze_state *m
);

maze_state *
find_longest_path(
   const maze_state *m
);

maze_state *
copy_maze(
   const maze_state *m
);

void
destroy_maze(
   maze_state *m
);

bool
is_up_open(maze_state *m);
bool
is_down_open(maze_state *m);
bool
is_left_open(maze_state *m);
bool
is_right_open(maze_state *m);

bool
can_move_up(maze_state *m);
bool
can_move_down(maze_state *m);
bool
can_move_left(maze_state *m);
bool
can_move_right(maze_state *m);

bool
can_move(
   maze_state *m,
   char direction
);

int
number_of_possible_moves(
   maze_state *m
);

bool
do_move(
   maze_state *m,
   char direction
);

bool
do_move_up(maze_state *m);
bool
do_move_down(maze_state *m);
bool
do_move_left(maze_state *m);
bool
do_move_right(maze_state *m);

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
