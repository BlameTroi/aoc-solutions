/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"
#include "solution.h"

/*
 * if a chip ends a move on a floor with any generators, that chip's
 * matching generator must be on the floor as well.
 *
 * so: hyg hym lig is legal,
 *
 * but: hym lig is not.
 */

MU_TEST(test_test) {
   move start;
   memset(&start, 0, sizeof(start));

   printf("\n*****\n");
   init_and_load(&start, false);
   /*  1 elv hym lim
       2     hyg
       3     lig
       4 nil */

   report(&start);
   mu_should(is_valid(&start));
   mu_shouldnt(is_goal(&start));

   /* 1 hym 2 hyg 3 lig lim */
   start.parts[f1] ^= to_microchip(lithium);
   start.parts[f3] |= to_microchip(lithium);
   report(&start);
   mu_should(is_valid(&start));
   mu_shouldnt(is_goal(&start));

   /* 1 hym 2 hyg lim 3 lig */
   start.parts[f3] ^= to_microchip(lithium);
   start.parts[f2] |= to_microchip(lithium);
   report(&start);
   mu_shouldnt(is_valid(&start));
   mu_shouldnt(is_goal(&start));

   for (int i = f1; i <= f3; i++) {
      start.parts[f4] |= start.parts[i] & all_as_microchips;
      start.parts[f4] |= start.parts[i] & all_as_generators;
      start.parts[i] = 0;
   }
   start.elevator = f4;
   report(&start);
   mu_should(is_valid(&start));
   mu_should(is_goal(&start));

   printf("*****\n");
   init_and_load(&start, true);
   /*  1 elv prg prm
       2     cog cug rug plg
       3     com cum rum plm
       4 nil */

   report(&start);
   mu_should(is_valid(&start));
   mu_should(true);
   mu_shouldnt(false);
}

void
test_setup(void) {}
void
test_teardown(void) {}

MU_TEST_SUITE(test_suite) {

   MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

   /* run your tests here */
   MU_RUN_TEST(test_test);
}

int
main(int argc, char *argv[]) {
   MU_RUN_SUITE(test_suite);
   MU_REPORT();
   return MU_EXIT_CODE;
}
