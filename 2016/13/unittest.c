/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <stdbool.h>
#include <stdio.h>

#include "minunit.h"
#include "solution.h"
#include "txbdl.h"

int num_args;
char **the_args;

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
   maze *m = create_maze(7, 10, 10);
   print_maze(m);
   maze_coordinate from = {1, 1};
   maze_coordinate to = {4, 7};

   int d = shortest_path_length(m, from, to);

   printf("%d\n", d);

   dlcb *dl = cells_within_path_length(m, from, to, 5);
   printf("\n%d\n", dl_count(dl));
   dl_delete_all(dl);
   dl_destroy(dl);

   destroy_maze(m);

   m = create_maze(50, 50, 1350);
   print_maze(m);

   from = (maze_coordinate) {
      1, 1
   };
   to = (maze_coordinate) {
      39, 31
   };

   d = shortest_path_length(m, from, to);
   printf("\n%d\n", d);

   d = shortest_path_length(m, to, from);
   printf("\n%d\n", d);

   dl = cells_within_path_length(m, from, to, 50);
   printf("\n%d\n", dl_count(dl));
   dl_delete_all(dl);
   dl_destroy(dl);
   destroy_maze(m);
   mu_should(true);
   mu_shouldnt(false);
}

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
