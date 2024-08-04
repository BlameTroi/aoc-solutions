/* solution.h -- aoc 2016 01 -- troy brumley */

#ifndef SOLUTION_H
#define SOLUTION_H

#include <stdbool.h>
#include <stdint.h>

#include "txbqu.h"

#define INPUT_LINE_MAX 4096

/*
 * since the data would all fit in 16 bits, i opted for using bitwise
 * operations.
 *
 * interestingly, #defines of the form 0x00000001 don't work as i
 * expect. one misses the rational mainframe memory layout.
 *
 * there are five elements in the live data, and two in the sample.
 * each element has a generator paired with a microchip.
 *
 * as i (along with almost everyone else it seems) struggled with the
 * problem performance, i finally learned that the element names are
 * not significant when checking for duplicate moves, and that the
 * distinction only comes into play when generating a possible move.
 *
 * i'm doing my third tear-it-up-and-try-again. i'm keeping bit
 * storage but splitting generators and microchips into separate
 * fields. the savings over an array are negligible, but i want the
 * practice.
 */

/*
 * there are five 'elements' in the live problem, and two in the
 * illustrative sample. a byte is enough. elements start at bit 0.
 */

#define promethium (uint8_t)(1 << 0)
#define cobalt     (uint8_t)(1 << 1)
#define curium     (uint8_t)(1 << 2)
#define ruthenium  (uint8_t)(1 << 3)
#define plutonium  (uint8_t)(1 << 4)
#define LIVE_ELEMENTS 5

#define elerium    (uint8_t)(1 << 5)
#define dilithium  (uint8_t)(1 << 6)
#define PART_TWO_ELEMENTS 7

#define hydrogen   (uint8_t)(1 << 0)
#define lithium    (uint8_t)(1 << 1)
#define SAMPLE_ELEMENTS 2

#define MASK_FOR(e)   (uint8_t)(1 << (e))

/* the floors of the building. */
enum e_levels { f1 = 0, f2 = 1, f3 = 2, f4 = 3 };
#define LEVEL_DIM 4

/* the set of possible elements. */
enum e_element { e1 = 0, e2 = 1, e3 = 2, e4 = 3, e5 = 4 };

/* state of a move */
typedef struct state state;
struct state {
   int no;
   uint8_t elevator;
   uint8_t microchips[LEVEL_DIM];
   uint8_t generators[LEVEL_DIM];
};

char *
trace_line(
   state *m,
   char *buffer,
   int buflen
);

/*
 * the apparent key to understanding the problem is to not worry so
 * much about specific elements as long as you've paired microchips
 * and generators that you move. so count of each should be
 * sufficient.
 */

typedef struct dupcheck dupcheck;
struct dupcheck {
   uint8_t elevator;
   uint8_t counts[LEVEL_DIM][2];
};

dupcheck
state_as_dupcheck(
   state *m
);

bool
id_duplicate(
   state *m
);

/*
 * query a move state.
 */

bool
is_valid(
   state *m
);

bool
is_goal(
   state *m
);

bool
is_duplicate(
   state *m
);

/*
 * generate a valid next move.
 */

qucb *
next_moves(
   state *m
);

/*
 * work the moves.
 */

void
init_and_load(
   state *,
   bool use_live
);

int
seek_r(
   state *m
);

int
seek_goal(
   state *start
);

/*
 * mains for each problem part.
 */

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
