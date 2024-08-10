/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
   initialize_floor_map(3, 5, "..^^.");
   /* ..^^. */
   /* .^^^^ */
   /* ^^..^ */
   fill_floor_map();
   int safes = 0;
   int traps = 0;
   count_map(&traps, &safes);
   print_map();
   printf("traps %d safes %d\n", traps, safes);
   mu_should(traps == 9);
   mu_should(safes == 6);
   initialize_floor_map(10, 10, ".^^.^.^^^^");
   fill_floor_map();
   count_map(&traps, &safes);
   print_map();
   printf("traps %d safes %d\n", traps, safes);
   mu_should(safes ==38);
   /*
   .^^.^.^^^^
   ^^^...^..^
   ^.^^.^.^^.
   ..^^...^^^
   .^^^^.^^.^
   ^^..^.^^..
   ^^^^..^^^.
   ^..^^^^.^^
   .^^^..^.^^
   ^^.^^^..^^ */
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
