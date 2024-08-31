/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include "minunit.h"
#include "solution.h"
#include <string.h>


/*
 * two separate grids, one for digital (part one), and one for analog
 * (part two).
 *
 * init and free are cheap operations.
 */

static lights *lgdigital = NULL;
static lights *lganalog = NULL;


/*
 * minunit setup and teardown of infrastructure.
 */

void
test_setup(void) {
	lgdigital = init_grid(1);        /* digital/partone rules */
	lganalog = init_grid(0);         /* analog/parttwo rules */
}

void
test_teardown(void) {
	free_grid(lgdigital);
	free_grid(lganalog);
}


/*
 * may as well verify creation separately.
 */

MU_TEST(test_created) {
	mu_assert(lgdigital, "grid allocated?");
	mu_assert(lganalog, "grid allocated?");
}

MU_TEST(test_initialized) {
	mu_assert_int_eq(lgdigital->lit, 0);
	mu_assert_int_eq(lganalog->intensity, 0);
}


/*
 * save some typing later...
 */

/* some standard coordinate pairs */
coord p00 = { 0, 0 };
coord p11 = { 1, 1 };
coord p12 = { 1, 2 };
coord p21 = { 2, 1 };
coord p22 = { 2, 2 };
coord p23 = { 2, 3 };
coord p33 = { 2, 3 };

/* valid command strings to parse */
char *clon = "turn on 012,345 through 678,987\n";
char *cloff = "turn off 10,20 through 30,40\n";
char *cltog = "toggle 100,800 through 99,77\n";

/* bad command strings to parse */
char *clbadprefix = "turnon 3,7 through 9,2\n";
char *clbadcoord = "turn on abc,def through 255,8\n";
char *clbaddelim = "turn on 12,34 thru 56,78\n";


/*
 * test that parsing is working. this should be independent of
 * analog or digital mode grids.
 *
 * (turn on|turn off|toggle) x0,y0 through x1,y1
 */

MU_TEST(test_read_on) {
	cmd cmd = parse_cmd(clon, strlen(clon));
	mu_assert(cmd.cmd == e_on, "command should be on");
	mu_assert_int_eq(cmd.p0.x, 12);
	mu_assert_int_eq(cmd.p0.y, 345);
	mu_assert_int_eq(cmd.p1.x, 678);
	mu_assert_int_eq(cmd.p1.y, 987);
}

MU_TEST(test_read_off) {
	cmd cmd = parse_cmd(cloff, strlen(cloff));
	mu_assert(cmd.cmd == e_off, "command should be off");
	mu_assert_int_eq(cmd.p0.x, 10);
	mu_assert_int_eq(cmd.p0.y, 20);
	mu_assert_int_eq(cmd.p1.x, 30);
	mu_assert_int_eq(cmd.p1.y, 40);
}

MU_TEST(test_read_toggle) {
	cmd cmd = parse_cmd(cltog, strlen(cltog));
	mu_assert(cmd.cmd == e_toggle, "command should be toggle");
	mu_assert_int_eq(cmd.p0.x, 100);
	mu_assert_int_eq(cmd.p0.y, 800);
	mu_assert_int_eq(cmd.p1.x, 99);
	mu_assert_int_eq(cmd.p1.y, 77);
}


/*
 * test that attempting to parase bad input fails in a
 * predictable manner.
 */

MU_TEST(test_bad_prefix) {
	cmd cmd = parse_cmd(clbadprefix, strlen(clbadprefix));
	mu_assert(cmd.cmd == e_invalid, "command should be invalid");
}

MU_TEST(test_bad_coord) {
	cmd cmd = parse_cmd(clbadcoord, strlen(clbadcoord));
	mu_assert(cmd.cmd == e_invalid, "command should be invalid");
}

MU_TEST(test_bad_delim) {
	cmd cmd = parse_cmd(clbaddelim, strlen(clbaddelim));
	mu_assert(cmd.cmd == e_invalid, "command should be invalid");
}


/*
 * is a light on or off?
 *
 * testing under digital rules.
 */

MU_TEST(test_light_on_off_d) {

	/* white box bit */
	lgdigital->bulb[0][0] = 1;
	lgdigital->lit = 1;

	/* black box bit */
	mu_assert_int_eq(number_on(lgdigital), 1);
	mu_assert(is_lit(lgdigital, p00), "this one should be lit");
	mu_assert(!is_lit(lgdigital, p11), "and this would shouldn't be");

	/* add another light */
	lgdigital->fntog(lgdigital, p11);
	mu_assert_int_eq(number_on(lgdigital), 2);
	mu_assert(is_lit(lgdigital, p00) && is_lit(lgdigital, p11), "both should be");

	/* turn a light on */
	lgdigital->fnon(lgdigital, p22);
	mu_assert_int_eq(number_on(lgdigital), 3);
	mu_assert(is_lit(lgdigital, p22), "should be on");
	mu_assert(!is_lit(lgdigital, p23), "should be off");

	/* turn off a light */
	lgdigital->fnoff(lgdigital, p11);
	mu_assert_int_eq(number_on(lgdigital), 2);
	mu_assert(!is_lit(lgdigital, p11), "now off");
}


