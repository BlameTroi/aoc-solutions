/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include "minunit.h"
#include "solution.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

- "" is 2 characters of code (the two double quotes), but the string
  contains zero characters.
- "abc" is 5 characters of code, but 3 characters in the string data.
- "aaa\"aaa" is 10 characters of code, but the string itself contains
  six "a" characters and a single, escaped quote character, for a
  total of 7 characters in the string data.
- "\x27" is 6 characters of code, but the string itself contains just
  one - an apostrophe ('), escaped using hexadecimal notation.

For example, given the four strings above, the total number of
 characters of string code (2 + 5 + 10 + 6 = 23) minus the total number
of characters in memory for string values (0 + 3 + 7 + 1 = 11) is 23 -
11 = 12.

*/



/* wrap an input string in double quotes for the test harness.*/

char*
wrapString(const char* inp) {
   char *oup = calloc(strlen(inp) + 2 + 1, 1);
   char *tmp = oup;
   *tmp = '"';
   tmp += 1;
   while(*inp) {
      *tmp++ = *inp++;
   }
   *tmp++ = '"';
   *tmp='\0';
   return oup;
}
/* into c format that should compile to the representation above */

const char* cc_empty = "";
const char* cc_no_escape = "abc";
const char* cc_embedded_quote = "aaa\\\"aaa";
const char* cc_hex_escape = "\\x27";

MU_TEST(test_empty) {
   char *c = wrapString(cc_empty);
   printf("\nprocessing '%s' => '%s'\n", cc_empty, c);
   mu_assert_int_eq(2, sourceStringLength(c));
   mu_assert_int_eq(strlen(cc_empty), compiledStringLength(c));
   free(c);
}

MU_TEST(test_no_escape) {
   char *c = wrapString(cc_no_escape);
   printf("\nprocessing '%s' => '%s'\n", cc_no_escape, c);
   mu_assert_int_eq(5, sourceStringLength(c));
   mu_assert_int_eq(3, compiledStringLength(c));
   free(c);
}

MU_TEST(test_embedded_quote) {
   char *c = wrapString(cc_embedded_quote);
   printf("\nprocessing '%s' => '%s'\n", cc_embedded_quote, c);
   mu_assert_int_eq(10, sourceStringLength(c));
   mu_assert_int_eq(7, compiledStringLength(c));
   free(c);
}

MU_TEST(test_hex_escape) {
   char *c = wrapString(cc_hex_escape);
   printf("\nprocessing '%s' => '%s'\n", cc_hex_escape, c);
   mu_assert_int_eq(6, sourceStringLength(c));
   mu_assert_int_eq(1, compiledStringLength(c));
   free(c);
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

   /* and more tests as needed */
   MU_RUN_TEST(test_empty);
   MU_RUN_TEST(test_no_escape);
   MU_RUN_TEST(test_embedded_quote);
   MU_RUN_TEST(test_hex_escape);

}


int
main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
