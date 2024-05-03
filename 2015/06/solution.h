/* solution.h -- aoc 2015 06 -- troy brumley */
#ifndef SOLUTION_H
#define SOLUTION_H

/*
 * probably a fire hazard
 *
 * lights in a 0,0 -> 999,999 grid are turned off and on
 * as directed, how many lights are left lit at the end
 * of the run if all the lights are off at the start.
 *
 * (turn on|turn off|toggle) x0,y0 through x1,y1
 */

#include <stdbool.h>

/* data structures: */


/* commands symbolically */
typedef enum cmd_e {
   invalid,
   on,
   off,
   toggle
} cmd_e;


/* use a grid of booleans */
typedef struct lights_t {
   int lit;
   bool bulb[1000][1000];
} lights_t;


/* coordinate block */
typedef struct coord_t {
   int x;
   int y;
} coord_t;


/* command block */
typedef struct cmd_t {
   cmd_e cmd;
   coord_t p0;
   coord_t p1;
} cmd_t;


/* functions: */


/* let there be lights */
lights_t
*initGrid(void);


/* entropy wins */
void
freeGrid(lights_t *);


/* parse command line into our command structure */
cmd_t
parseCmd(char *line, int len);


/*
 * the code for each of the day's parts.
 *
 * each expects the name of an input dataset or
 * some other string to drive the part's code.
 *
 * should print the result to stdout.
 *
 * returns EXIT_SUCCESS or EXIT_FAILURE.
 */

int
partOne(char *);

int
partTwo(char *);

/* end solution.h */
#endif /* SOLUTION_H */
