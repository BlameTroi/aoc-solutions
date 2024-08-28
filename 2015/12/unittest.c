/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <stdlib.h>
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
test_setup(void)
{
}

void
test_teardown(void)
{
}


/*

sum all the numbers in a json file without regard to file structure.
no numbers will be embedded in strings.

examples:

[1,2,3] and {"a":2,"b":4} both have a sum of 6.

[3] and {"a":{"b":4},"c":-1} both have a sum of 3.

{"a":[-1,1]} and [-1,{"a":1}] both have a sum of 0.

[] and {} both have a sum of 0.

problem dataset is one big long unformatted blob of json looking
text.

for part two:

Ignore any object (and all of its children) which has any property
with the value "red". Do this only for objects ({...}), not arrays
([...]).

[1,2,3] still has a sum of 6.

[1,{"c":"red","b":2},3] now has a sum of 4, because the middle object is ignored.

{"d":"red","e":[1,2,3,4],"f":5} now has a sum of 0, because the entire structure is ignored.

[1,"red",5] has a sum of 6, because "red" in an array has no effect.

*/


MU_TEST(test_single_object)
{
	char *s;

	/* simple object: no red attribute */
	s = "{\"a\":1,\"b\":2,\"c\":\"blue\",\"d\":3}";
	s = strdup(s);
	mu_assert_int_eq(0, scrubObjects(s, "red"));
	mu_assert_int_eq(6, sumNumbers(s, NULL));
	free(s);

	/* simple object: red attribute */
	s = "{\"a\":1,\"b\":2,\"c\":\"red\",\"d\":3}";
	s = strdup(s);
	mu_assert_int_eq(1, scrubObjects(s, "red"));
	mu_assert_int_eq(0, sumNumbers(s, NULL));
	free(s);
}

MU_TEST(test_array_object_red_blue)
{
	char *s = "[1,{\"c\":\"red\",\"b\":2},3]";
	s = strdup(s);
	mu_assert_int_eq(1, scrubObjects(s, "red"));
	mu_assert_int_eq(4, sumNumbers(s, NULL));
	free(s);

	s = "[1,{\"c\":\"blue\",\"b\":2},3]";
	s = strdup(s);
	mu_assert_int_eq(0, scrubObjects(s, "red"));
	mu_assert_int_eq(6, sumNumbers(s, NULL));
	free(s);
}


MU_TEST(test_array_red_string)
{
	char *s = "[1,\"red\",5]";
	s = strdup(s);
	mu_assert_int_eq(0, scrubObjects(s, "red"));
	mu_assert_int_eq(6, sumNumbers(s, NULL)); /* freestanding red in object doesn't matter */
}



MU_TEST_SUITE(test_suite_structured)
{
	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	MU_RUN_TEST(test_single_object);
	MU_RUN_TEST(test_array_object_red_blue);
	MU_RUN_TEST(test_array_red_string);

}

/* these are the unstructured stream tests */
#define FULL 1

MU_TEST(test_1)
{
	char *s = "1";
	mu_assert_int_eq(1, sumNumbers(s, NULL));
}

MU_TEST(test_2)
{
	char *s = "2";
	mu_assert_int_eq(2, sumNumbers(s, NULL));
}
MU_TEST(test_empty)
{
	char *s = "";
	mu_assert_int_eq(0, sumNumbers(s, NULL));
}

MU_TEST(test_no_digits)
{
	char *s = "[]{}";
	mu_assert_int_eq(0, sumNumbers(s, NULL));
}

MU_TEST(test_single_digit)
{
	char *s = "9";
	mu_assert_int_eq(9, sumNumbers(s, NULL));
}

MU_TEST(test_embedded)
{
	char *s = "[1,2,3]";
	mu_assert_int_eq(6, sumNumbers(s, NULL));
	s = "{\"a\":2,\"b\":4}";
	mu_assert_int_eq(6, sumNumbers(s, NULL));
}

MU_TEST(test_negative)
{
	char *s = "[1,-1]";
	mu_assert_int_eq(0, sumNumbers(s, NULL));
}

MU_TEST(test_two_negative)
{
	char *s = "-2,-3";
	mu_assert_int_eq(-5, sumNumbers(s, NULL));
}


MU_TEST(test_substring)
{
	char *s = "b=7+3*(-4);";
	char *t = s;
	while (*t != '(')
		t += 1;
	mu_assert_int_eq(10, sumNumbers(s, t));
	mu_assert_int_eq(6, sumNumbers(s, NULL));
}


MU_TEST_SUITE(test_suite_unstructured)
{

	/* always have a setup and teardown, even if they */
	/* do nothing. */

	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	/* is it plugged in? */

	MU_RUN_TEST(test_1);
	MU_RUN_TEST(test_2);
	MU_RUN_TEST(test_empty);
	MU_RUN_TEST(test_no_digits);
	MU_RUN_TEST(test_single_digit);
	MU_RUN_TEST(test_embedded);
	MU_RUN_TEST(test_negative);
	MU_RUN_TEST(test_two_negative);
	MU_RUN_TEST(test_substring);

}


int
main(int argc, char *argv[])
{
	if (FULL)
		MU_RUN_SUITE(test_suite_unstructured);
	;
	MU_RUN_SUITE(test_suite_structured);
	MU_REPORT();
	return MU_EXIT_CODE;
}
