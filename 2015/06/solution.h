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
 *
 * part one was binary, on, off or toggle.
 *
 * part two is analog, on = brightness+1, off = brightness-1 (min 0), toggle = brightness+2.
 *
 * this requires changing the data structure from bools to ints.
 */

#include <stdbool.h>

/* data structures: */


/* coordinate block */
typedef struct coord coord;
struct coord {
	int x;
	int y;
};


/* use a grid of booleans */
typedef struct lights lights;
struct lights {
	long lit;                            /* number of lights lit */
	long intensity;                      /* total intensity if in analog mode */
	void (*fnon)(lights *, coord);   /* pointers to proper on/off/toggle */
	void (*fnoff)(lights *, coord);  /* implementations for the grid mode */
	void (*fntog)(lights *, coord);
	int bulb[1000][1000];                /* the lights in the grid */
	int digital;                         /* digital (part_one) or analog (part_two) */
};


/* commands symbolically */
typedef enum cmd_e {
	e_invalid,
	e_on,
	e_off,
	e_toggle
} cmd_e;


/* command block */
typedef struct cmd cmd;
struct cmd {
	cmd_e cmd;
	coord p0;
	coord p1;
};


/* functions: */


/* let there be lights */
lights *
init_grid(
        bool                /* 1 = use digital interface, 0 = use analog interface */
);


/* entropy wins */
void
free_grid(
        lights *          /* light grid */
);


/* parse command line into our command structure */
cmd
parse_cmd(
        const char *,       /* command line text */
        int                 /* length to consume, makes buffered reads easier */
);


/*
 * how many lights are on?
 */
long
number_on(
        lights *          /* light grid */
);


/*
 * total intensity of lights.
 */
long
total_intensity(
        lights *          /* light grid */
);

/*
 * turn single light on, off, or toggle it. The 'D' versions
 * are for digital, either on or off. The 'A' versions are
 * for analog, where a dial adjusts intensity.
 */

void
turn_on_d(
        lights *,           /* light grid */
        coord               /* where */
);

void
turn_off_d(
        lights *,          /* light grid */
        coord              /* where */
);

void
toggle_d(
        lights *,           /* light grid */
        coord               /* where */
);

void
turn_on_a(
        lights *,           /* light grid */
        coord               /* where */
);

void
turn_off_a(
        lights *,          /* light grid */
        coord              /* where */
);

void
toggle_a(
        lights *,           /* light grid */
        coord               /* where */
);

/*
 * single light's status.
 */
bool
is_lit(
        lights *,            /* light grid */
        coord                /* where */
);

int
intensity(
        lights *,        /* light grid */
        coord            /* where */
);


/*
 * by your command
 */
void
do_cmd(
        lights *,            /* light grid */
        cmd                  /* parsed command line */
);


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
part_one(
        const char *
);

int
part_two(
        const char *
);

/* end solution.h */
#endif /* SOLUTION_H */
