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



MU_TEST(test_problem_data) {

   /* clear any prior work */
   resetData();

   /* 'read' the input distances and build the locations list
      and distances matrix. */

   int numInputs = sizeof(sampleInput) / sizeof(char *);
   for (int i = 0; i < numInputs; i++) {
      addDistance(sampleInput[i]);
   }

   /* these are the expected results */

   mu_assert_int_eq(605, shortestPath());
   mu_assert_int_eq(982, longestPath());

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
