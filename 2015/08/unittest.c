/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include "minunit.h"
#include "solution.h"
#include <stdlib.h>
#include <string.h>


/*
 * global/static data and constants.
 */

/* none for this problem */


/*
 * minunit setup and teardown of infrastructure.
 */

/* empty for this problem */

void
test_setup(void) {
}

void
test_teardown(void) {
}


/* helper to wrap an input string in double quotes for the test harness.*/

char *
wrap_string(const char *inp) {
	char *oup = calloc(strlen(inp) + 2 + 1, 1);
	char *tmp = oup;
	*tmp = '"';
	tmp += 1;
	while (*inp)
		*tmp++ = *inp++;
	*tmp++ = '"';
	*tmp='\0';
	return oup;
}


/* tests from the problem statement */


MU_TEST(test_empty) {
	char* cc_empty = "";
	char *c = wrap_string(cc_empty);
	char *d = encoded_string(c);
	/* printf("\nprocessing '%s' => '%s' => '%s'\n", cc_empty, c, d); */
	mu_assert_int_eq(2, source_string_length(c));
	mu_assert_int_eq(strlen(cc_empty), compiled_string_length(c));
	mu_assert_int_eq(6, encoded_string_length(d));
	free(c);
	free(d);
}


MU_TEST(test_no_escape) {
	char* cc_no_escape = "abc";
	char *c = wrap_string(cc_no_escape);
	char *d = encoded_string(c);
	/* printf("\nprocessing '%s' => '%s' => '%s'\n", cc_no_escape, c, d); */
	mu_assert_int_eq(5, source_string_length(c));
	mu_assert_int_eq(3, compiled_string_length(c));
	mu_assert_int_eq(9, encoded_string_length(d));
	free(c);
	free(d);
}


MU_TEST(test_embedded_quote) {
	char* cc_embedded_quote = "aaa\\\"aaa";
	char *c = wrap_string(cc_embedded_quote);
	char *d = encoded_string(c);
	/* printf("\nprocessing '%s' => '%s' => '%s'\n", cc_embedded_quote, c, d); */
	mu_assert_int_eq(10, source_string_length(c));
	mu_assert_int_eq(7, compiled_string_length(c));
	mu_assert_int_eq(16, encoded_string_length(d));
	free(c);
	free(d);
}


MU_TEST(test_hex_escape) {
	char* cc_hex_escape = "\\x27";
	char *c = wrap_string(cc_hex_escape);
	char *d = encoded_string(c);
	/* printf("\nprocessing '%s' => '%s' => '%s'\n", cc_hex_escape, c, d); */
	mu_assert_int_eq(6, source_string_length(c));
	mu_assert_int_eq(1, compiled_string_length(c));
	mu_assert_int_eq(11, encoded_string_length(d));
	free(c);
	free(d);
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
