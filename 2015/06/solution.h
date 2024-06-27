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

/* data structures: */


/* coordinate block */
typedef struct coord_t {
   int x;
   int y;
} coord_t;


/* use a grid of booleans */
typedef struct lights_t {
   long lit;                            /* number of lights lit */
   long intensity;                      /* total intensity if in analog mode */
   void (*fnon)(struct lights_t *, coord_t);   /* pointers to proper on/off/toggle */
   void (*fnoff)(struct lights_t *, coord_t);  /* implementations for the grid mode */
   void (*fntog)(struct lights_t *, coord_t);
   int bulb[1000][1000];                /* the lights in the grid */
   int digital;                         /* digital (part_one) or analog (part_two) */
} lights_t;


/* commands symbolically */
typedef enum cmd_e {
   e_invalid,
   e_on,
   e_off,
   e_toggle
} cmd_e;


/* command block */
typedef struct cmd_t {
   cmd_e cmd;
   coord_t p0;
   coord_t p1;
} cmd_t;


/* functions: */


/* let there be lights */
lights_t *
initGrid(
   int                /* 1 = use digital interface, 0 = use analog interface */
);


/* entropy wins */
void
freeGrid(
   lights_t *          /* light grid */
);


/* parse command line into our command structure */
cmd_t
parseCmd(
   const char *,       /* command line text */
   int                 /* length to consume, makes buffered reads easier */
);


/*
 * how many lights are on?
 */
long
numberOn(
   lights_t *          /* light grid */
);


/*
 * total intensity of lights.
 */
long
totalIntensity(
   lights_t *          /* light grid */
);

/*
 * turn single light on, off, or toggle it. The 'D' versions
 * are for digital, either on or off. The 'A' versions are
 * for analog, where a dial adjusts intensity.
 */
void
turnOnD(
   lights_t *,           /* light grid */
   coord_t               /* where */
);

void
turnOffD(
   lights_t *,          /* light grid */
   coord_t              /* where */
);

void
toggleD(
   lights_t *,           /* light grid */
   coord_t               /* where */
);

void
turnOnA(
   lights_t *,           /* light grid */
   coord_t               /* where */
);

void
turnOffA(
   lights_t *,          /* light grid */
   coord_t              /* where */
);

void
toggleA(
   lights_t *,           /* light grid */
   coord_t               /* where */
);

/*
 * single light's status.
 */
int
isLit(
   lights_t *,            /* light grid */
   coord_t                /* where */
);

int
intensity(
   lights_t *,        /* light grid */
   coord_t            /* where */
);


/*
 * by your command
 */
void
doCmd(
   lights_t *,            /* light grid */
   cmd_t                  /* parsed command line */
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
