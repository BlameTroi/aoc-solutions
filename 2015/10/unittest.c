/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <stdlib.h>

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



/* the tests given in the problem statement for part one */

MU_TEST(test_1) {
	char *said = seesay("1");
	mu_assert_string_eq("11", said);
	free(said);
}

MU_TEST(test_11) {
	char *said = seesay("11");
	mu_assert_string_eq("21", said);
	free(said);
}

MU_TEST(test_21) {
	char *said = seesay("21");
	mu_assert_string_eq("1211", said);
	free(said);
}

MU_TEST(test_1211) {
	char *said = seesay("1211");
	mu_assert_string_eq("111221", said);
	free(said);
}

MU_TEST(test_chunky) {
	/* a bit white box here, the output buffer is allocated in blocks
	   of 16 bytes, make sure build the result correctly across that
	   boundary. */
	char *said = seesay("12345678901234567890");
	mu_assert_string_eq("1112131415161718191011121314151617181910", said);
	free(said);
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

	MU_RUN_TEST(test_1);
	MU_RUN_TEST(test_11);
	MU_RUN_TEST(test_21);
	MU_RUN_TEST(test_1211);

	MU_RUN_TEST(test_chunky);

	/* and more tests as needed */

}


int
main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
