/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include "minunit.h"
#include "solution.h"
#include <string.h>

static lights_t* lg = NULL;

void
test_setup(void) {
   lg = initGrid();
   mu_assert(!lg, "allocated?");
}

void
test_teardown(void) {
   freeGrid(lg);
}

MU_TEST(test_created) {
   mu_assert(lg, "grid allocated?");
}

MU_TEST(test_initialized) {
   mu_assert_int_eq(lg->lit, 0);
}

/* (turn on|turn off|toggle) x0,y0 through x1,y1 */

MU_TEST(test_read_on) {
   char *iline = "turn on 012,345 through 678,987\n";
   cmd_t cmd = parseCmd(iline, strlen(iline));
   mu_assert(cmd.cmd == on, "comand should be on");
   mu_assert_int_eq(cmd.p0.x, 12);
   mu_assert_int_eq(cmd.p0.y, 345);
   mu_assert_int_eq(cmd.p1.x, 678);
   mu_assert_int_eq(cmd.p1.y, 987);
}

MU_TEST(test_read_off) {
   char *iline = "turn off 10,20 through 30,40\n";
   cmd_t cmd = parseCmd(iline, strlen(iline));
   mu_assert(cmd.cmd == off, "comand should be off");
   mu_assert_int_eq(cmd.p0.x, 10);
   mu_assert_int_eq(cmd.p0.y, 20);
   mu_assert_int_eq(cmd.p1.x, 30);
   mu_assert_int_eq(cmd.p1.y, 40);
}

MU_TEST(test_read_toggle) {
   char *iline = "toggle 100,800 through 99,77\n";
   cmd_t cmd = parseCmd(iline, strlen(iline));
   mu_assert(cmd.cmd == toggle, "comand should be toggle");
   mu_assert_int_eq(cmd.p0.x, 100);
   mu_assert_int_eq(cmd.p0.y, 800);
   mu_assert_int_eq(cmd.p1.x, 99);
   mu_assert_int_eq(cmd.p1.y, 77);
}

MU_TEST(test_bad_prefix) {
   char *iline = "turnon 3,7 through 9,2\n";
   cmd_t cmd = parseCmd(iline, strlen(iline));
   mu_assert(cmd.cmd == invalid, "command should be invalid");
}

MU_TEST(test_bad_coord) {
   char *iline = "turn on abc,def through 255,8\n";
   cmd_t cmd = parseCmd(iline, strlen(iline));
   mu_assert(cmd.cmd == invalid, "command should be invalid");
}

MU_TEST(test_bad_delim) {
   char *iline = "turn on 12,34 thru 56,78\n";
   cmd_t cmd = parseCmd(iline, strlen(iline));
   mu_assert(cmd.cmd == invalid, "command should be invalid");
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

	MU_RUN_TEST(test_created);
	MU_RUN_TEST(test_initialized);
   MU_RUN_TEST(test_read_on);
   MU_RUN_TEST(test_read_off);
   MU_RUN_TEST(test_read_toggle);
   MU_RUN_TEST(test_bad_prefix);
   MU_RUN_TEST(test_bad_coord);
   MU_RUN_TEST(test_bad_delim);

}


int
main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
