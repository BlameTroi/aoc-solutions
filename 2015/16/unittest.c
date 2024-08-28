/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */


#include <string.h>


#include "minunit.h"
#include "solution.h"


/*
 * global/static data and constants.
 */


char *input_lines[] = {
	"Sue 1: goldfish: 9, cars: 0, samoyeds: 9\n",
	"Sue 2: perfumes: 5, trees: 8, goldfish: 8\n",
	"Sue 3: pomeranians: 2, akitas: 1, trees: 5\n",
	"Sue 4: goldfish: 10, akitas: 2, perfumes: 9\n",
	"Sue 5: cars: 5, perfumes: 6, akitas: 9\n",
	"Sue 6: goldfish: 10, cats: 9, cars: 8\n",
	"Sue 7: trees: 2, samoyeds: 7, goldfish: 10\n",
	"Sue 8: cars: 8, perfumes: 6, goldfish: 1\n",
	"Sue 9: cats: 4, pomeranians: 0, trees: 0\n",
	"Sue 10: trees: 2, children: 10, samoyeds: 10\n",
	""
};

char *cond_get_2 = "perfumes: 5, trees: 8, goldfish: 8\n";
char *cond_9 = "cats: 4, trees: 0\n";
char *cond_0 = "goldfish: 11, cars: 0\n";

/*
 * minunit setup and teardown of infrastructure.
 */

void
test_setup(void)
{
	reset_state(0);
	int i = 0;
	while (strlen(input_lines[i]) != 0) {
		parse_line(input_lines[i]);
		i += 1;
	}
}


void
test_teardown(void)
{
	reset_state(1);
}


/*
 * sample test shell.
    "Sue 4: goldfish: 10, akitas: 2, perfumes: 9\n",

 */

MU_TEST(test_created)
{
	mu_assert_int_eq(10, num_aunts);
	mu_assert_int_eq(9, aunts[8].id);
	mu_assert_int_eq(-1, aunts[8].data[FISH_IX]);
	mu_assert_int_eq(4, aunts[8].data[CATS_IX]);
	mu_assert_int_eq(0, aunts[8].data[POME_IX]);
	mu_assert_int_eq(0, aunts[8].data[TREE_IX]);
	mu_assert_int_eq(4, aunts[3].id);
	mu_assert_int_eq(10, aunts[3].data[FISH_IX]);
	mu_assert_int_eq(2, aunts[3].data[AKIT_IX]);
	mu_assert_int_eq(9, aunts[3].data[PERF_IX]);

	facts_t cond = parse_condition(cond_9);
	mu_assert_int_eq(4, cond.data[CATS_IX]);
	mu_assert_int_eq(-1, cond.data[CARS_IX]);
	mu_assert_int_eq(0, cond.data[TREE_IX]);

}

MU_TEST(test_satisfaction)
{
	facts_t cond = parse_condition(cond_get_2);
	mu_assert_int_eq(1, satisfy_count(cond));
	mu_assert_int_eq(1, satisfy_ix(cond, 0));
	mu_assert_int_eq(-1, satisfy_ix(cond, satisfy_ix(cond, 0)+1));
}


/*
 * here we define the whole test suite. sadly there's no runtime
 * introspection. there is probably an opportunity for an elisp helper
 * to create the suite in the editor, but for now it's just a matter
 * of doing it manually.
 */
MU_TEST_SUITE(test_suite)
{

	/* always have a setup and teardown, even if they */
	/* do nothing. */

	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	/* run your tests here */
	MU_RUN_TEST(test_created);
	MU_RUN_TEST(test_satisfaction);

}


/*
 * master control:
 */

int
main(int argc, char *argv[])
{
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
