/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <stdio.h>
#include <string.h>

#include "minunit.h"
#include "solution.h"


/*
 * global/static data and constants.
 */


/*
 * minunit setup and teardown of infrastructure.
 */

void
test_setup(void) {
}

void
test_teardown(void) {
}


/*
 * the rules of a valid password:
 *
 * rule: 8 lowercase letters
 *
 * rule: odometer passwords, a-z are digits 0-25
 *
 * rule: at least one increasing sequential straight 'abc', 'bcd' are
 *       legal, 'abd' is not.
 *
 * rule: no visually confusing letters: 'i', 'o', and 'l' are not
 *       legal.
 *
 * rule: two distinct letter pairs: 'aa', 'bb', but 'aaa' does not
 *       count as two pair, they overlap. 'aaaa' should be ok.
 *
 */

MU_TEST(test_first) {
   mu_assert_int_eq(0, password_p("abcdefgh")); /* no pairs */
}

MU_TEST(test_bad_chars) {
   mu_assert_int_eq(0, password_p("hijklmmn"));
}

MU_TEST(test_legal) {
   mu_assert_int_eq(1, password_p("abcddeef"));
}

MU_TEST(test_overlap_pair) {
   mu_assert_int_eq(0, password_p("abcdddef"));
   mu_assert_int_eq(1, password_p("abcdddde"));
}

MU_TEST(test_no_straight) {
   mu_assert_int_eq(0, password_p("abbceffg"));
}

MU_TEST(test_missing_pair) {
   mu_assert_int_eq(0, password_p("abbcegjk"));
}


MU_TEST(test_bad_lengths) {
   mu_assert_int_eq(0, password_p("abcdefghk"));
   mu_assert_int_eq(0, password_p("hjkaabb"));    /* too short but legal otherwise */
   mu_assert_int_eq(0, password_p("hjkaacbbd"));  /* too long but legal otherwise */
}

MU_TEST(test_two_in_sequence) {
   /* the next password after abcdefgh is abcdffaa */
   mu_assert_int_eq(0, password_p("abcdefgh")); /* actually illegal, but we can increment it */
   mu_assert_int_eq(1, password_p("abcdffaa")); /* next legal in sequence aftere above */
}

MU_TEST(test_two_in_sequnce_again) {
   /* the next password after ghijklmn is ghjaabcc */
   mu_assert_int_eq(0, password_p("ghijklmn"));
   mu_assert_int_eq(1, password_p("ghjaabcc"));
}

MU_TEST(test_santa) {

   /* santa's current is illegal */
   char *p = strdup("hepxcrrq");
   mu_assert_int_eq(0, password_p(p));

   /* cycle until we get a legal new password */
   int n = 0;
   while (n < 5000000 && !password_p(password_increment(p))) {
      if (n % 100000 == 0) {
         printf("%6d %s\n", n, p);
      }
      n += 1;
   }
   mu_assert_int_eq(1, password_p(p));
   printf("\npart one after %6d tries gets %s\n", n, p);

   /* cycle until we get a legal new password */
   n = 0;
   while (n < 5000000 && !password_p(password_increment(p))) {
      if (n % 100000 == 0) {
         printf("%6d %s\n", n, p);
      }
      n += 1;
   }
   mu_assert_int_eq(1, password_p(p));
   printf("\npart two after another %6d tries gets %s\n", n, p);

}


MU_TEST(test_increment) {
   mu_assert_string_eq("abcdefgi", password_increment(strdup("abcdefgh")));
   mu_assert_string_eq("abcdefyz", password_increment(strdup("abcdefyy")));
   mu_assert_string_eq("abcdefza", password_increment(strdup("abcdefyz")));
   mu_assert_string_eq("abcdegaa", password_increment(strdup("abcdefzz")));
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

   MU_RUN_TEST(test_first);
   MU_RUN_TEST(test_bad_chars);
   MU_RUN_TEST(test_legal);
   MU_RUN_TEST(test_overlap_pair);
   MU_RUN_TEST(test_no_straight);
   MU_RUN_TEST(test_missing_pair);
   MU_RUN_TEST(test_bad_lengths);
   MU_RUN_TEST(test_two_in_sequence);
   MU_RUN_TEST(test_two_in_sequnce_again);
   MU_RUN_TEST(test_increment);
   MU_RUN_TEST(test_santa);


}


int
main(int argc, char *argv[]) {
   MU_RUN_SUITE(test_suite);
   MU_REPORT();
   return MU_EXIT_CODE;
}