/*
 * light up four in a single box under the digital rules.
 */

MU_TEST(test_four_d) {

	/* make sure grid is as we expect it */
	mu_assert_int_eq(lgdigital->lit, 0);

	/* a small box is lit */
	char *iline = "turn on 1,1 through 2,2\n";
	cmd cmd = parse_cmd(iline, strlen(iline));
	do_cmd(lgdigital, cmd);

	/* verify that things are as they should be */

	/* only four lit */
	mu_assert_int_eq(lgdigital->lit, 4);
	mu_assert_int_eq(lgdigital->intensity, 0);

	/* these are the four we lit */
	mu_assert(is_lit(lgdigital, p11), "");
	mu_assert(is_lit(lgdigital, p22), "");
	mu_assert(is_lit(lgdigital, p12), "");
	mu_assert(is_lit(lgdigital, p21), "");

	/* and these are some around the boundary that we should have
	   left alone. */
	mu_assert(!is_lit(lgdigital, p23), "");
	mu_assert(!is_lit(lgdigital, p00), "");
	mu_assert(!is_lit(lgdigital, p33), "");
}


/*
 * test just turning on four under analog rules.
 */
MU_TEST(test_four_a) {
	/* make sure grid is as we expect it */
	mu_assert_int_eq(lganalog->lit, 0);

	/* a small box is lit */
	char *iline = "turn on 1,1 through 2,2\n";
	cmd cmd = parse_cmd(iline, strlen(iline));
	do_cmd(lganalog, cmd);

	/* verify that things are as they should be */

	/* only four lit, and in analog we should report
	   the intensity as well. */
	/* mu_assert_int_eq(lganalog->lit, 4); */
	mu_assert_int_eq(lganalog->intensity, 4);

	/* these are the four we lit */
	mu_assert(is_lit(lganalog, p11), "");
	mu_assert(is_lit(lganalog, p22), "");
	mu_assert(is_lit(lganalog, p12), "");
	mu_assert(is_lit(lganalog, p21), "");

	/* and these are some around the boundary that we should have
	   left alone. */
	mu_assert(!is_lit(lganalog, p23), "");
	mu_assert(!is_lit(lganalog, p00), "");
	mu_assert(!is_lit(lganalog, p33), "");

}

MU_TEST(test_analog) {
	/* make sure grid is as we expect it */
	mu_assert_int_eq(lganalog->lit, 0);

	/* a small box is lit */
	char *iline = "turn on 1,1 through 2,2\n";
	cmd cmd = parse_cmd(iline, strlen(iline));
	do_cmd(lganalog, cmd);

	/* verify that things are as they should be */
	mu_assert_int_eq(lganalog->intensity, 4);

	/* dim one light by 1 */
	iline = "turn off 1,1 through 1,1\n";
	cmd = parse_cmd(iline, strlen(iline));
	do_cmd(lganalog, cmd);
	mu_assert_int_eq(lganalog->intensity, 3);
	mu_assert(!is_lit(lganalog, p11), "");

	/* toggle (+2 in analog) the original 4 */
	iline = "toggle 1,1 through 2,2\n";
	cmd = parse_cmd(iline, strlen(iline));
	do_cmd(lganalog, cmd);
	mu_assert_int_eq(lganalog->intensity, 11); /* 3 + 4*2 */

	/* turn off the lights 3 times, so -3 from all */
	iline = "turn off 1,1 through 2,2\n";
	cmd = parse_cmd(iline, strlen(iline));
	do_cmd(lganalog, cmd);
	do_cmd(lganalog, cmd);
	do_cmd(lganalog, cmd);

	/* 1,1 was 0+2, so it should now be 0 */
	mu_assert(!is_lit(lganalog, p11), "");

	/* the other 3 were also at 3, so they should be zero as well */
	mu_assert_int_eq(lganalog->intensity, 0);

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

	/* correct parsing of commands */
	MU_RUN_TEST(test_read_on);
	MU_RUN_TEST(test_read_off);
	MU_RUN_TEST(test_read_toggle);

	/* detect bad commands */
	MU_RUN_TEST(test_bad_prefix);
	MU_RUN_TEST(test_bad_coord);
	MU_RUN_TEST(test_bad_delim);

	/* digital mode tests */
	MU_RUN_TEST(test_light_on_off_d);
	MU_RUN_TEST(test_four_d);

	/* analog mode tests */
	MU_RUN_TEST(test_four_a);
	MU_RUN_TEST(test_analog);

}


int
main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
