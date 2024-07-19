/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */


#include <stdlib.h>
#include <string.h>

#include "minunit.h"

#include "solution.h"

/*
 * notes:
 *
 * a hit is opp hp - max(1, agg dmg - opp armor)
 * player is first aggressor
 *
 */


FILE *datafile = NULL;

/*
 * minunit setup and teardown of infrastructure.
 */

void
test_setup(void) {
   reset_all();
   datafile = fopen("unitdata.txt", "r");
   if (!datafile) {
      mu_fail("could not open test data file!");
   }
}

void
test_teardown(void) {
   fclose(datafile);
}


/*
 * sample test shell.
 */

MU_TEST(test_read) {

   mob_t *boss = read_mob("Boss", datafile);
   mu_should(boss);
   mu_should(boss->hp == 12);
   mu_should(boss->damage == 7);
   mu_should(boss->armor == 2);

   mob_t *player = read_mob("Player", datafile);
   mu_should(player);
   mu_should(player->hp == 8);
   mu_should(player->damage == 5);
   mu_should(player->armor == 2);

   free(boss);
   free(player);
}

/*
 * the sample battle given in the original problem.
 */

MU_TEST(test_battle_data) {
   /* dummy up data for the battle */
   gear_t bogus_weapon = {
      "bogus",
      "bogus",
      0,
      0,
      0,
   };
   set_t bogus_set = {
      0,
      0,
      0,
      0,
      0,
      &bogus_weapon,
      NULL,
      NULL,
      NULL,
   };
   mob_t boss = {
      "Boss",
      12,
      7,
      2,
      0,
      NULL,
   };
   mob_t player = {
      "Player",
      8,
      5,
      5,
      0,
      &bogus_set,
   };

   mob_t *victor = battle(&player, &boss, true);
   mu_should(victor == &player);
   mu_should(victor->curr_hp == 2);
   mu_should(boss.curr_hp == 0);
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

   /* run your tests here */
   MU_RUN_TEST(test_read);
   MU_RUN_TEST(test_battle_data);

}


/*
 * master control:
 */

int
main(int argc, char *argv[]) {
   MU_RUN_SUITE(test_suite);
   MU_REPORT();
   return MU_EXIT_CODE;
}
