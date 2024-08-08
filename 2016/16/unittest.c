/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"
#include "solution.h"

/*
 * our expansion algorithm:
 *
 * Call the data you have at this point "a".
 *
 * Make a copy of "a"; call this copy "b".
 *
 * Reverse the order of the characters in "b".
 *
 * In "b", replace all instances of 0 with 1 and all 1s with 0.
 *
 * The resulting data is "a", then a single 0, then "b".
 *
 * For example, after a single step of this process:
 *
 * * 1 becomes 100.
 *
 * * 0 becomes 001.
 *
 * * 11111 becomes 11111000000.
 *
 * * 111100001010 becomes 1111000010100101011110000.
 *
 * our checksum algorithm:
 *
 * checksum is one bit out for two bits in. even parity gets a one,
 * odd parity gets a zero, so the parity is odd.
 *
 * 00 or 11 == 2 -> 1
 * 10 or 01 == 1 -> 0
 *
 * if the checksum length is even, take the checksum of the checksum.
 *
 * for a string of twelve bits 110010110100, the checksum would be:
 *
 * 11 00 10 11 01 00 -> 1 1 0 1 0 1
 *
 * 11 01 01 -> 1 0 0
 *
 * so the checksum is 100.
 *
 */

/*
 * minunit setup and teardown of infrastructure.
 */

bool got_args = false;
int num_args = 0;
char **the_args = NULL;

void
test_setup(void) {
}

void
test_teardown(void) {
}

/*
 * the problem is expressed in terms of bits, but we'll do this as
 * character strings.
 */


/* Call the data you have at this point "a".
 *
 * Make a copy of "a"; call this copy "b".
 *
 * Reverse the order of the characters in "b".
 *
 * In "b", replace all instances of 0 with 1 and all 1s with 0.
 *
 * The resulting data is "a", then a single 0, then "b".
 * For example, after a single step of this process:
 *
 * * 1 becomes 100.
 *
 * * 0 becomes 001.
 *
 * * 11111 becomes 11111000000.
 *
 * * 111100001010 becomes 1111000010100101011110000.

 */

char *expand_tests[][2] = {
   { "1", "100" },
   { "0", "001" },
   { "11111", "11111000000" },
   { "111100001010", "1111000010100101011110000" },
   { NULL, NULL }
};

MU_TEST(test_expand) {
   for (int i = 0; expand_tests[i][0]; i++) {
      char *a = expand_tests[i][0];
      char *expected = expand_tests[i][1];
      char *b = dragon_expand(a);
      printf("\n   input: %s\n", a);
      printf("  output: %s\n", b);
      printf("expected: %s\n", expected);
      mu_should(strlen(b) == strlen(a) * 2 + 1);
      mu_should(strcmp(b, expected) == 0);
      free(b);
   }
}

char *checksum_tests[][2] = {
   { "10", "0" },
   { "01", "0" },
   { "00", "1" },
   { "11", "1" },
   { "110010110100", "100" },
   { "0000", "1" },
   { "1111", "1" },
   { "1011", "0" },
   { "1010", "1" },
   { "10000011110010000111", "01100" },
   { NULL, NULL }
};

MU_TEST(test_checksum) {
   for (int i = 0; checksum_tests[i][0]; i++) {
      char *a = checksum_tests[i][0];
      char *expected = checksum_tests[i][1];
      char *b = dragon_checksum(a);
      printf("\n   input: %s\n", a);
      printf("  output: %s\n", b);
      printf("expected: %s\n", expected);
      mu_should(strcmp(b, expected) == 0);
      free(b);
   }
}

MU_TEST(test_sample) {
   char *input = "10000";
   int fill_length = 20;
   char *expanded = malloc(strlen(input) + 1);
   strcpy(expanded, input);
   while (strlen(expanded) < fill_length) {
      char *next = dragon_expand(expanded);
      free(expanded);
      expanded = next;
   }
   expanded[fill_length] = '\0';
   char *checksum = dragon_checksum(expanded);
   mu_should(strcmp(checksum, "01100") == 0);
   free(expanded);
   free(checksum);
}

/*
 * sample test shell.
 */

MU_TEST(test_test) {
   mu_should(true);
   mu_shouldnt(false);
}

/*
 * here we defini the whole test suite.
 */
MU_TEST_SUITE(test_suite) {

   MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

   /* run your tests here */
   MU_RUN_TEST(test_test);
   MU_RUN_TEST(test_expand);
   MU_RUN_TEST(test_checksum);
   MU_RUN_TEST(test_sample);
}

/*
 * master control:
 */

int
main(int argc, char *argv[]) {
   num_args = argc;
   the_args = argv;
   MU_RUN_SUITE(test_suite);
   MU_REPORT();
   return MU_EXIT_CODE;
}
