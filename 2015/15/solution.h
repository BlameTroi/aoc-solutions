/* solution.h -- aoc 2015 15 -- troy brumley */
#ifndef SOLUTION_H
#define SOLUTION_H

/*
 * churn through ingredients and find the best recipe. *
 */


/*
 * necessary constants and definitions:
 */

/* how long may a line in the test dataset be? */
#define INPUT_LEN_MAX 255

/* array sizes, 2 in test, 4 in 'big' */
#define INGREDIENTS_MAX 4

/* how many units of ingredients are allowed total? */
#define UNITS_MAX 100


/*
 * typedefs:
 */

/* where to put the various ingredient attributes */

#define CAP 0
#define DUR 1
#define FLA 2
#define TEX 3
#define CAL 4

#define ATTRIBUTE_MAX 5

/* only the first few are used in the score calculation */

#define SCOREABLE_MAX 4

typedef struct ingredient_t {
   char *name;
   int attributes[ATTRIBUTE_MAX];
} ingredient_t;


/*
 * data structures:
 */


ingredient_t ingredients[INGREDIENTS_MAX];
int num_ingredients;
long max_score;


/*
 * functions:
 */

void
reset_state(
   int rel
);

void
parse_line(
   const char *iline
);

long
score(
   int *unit         /* allocated portions for each ingredient */
);

long
calories(
   int *unit
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
partOne(
   const char *                       /* a file name */
);


int
partTwo(
   const char *                       /* a file name */
);


/* end solution.h */
#endif /* SOLUTION_H */
