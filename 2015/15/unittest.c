/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */


#include <string.h>


#include "minunit.h"
#include "solution.h"


/*
 * global/static data and constants.
 */

char *test_input[] = {
	"Butterscotch: capacity -1, durability -2, flavor 6, texture 3, calories 8",
	"Cinnamon: capacity 2, durability 3, flavor -2, texture -1, calories 3",
	""
};


/*
 * minunit setup and teardown of infrastructure.
 */

void
test_setup(void) {
	reset_state(0);
	char **iline = test_input;
	while (strlen(*iline)) {
		parse_line(*iline);
		iline += 1;
	}

}

void
test_teardown(void) {
	reset_state(1);
}


/*
 * sample test shell.
 */

MU_TEST(test_parse) {
	mu_assert_int_eq(2, num_ingredients);
	mu_assert_string_eq("Butterscotch", ingredients[0].name);
	mu_assert_string_eq("Cinnamon", ingredients[1].name);
	mu_assert_int_eq(-1, ingredients[0].attributes[CAP]);
	mu_assert_int_eq(-2, ingredients[0].attributes[DUR]);
	mu_assert_int_eq(6, ingredients[0].attributes[FLA]);
	mu_assert_int_eq(3, ingredients[0].attributes[TEX]);
	mu_assert_int_eq(8, ingredients[0].attributes[CAL]);
}

MU_TEST(test_mixes) {
	/* score provided in problem statement */
	int portions[] = {44, 56};
	mu_assert_int_eq(62842880, score(portions));
	mu_assert_int_eq(520, calories(portions));

	/* a different mix */
	portions[0] = 56;
	portions[1] = 44;
	mu_assert_int_eq(19681280, score(portions));
	mu_assert_int_eq(580, calories(portions));

	/* the extremes */
	portions[0] = 0;
	portions[1] = 100;
	mu_assert_int_eq(0, score(portions));
	portions[0] = 100;
	portions[1] = 0;
	mu_assert_int_eq(0, score(portions));

	/* mixes that should return 0 because an attribute sums to 0 */

	/* capacity should net to 0 */
	portions[0] = 2;
	portions[1] = 1;
	mu_assert_int_eq(0, score(portions));

	/* durability should net to 0 */
	portions[0] = 3;
	portions[1] = 2;
	mu_assert_int_eq(0, score(portions));

	/* flavor should net to 0 */
	portions[0] = 1;
	portions[1] = 3;
	mu_assert_int_eq(0, score(portions));

	/* texture should net to 0 */
	portions[0] = 1;
	portions[1] = 3;
	mu_assert_int_eq(0, score(portions));

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
	MU_RUN_TEST(test_mixes);

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
