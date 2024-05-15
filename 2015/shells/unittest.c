/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */


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
 * sample test shell.
 */

MU_TEST(test_created) {
   int want=4;
   int got=7;
   mu_assert_int_eq(want, got);
   mu_assert(1, "just do something");
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
	MU_RUN_TEST(test_created);

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
