/* solution.h -- aoc 2015 18 -- troy brumley */
#ifndef SOLUTION_H
#define SOLUTION_H

/*
 * play a grid of lights following the rules of conway's game of life.
 */


/*
 * necessary constants and definitions:
 */

/* how long may a line in the test dataset be? */

#define INPUT_LEN_MAX 255

/* 100x100 is the large test dataset */

#define DIMENSION_MAX 100

/* glyphs have meaning */

#define G_ON '#'
#define G_OFF '.'

/*
 * typedefs:
 */

typedef struct page_t {
   char p[DIMENSION_MAX][DIMENSION_MAX];
} page_t;


/*
 * data structures:
 */

int dim_rows;         /* these are the same for this day's problem, but */
int dim_cols;         /* there's no harm in coding to allow for different */
int iter;
int stuck_rules;      /* some lights are stuck on */

/* to achive simultanity, page flip as in some graphics modes */

page_t page_one;
page_t page_two;

page_t *reading;
page_t *writing;


/*
 * functions:
 */

void
reset_state(int rel);


/*
 * in part two, some lights in the grid are stuck on. is this a stuck
 * light?
 */

int
stuck_p(int row, int col);


/*
 * is a light on or off. coordinates can be out of bounds and if they
 * are, off is reported.
 */

int
on_p(int row, int col);


/*
 * return the number of neighboring lights that are on.
 */

int
neighbors_on(int row, int col);


/*
 * decide this light's upcoming state based on its state and
 * that of its neighbors. returns 0 if light will be off,
 * or 1 if on.
 */

int
decide(int row, int col);


/*
 * turn a light on or off as directed. honors stuckness if stuck_rules
 * is true.
 */

void
turn_on(int row, int col);

void
turn_off(int row, int col);


/*
 * process a cycle of lights by checking each light in the reading
 * page and sets it on or off in the writing page.
 */

int
cycle_lights(void);


/*
 * flip display pages.
 */

void
flip_pages(void);


/*
 * the code for each of the day's parts.
 *
 * each expects the name of an input dataset or some other string to
 * drive the part's code.
 *
 * should print the result to stdout.
 *
 * returns EXIT_SUCCESS or EXIT_FAILURE.
 */


int
partOne(
   char *                       /* a file name */
);


int
partTwo(
   char *                       /* a file name */
);


/* end solution.h */
#endif /* SOLUTION_H */
