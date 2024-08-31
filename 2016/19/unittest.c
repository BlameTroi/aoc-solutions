/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <stdio.h>

#include "minunit.h"
#include "solution.h"

/*
 * minunit setup and teardown of infrastructure.
 */

int num_args = 0;
char **the_args = NULL;

void
test_setup(void) {
}

void
test_teardown(void) {
}

/*
 * sample test shell.
 */

MU_TEST(test_test) {
	mu_should(josephus_adjacent(5) == 3);
	printf("\n%d\n", josephus_adjacent(3018458));
	printf("\n%d\n", josephus_opposite(5));
	printf("\n%d\n", josephus_opposite(3018458));
}

/*
 * here we defini the whole test suite.
 */
MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	/* run your tests here */
	MU_RUN_TEST(test_test);
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
