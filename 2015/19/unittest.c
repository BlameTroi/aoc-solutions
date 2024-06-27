/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <stdlib.h>
#include <string.h>

#include "minunit.h"
#include "txbstr.h"
#include "solution.h"


/*
 * global/static data and constants.
 */

char *test_input[] = {
   "H => HO",
   "H => OH",
   "O => HH",
   "",
   "HOH",
   NULL
};

/*
 * the above is epected to produce the following:
 *
 *  HOOH (via H => HO on the first H).
 *  HOHO (via H => HO on the second H).
 *  OHOH (via H => OH on the first H).
 *  HOOH (via H => OH on the second H).
 *  HHHH (via O => HH).
 *
 * which are four, not five, distinct outputs.
 */

/*
 * minunit setup and teardown of infrastructure.
 */

void
test_setup(void) {
   reset_state(0);
}

void
test_teardown(void) {
   reset_state(1);
}

/*
 * loader
 */

void
load_input(void) {
   int i = 0;
   while (test_input[i]) {
      if (strlen(test_input[i]) == 0) {
         i += 1;
         continue;
      }
      const char **t = split_string(test_input[i], " \n");
      if (t[2] && strcmp(t[2], "=>") == 0) {
         parse_line(test_input[i]);
      } else {
         strncpy(base, test_input[i], sizeof(base));
      }
      free((void *)t[0]);
      free(t);
      i += 1;
   }
}

/*
 * sample test shell.
 */

MU_TEST(test_parse) {
   load_input();
   mu_assert_int_eq(3, num_transforms);
   mu_assert_string_eq("HOH", base);
}

MU_TEST(test_transforms) {
   load_input();

   mu_assert_string_eq("H", transforms[0]->from);
   mu_assert_string_eq("HO", transforms[0]->to);
   mu_assert_string_eq("H", transforms[1]->from);
   mu_assert_string_eq("OH", transforms[1]->to);
   mu_assert_string_eq("O", transforms[2]->from);
   mu_assert_string_eq("HH", transforms[2]->to);

   int pos = 0;
   char *temp = NULL;

   pos = 0;
   temp = transformer(transforms[0], base, &pos);
   mu_assert_string_eq("HOOH", temp);
   mu_assert_int_eq(1, pos);
   free(temp);
   temp = transformer(transforms[0], base, &pos);
   mu_assert_string_eq("HOHO", temp);
   free(temp);
   temp = transformer(transforms[0], base, &pos);
   mu_assert_int_eq(0, (long)temp);

   pos = 0;
   temp = transformer(transforms[1], base, &pos);
   mu_assert_string_eq("OHOH", temp);
   free(temp);
   temp =  transformer(transforms[1], base, &pos);
   mu_assert_string_eq("HOOH", temp);
   free(temp);

   pos = 0;
   temp = transformer(transforms[2], base, &pos);
   mu_assert_string_eq("HHHH", temp);
   free(temp);
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
   MU_RUN_TEST(test_parse);
   MU_RUN_TEST(test_transforms);


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
