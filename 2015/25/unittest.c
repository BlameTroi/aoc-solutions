/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "minunit.h"
#include "solution.h"

void
test_setup(void) {
}

void
test_teardown(void) {
}

#define DESIRED_ROW      2981
#define DESIRED_COL      3075

MU_TEST(test_diag) {
   printf("%d,1 = %lu\n", 6, diag_begin(6));
   printf("1,%d = %lu\n", 6, diag_end(6));
   printf("%d,1 = %lu\n", DESIRED_ROW, diag_begin(DESIRED_ROW));
   printf("1,%d = %lu\n", DESIRED_ROW, diag_end(DESIRED_ROW));
   printf("%d,1 = %lu\n", DESIRED_COL, diag_begin(DESIRED_COL));
   printf("1,%d = %lu\n", DESIRED_COL, diag_end(DESIRED_COL));
   printf("%d,1 = %lu\n", 7, diag_begin(7));
   printf("1,%d = %lu\n", 7, diag_end(7));
   mu_should(diag_begin(6) == 16);
   mu_should(diag_end(6) == 21);
}

MU_TEST(test_next_code) {
   unsigned long code[50];
   code[1] = SEED;
   printf("%3d %12lu\n", 1, code[0]);
   for (int i = 2; i < 50; i++) {
      code[i] = next_code(code[i-1]);
      printf("%3d %12lu\n", i, code[i]);
   }
   /* 15 10071777
   16 33071741
    5 21629792
   11    77061 */
   mu_should(code[15] == 10071777);
   mu_should(code[16] == 33071741);
   mu_should(code[5] == 21629792);
   mu_should(code[11] == 77061);
}

MU_TEST(test_full_run) {
   const int COL = 3075;
   const int ROW = 2981;
   unsigned long code[COL+2];
   code[1] = SEED;
   for (int i = 2; i <= COL; i++) {
      code[i] = next_code(code[i-1]);
   }
   for (int i = ROW; i <= COL; i++) {
      printf("%5d %12lu\n", i, code[i]);
   }
   mu_should(code[21] == 33511524);
}


MU_TEST(test_bfc) {
   const int COL = 3075;
   const int ROW = 2981;
   unsigned long code = code_at(ROW, COL);
   mu_should(code);
   printf("%lu\n", code);
}

MU_TEST_SUITE(test_suite) {
   MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

   MU_RUN_TEST(test_next_code);
   MU_RUN_TEST(test_full_run);
   MU_RUN_TEST(test_diag);
   MU_RUN_TEST(test_bfc);

}

int
main(int argc, char *argv[]) {
   MU_RUN_SUITE(test_suite);
   MU_REPORT();
   return MU_EXIT_CODE;
}
