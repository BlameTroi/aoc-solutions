/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */


#include <string.h>


#include "minunit.h"
#include "solution.h"


/*
 * global/static data and constants.
 */

char *test_input[] = {
	"Comet can fly 14 km/s for 10 seconds, but then must rest for 127 seconds.",
	"Dancer can fly 16 km/s for 11 seconds, but then must rest for 162 seconds.",
	"Clyde can fly 1 km/s for 1 seconds, but then must rest for 1 seconds.",
	""
};


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
 * sample test shell.
 */

MU_TEST(test_parse) {
	int i = 0;
	while (strlen(test_input[i])) {
		parse_line(test_input[i]);
		i += 1;
	}

	mu_assert_int_eq(3, num_racers);
	mu_assert_int_eq(14, racers[0].speed);
	mu_assert_int_eq(10, racers[0].burst);
	mu_assert_int_eq(162, racers[1].rest);
	mu_assert_string_eq("Dancer", racers[1].name);
	mu_assert_string_eq("Clyde", racers[2].name);

}

MU_TEST(test_iterate) {
	int i = 0;
	while (strlen(test_input[i])) {
		parse_line(test_input[i]);
		i += 1;
	}
	mu_assert_int_eq(0, racers[0].distance);
	mu_assert_int_eq(0, racers[1].distance);
	iterate();
	mu_assert_int_eq(14, racers[0].distance);
	mu_assert_int_eq(16, racers[1].distance);
	mu_assert_int_eq(1, racers[2].distance);
	iterate();
	mu_assert_int_eq(28, racers[0].distance);
	mu_assert_int_eq(32, racers[1].distance);
	mu_assert_int_eq(1, racers[2].distance);
	iterate();
	mu_assert_int_eq(42, racers[0].distance);
	mu_assert_int_eq(48, racers[1].distance);
	mu_assert_int_eq(2, racers[2].distance);

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
	MU_RUN_TEST(test_iterate);

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
