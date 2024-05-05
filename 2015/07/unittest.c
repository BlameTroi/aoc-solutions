/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include "minunit.h"
#include "solution.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>


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
 * may as well verify creation separately.
 */

MU_TEST(test_created) {
   mu_assert(1, "just do something");
}

MU_TEST(test_initialized) {
   int want=4;
   int got=4;
   mu_assert_int_eq(want, got);
}


/*
 * sample input from problem text:
 *
 * while given in this order, the order really doesn't matter.
 */

/* as individual lines */
const char* cin1 = "123 -> xa\n";             /* raw signals */
const char* cin2 = "456 -> y\n";
const char* cin3 = "xa AND y -> da\n";        /* and/or gates */
const char* cin4 = "xa OR y -> e\n";
const char* cin5 = "xa LSHIFT 2 -> f\n";      /* bit shifters */
const char* cin6 = "y RSHIFT 2 -> g\n";
const char* cin7 = "NOT xa -> h\n";           /* negation */
const char* cin8 = "NOT y -> i\n";
const char* cin9 = "xa -> zz\n";              /* wire to wire */

/*
 * test individual forms
 */

MU_TEST(test_parse_signal) {
   parsed_t pb = parse(cin1, strlen(cin1));
   mu_assert_string_eq("xa", pb.wire);
   mu_assert_int_eq(se_value, pb.source_type);
   mu_assert_int_eq(123, pb.inp_value);
}

MU_TEST(test_parse_wire) {
   parsed_t pb = parse(cin9, strlen(cin9));
   mu_assert_string_eq("zz", pb.wire);
   mu_assert_int_eq(se_wire, pb.source_type);
   mu_assert_string_eq("xa", pb.inp_wire);
}

MU_TEST(test_parse_and) {
   parsed_t pb = parse(cin3, strlen(cin3));
   mu_assert_string_eq("da", pb.wire);
   mu_assert_int_eq(se_gate, pb.source_type);
   mu_assert_int_eq(ge_and, pb.inp_gate.type);
   mu_assert_string_eq("xa", pb.inp_gate.wire1);
   mu_assert_string_eq("y", pb.inp_gate.wire2);
}

MU_TEST(test_parse_shift) {
   parsed_t pb = parse(cin5, strlen(cin5));
   mu_assert_string_eq("f", pb.wire);
   mu_assert_int_eq(se_gate, pb.source_type);
   mu_assert_int_eq(ge_lshift, pb.inp_gate.type);
   mu_assert_string_eq("xa", pb.inp_gate.wire1);
   mu_assert_int_eq(2, pb.inp_gate.bit_shift);
}

MU_TEST(test_parse_not) {
   parsed_t pb = parse(cin8, strlen(cin8));
   mu_assert_string_eq("i", pb.wire);
   mu_assert_int_eq(se_gate, pb.source_type);
   mu_assert_int_eq(ge_not, pb.inp_gate.type);
   mu_assert_string_eq("y", pb.inp_gate.wire1);
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

   /* is it plugged in? */
	MU_RUN_TEST(test_created);
	MU_RUN_TEST(test_initialized);

   /* and more tests as needed */
   MU_RUN_TEST(test_parse_signal);
   MU_RUN_TEST(test_parse_wire);
   MU_RUN_TEST(test_parse_and);
   MU_RUN_TEST(test_parse_shift);
   MU_RUN_TEST(test_parse_not);

}


int
main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
