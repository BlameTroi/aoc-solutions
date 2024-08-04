/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"
#include "solution.h"
#include "txbqu.h"
#include "txbmisc.h"

/*
 * if a chip ends a move on a floor with any generators, that chip's
 * matching generator must be on the floor as well.
 *
 * so: hyg hym lig is legal,
 *
 * but: hym lig is not.
 */

MU_TEST(test_test) {
   state start;
   memset(&start, 0, sizeof(start));

   char buffer[256];
   memset(buffer, 0, 256);
   int buflen = 255;

   printf("\n*****\n");
   init_and_load(&start, false);
   /*  1 elv hym lim
       2     hyg
       3     lig
       4 nil */

   trace_line(&start, buffer, buflen);
   printf("\n%s\n", buffer);
   mu_should(is_valid(&start));
   mu_shouldnt(is_goal(&start));

   /* 1 hym 2 hyg 3 lig lim */
   start.microchips[f1] ^= lithium;
   start.microchips[f3] |= lithium;
   trace_line(&start, buffer, buflen);
   printf("\n%s\n", buffer);
   mu_should(is_valid(&start));
   mu_shouldnt(is_goal(&start));

   /* 1 hym 2 hyg lim 3 lig */
   start.microchips[f3] ^= lithium;
   start.microchips[f2] |= lithium;
   trace_line(&start, buffer, buflen);
   printf("\n%s\n", buffer);
   mu_shouldnt(is_valid(&start));
   mu_shouldnt(is_goal(&start));

   for (int i = f1; i <= f3; i++) {
      start.microchips[f4] |= start.microchips[i];
      start.generators[f4] |= start.generators[i];
      start.microchips[i] = 0;
      start.generators[i] = 0;
   }
   start.elevator = f4;
   trace_line(&start, buffer, buflen);
   printf("\n%s\n", buffer);
   mu_should(is_valid(&start));
   mu_should(is_goal(&start));

   printf("*****\n");
   init_and_load(&start, true);
   /*  1 elv prg prm
       2     cog cug rug plg
       3     com cum rum plm
       4 nil */
   trace_line(&start, buffer, buflen);
   printf("\n%s\n", buffer);
   mu_should(is_valid(&start));
   mu_shouldnt(is_goal(&start));

}

MU_TEST(test_next) {
   state start;
   memset(&start, 0, sizeof(start));

   char buffer[256];
   memset(buffer, 0, 256);
   int buflen = 255;

   /* setup for moving a single chip from f1 */
   printf("\n*****\n");
   start.elevator = f1;
   start.microchips[f1] = hydrogen;
   start.microchips[f2] = lithium;
   start.generators[f3] = hydrogen;
   start.generators[f4] = lithium;
   /*  1 elv hym
       2     lim
       3     hyg
       4     lig */
   trace_line(&start, buffer, buflen);
   printf("\n%s\n", buffer);
   mu_should(is_valid(&start));
   mu_shouldnt(is_goal(&start));

   /* get next moves, there should be only one moving hydrogen
    * from f1 to f2. */
   qucb *possibilities = next_moves(&start);
   mu_shouldnt(qu_empty(possibilities));

   state *nm = NULL;
   nm = qu_dequeue(possibilities);
   trace_line(nm, buffer, buflen);
   printf("\n%s\n", buffer);
   mu_should(is_valid(nm));
   mu_shouldnt(is_goal(nm));

   qu_destroy(possibilities);
   free(nm);

   /* now put a single chip on a f2, we should get two moves */
   printf("\n*****\n");
   start.elevator = f2;
   start.microchips[f1] = hydrogen;
   start.microchips[f2] = lithium;
   start.generators[f3] = lithium;
   start.generators[f4] = hydrogen;
   /*  1 elv hym
       2     lim
       3     lig
       4     hyg */
   trace_line(&start, buffer, buflen);
   printf("\n%s\n", buffer);
   mu_should(is_valid(&start));
   mu_shouldnt(is_goal(&start));

   /* get next moves, there should be two of them, moving lithium
    * to first and third floors is legal. */
   possibilities = next_moves(&start);
   mu_shouldnt(qu_empty(possibilities));
   nm = qu_dequeue(possibilities);
   trace_line(nm, buffer, buflen);
   printf("\n%s\n", buffer);
   free(nm);
   nm = qu_dequeue(possibilities);
   trace_line(nm, buffer, buflen);
   printf("\n%s\n", buffer);
   qu_destroy(possibilities);
   free(nm);

   /* now put a single chip on a f2, but it shouldn't be able to
    * move to f1 because of a generator mismatch. */
   printf("\n*****\n");
   start.elevator = f2;
   start.generators[f1] = hydrogen;
   start.microchips[f2] = lithium;
   start.generators[f3] = lithium;
   start.microchips[f4] = hydrogen;
   /*  1 elv hyg
       2     lim
       3     lig
       4     hym */
   trace_line(&start, buffer, buflen);
   printf("\n%s\n", buffer);
   mu_should(is_valid(&start));
   mu_shouldnt(is_goal(&start));

   possibilities = next_moves(&start);
   mu_shouldnt(qu_empty(possibilities));
   nm = qu_dequeue(possibilities);
   trace_line(nm, buffer, buflen);
   printf("\n%s\n", buffer);
   free(nm);
   qu_destroy(possibilities);
}

