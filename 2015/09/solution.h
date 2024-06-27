/* solution.h -- aoc 2015 09 -- troy brumley */
#ifndef SOLUTION_H
#define SOLUTION_H

/*
 * describe the day's problem
 *
 */


/*
 * necessary constants and definitions:
 */

/* how long may a line in the test dataset be? */

#define INPUT_LEN_MAX 255

/* parse helpers and limits */

#define LOCATION_LEN_MAX 32
#define LOCATION_SEP " to "
#define DISTANCE_SEP " = "

#define LOCATIONS_MAX 15


/*
 * global data structures:
 *
 * in a real application i'd hide these, but it's not worth it at this
 * point.
 */

/* distances between two locations can be found by indexing into this
   table. */

int distances[LOCATIONS_MAX][LOCATIONS_MAX];

/* locations are identified by their index into this list. as the
   list is short and lookups are isolated to loading, a linear
   search is sufficient. */

char *locations[LOCATIONS_MAX];

/* we found this many locations during the load process. */

int numLocations;


/*
 * functions:
 */

/* zero out our tables */

void
resetData(void);


/* get the shortest and longest path lengths */

int
shortestPath(void);

int
longestPath(void);


/* parse the distance between two locations from an input record.
   format is 'loc1 to loc2 = dst'. */

void
addDistance(
   const char *s
);


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
