/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

/* this ended up being not realy needed except barely for the parse and
   load differences. this is a rehash of an early tsp puzzle. */

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
	reset_state(0);
}

void
test_teardown(void) {
	reset_state(1);
}


/*
 * sample test shell.
 */

MU_TEST(test_created) {
	node_count = 2;
	node_names[0] = strdup("fred");
	node_names[1] = strdup("barney");
	node_names[2] = strdup("dino");
	node_delta[0][0] = 1;
	node_delta[0][1] = 2;
	node_delta[0][2] = 3;
	node_delta[1][0] = 2;
	node_delta[1][1] = 4;
	node_delta[1][2] = 7;
	int want=4;
	int got=4;
	mu_assert_int_eq(want, got);
}

MU_TEST(test_load_parse) {
	char *test_data[] = {
		"Alice would gain 54 happiness units by sitting next to Bob.",
		"Alice would lose 79 happiness units by sitting next to Carol.",
		"Alice would lose 2 happiness units by sitting next to David.",
		"Bob would gain 83 happiness units by sitting next to Alice.",
		"Bob would lose 7 happiness units by sitting next to Carol.",
		"Bob would lose 63 happiness units by sitting next to David.",
		"Carol would lose 62 happiness units by sitting next to Alice.",
		"Carol would gain 60 happiness units by sitting next to Bob.",
		"Carol would gain 55 happiness units by sitting next to David.",
		"David would gain 46 happiness units by sitting next to Alice.",
		"David would lose 7 happiness units by sitting next to Bob.",
		"David would gain 41 happiness units by sitting next to Carol."
	};
	const int line_count = sizeof(test_data) / sizeof(char *);
	const int name_count = 4;

	char *name_one = "Carol";
	char *name_two = "Alice";
	char *name_three = "Bob";

	/* we know already that setup has cleared data stores, so */
	for (int i = 0; i < line_count; i++)
		parse_line(test_data[i]);

	/* that should have created four nodes */
	mu_assert_int_eq(name_count, node_count);

	/* let's check the loaded data */
	int idx_one = node_index(name_one); /* for side effect of getting index */
	int idx_two = node_index(name_two);
	int idx_three = node_index(name_three);

	/* confirm that the nodes were already there, no new nodes created */
	mu_assert_int_eq(name_count, node_count);

	/* happiness changes based on direction */
	mu_assert_int_eq(-62, node_delta[idx_one][idx_two]);
	mu_assert_int_eq(-79, node_delta[idx_two][idx_one]);
	mu_assert_int_eq(60, node_delta[idx_one][idx_three]);
	mu_assert_int_eq(54, node_delta[idx_two][idx_three]);
	mu_assert_int_eq(-7, node_delta[idx_three][idx_one]);
	mu_assert_int_eq(83, node_delta[idx_three][idx_two]);

	/* indifference to themselves */
	mu_assert_int_eq(0, node_delta[idx_one][idx_one]);
	mu_assert_int_eq(0, node_delta[idx_two][idx_two]);
	mu_assert_int_eq(0, node_delta[idx_three][idx_three]);
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
	MU_RUN_TEST(test_load_parse);

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
