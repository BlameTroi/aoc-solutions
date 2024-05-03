// unittest.c -- shell for advent of code unit tests -- troy brumley

// because you should always make an effort to test first!


// standard includes, most of these are actually included in
// "minunit.h" (which is completely self contained, there is no
// matching library). but, i like to always explicitly include
// that which i plan to use.

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>


// local includes, so far jsut minunit.

#include "minunit.h"


// this is some of the test code from minunit_example.c that came with
// the library. it is left for templating and documentation.

static int foo = 0;
static int bar = 0;
static double dbar = 0.1;
static const char* foostring = "Thisstring";

void test_setup(void) {
	foo = 7;
	bar = 4;
}

void test_teardown(void) {
	// does nothing
}

// a passing test, the MU_TEST macro does nothing beyond:
//
// static void method_name(void)
//
// so no parameters and no return value.
MU_TEST(test_check) {

   // the mu_check(test) macro is an assertion that embeds the test in
   // supporting code to log success or failure of the assertion,
   // calculate timing, and so on.

	mu_check(foo == 7);
}

MU_TEST(test_check_fail) {

   // the prior check passed quietly. this one will fail and print an
   // informational error message.

	mu_check(foo != 7);
}

MU_TEST(test_assert) {

   // a more explanatory version of mu_check.

	mu_assert(foo == 7, "foo should be 7");
}

MU_TEST(test_assert_fail) {
	mu_assert(foo != 7, "foo should be <> 7");
}

MU_TEST(test_assert_int_eq) {

   // and alternatives
	mu_assert_int_eq(4, bar);
}

MU_TEST(test_assert_int_eq_fail) {

	mu_assert_int_eq(5, bar);
}

MU_TEST(test_assert_double_eq) {
	mu_assert_double_eq(0.1, dbar);
}

MU_TEST(test_assert_double_eq_fail) {
	mu_assert_double_eq(0.2, dbar);
}

MU_TEST(test_fail) {

   // why you would, i do not know, but you can just fail.

	mu_fail("Fail now!");
}

MU_TEST(test_string_eq){
	mu_assert_string_eq("Thisstring", foostring);
}

MU_TEST(test_string_eq_fail){
	mu_assert_string_eq("Thatstring", foostring);
}


// here we define the whole test suite. sadly there's no runtime
// introspection. there is probably an opportunity for an elisp
// helper to create the suite in the editor, but for now it's
// just a matter of doing it manually.

// as with the MU_TEST macro, MU_TEST_SUITE a veneer.
MU_TEST_SUITE(test_suite) {

   // always have a setup and teardown, even if they
   // do nothing.

	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	MU_RUN_TEST(test_check);
	MU_RUN_TEST(test_assert);
	MU_RUN_TEST(test_assert_int_eq);
	MU_RUN_TEST(test_assert_double_eq);

	MU_RUN_TEST(test_check_fail);
	MU_RUN_TEST(test_assert_fail);
	MU_RUN_TEST(test_assert_int_eq_fail);
	MU_RUN_TEST(test_assert_double_eq_fail);

	MU_RUN_TEST(test_string_eq);
	MU_RUN_TEST(test_string_eq_fail);

	MU_RUN_TEST(test_fail);
}

int main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
