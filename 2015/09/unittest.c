/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "minunit.h"

#include "txbmisc.h"
#include "txbstr.h"
#include "txbpmute.h"

#include "solution.h"


/*
 * minunit setup and teardown of infrastructure.
 */


void
test_setup(void) {
   /* does nothing at the moment. */
}

void
test_teardown(void) {
   /* does nothing at the moment. */
}


/*
 * tests here ...
 */

/* global/static data and constants */

#define LOCATIONS_MAX 10

int distances[LOCATIONS_MAX][LOCATIONS_MAX];
char* locations[LOCATIONS_MAX];
int numLocations = 0;

char *sampleInput[] = {
   "London to Dublin = 464\n",
   "London to Belfast = 518\n",
   "Dublin to Belfast = 141\n",
};

char *expectedLocations[] = {
   "London",
   "Dublin",
   "Belfast"
};


/*
 * map the location name to an index. as the
 * dataset is very small, there's no need to
 * sort or optimize.
 */

int
indexOfLocation(char *s) {
   int i;
   /* if it's already here, return it */
   for (i = 0; i < numLocations; i++) {
      if (strcmp(locations[i], s) == 0) {
         return i;
      }
   }
   assert(numLocations <= LOCATIONS_MAX);
   locations[numLocations] = strdup(s);
   numLocations += 1;
   return numLocations-1;
}


MU_TEST(test_problem_data) {

   int numInputs = sizeof(sampleInput) / sizeof(char *);

   memset(distances, 0, sizeof(distances));
   memset(locations, 0, sizeof(locations));
   numLocations = 0;

   /* loc1 to loc2 = dst */
   for (int i = 0; i < numInputs; i++) {
      /* first city in 1, second in 3, distance in 5 */
      char **tokens = splitString(sampleInput[i], " \n");
      /* find in locations table -or- add new entry */
      int lx1 = indexOfLocation(tokens[1]);
      int lx2 = indexOfLocation(tokens[3]);
      /* if no distance logged yet, add it */
      if (distances[lx1][lx2] == 0) {
         distances[lx1][lx2] = strtol(tokens[5], NULL, 10);
         distances[lx2][lx1] = distances[lx1][lx2];
      }
      /* memory hygine */
      free(tokens[0]);
      free(tokens);
   }
   /* temp print */
   for (int i = 0; i < numLocations; i++) {
      printf("\n%3d %15s: ", i, locations[i]);
      for (int j = 0; j < numLocations; j++) {
         printf("%6d ", distances[i][j]);
      }
   }
   printf("\n");

   mu_assert_int_eq(3, 3);
}


/*
 * here we define the whole test suite. sadly there's no runtime
 * introspection. there is probably an opportunity for an elisp helper
 * to create the suite in the editor, but for now it's just a matter
 * of doing it manually.
 */

MU_TEST_SUITE(test_suite) {

   /* always have a setup and teardown, even if they */
   /* do nothing. */

   MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

   /* is it plugged in? */
   /*	MU_RUN_TEST(test_created); */
   MU_RUN_TEST(test_problem_data);

}


int
main(int argc, char *argv[]) {
   MU_RUN_SUITE(test_suite);
   MU_REPORT();
   return MU_EXIT_CODE;
}
