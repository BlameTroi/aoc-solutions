/* solution.h -- aoc 2015 14 -- troy brumley */
#ifndef SOLUTION_H
#define SOLUTION_H

/*
 * reindeer race but their pace is discontinuous. simulate a race and determine
 * how far the leading reindeer has travelled after some time interval. speed
 * changes (start/stop) are instantaneous.
 *
 */


/*
 * necessary constants and definitions:
 */

/* how long may a line in the test dataset be? */
#define INPUT_LEN_MAX 255

/* largest reindeer dataset, actually 8 plus rudolph so 9 would work */

#define RACERS_MAX 10


/*
 * typedefs:
 */

typedef struct race_trace_t {
	char *name;     /* you know dancer and dasher and comet and vixen ... */
	int speed;      /* km/s not that it matters */
	int burst;      /* seconds of sustained flight before */
	int rest;       /* seconds of sustained rest before flight resumes */
	int flying;     /* 1 = flying, 0 = resting */
	int ticks;      /* how many tickes left before next state change */
	int distance;   /* travelled to the current second */
	int score;      /* how many seconds in the lead */
} race_trace_t;

/*
 * data structures:
 */

int timer;
int num_racers;
race_trace_t racers[RACERS_MAX];

/*
 * functions:
 */

void
reset_state(int free);

void
parse_line(
        const char *line
);

void
iterate(void);

int
run_race(int duration);

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
part_one(
        const char *                       /* a file name */
);


int
part_two(
        const char *                       /* a file name */
);


/* end solution.h */
#endif /* SOLUTION_H */