MU_TEST(test_many_moves) {
   state start;
   memset(&start, 0, sizeof(start));

   char buffer[256];
   memset(buffer, 0, 256);
   int buflen = 255;

   printf("*****\n");
   init_and_load(&start, true);
   /*  1 elv prg prm
       2     cog cug rug plg
       3     com cum rum plm
       4 nil */
   trace_line(&start, buffer, buflen);
   printf("\n%s\n", buffer);
   mu_should(is_valid(&start));
   mu_shouldnt(is_goal(&start));

   state *nm = NULL;
   /* first, let's see if we can get the initial moves done */
   qucb *possibilities = next_moves(&start);
   mu_shouldnt(qu_empty(possibilities));
   /* possible moves are floor 2 gets prg, prg prm, so 2 moves */
   while (!qu_empty(possibilities)) {
      nm = qu_dequeue(possibilities);
      trace_line(nm, buffer, buflen);
      printf("\n%s\n", buffer);
      mu_should(is_valid(nm));
      mu_shouldnt(is_goal(nm));
      free(nm);
   }

   qu_destroy(possibilities);

   /* now a case where we've got multiple chips and generators */
   printf("*****\n");
   init_and_load(&start, true);
   /*  1 elv prg prm
       2     cog cug rug plg
       3     com cum rum plm
       4 nil */
   /* tweak the setup slightly, nothing on first floor,
    * but prg and prm on second floor with what's already there.
    * every other generator should be movable downward, but only one
    * of each can move upward. */
   start.elevator = f2;
   start.generators[f2] |= start.generators[f1];
   start.generators[f1] = 0;
   start.microchips[f2] |= start.microchips[f1];
   start.microchips[f1] = 0;
   trace_line(&start, buffer, buflen);
   printf("\n%s\n", buffer);
   mu_should(is_valid(&start));
   mu_shouldnt(is_goal(&start));
   possibilities = next_moves(&start);
   while (!qu_empty(possibilities)) {
      nm = qu_dequeue(possibilities);
      trace_line(nm, buffer, buflen);
      printf("\n%s\n", buffer);
      mu_should(is_valid(nm));
      mu_shouldnt(is_goal(nm));
      free(nm);
   }
   free(possibilities);
}


void
test_setup(void) {}
void
test_teardown(void) {}

MU_TEST_SUITE(test_suite) {

   MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

   /* run your tests here */
   MU_RUN_TEST(test_test);
   MU_RUN_TEST(test_next);
   MU_RUN_TEST(test_many_moves);
}

int
main(int argc, char *argv[]) {
   MU_RUN_SUITE(test_suite);
   MU_REPORT();
   return MU_EXIT_CODE;
}
