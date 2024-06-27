/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "minunit.h"

#include "txbmisc.h"

#include "solution.h"


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
 * sample test shell.
 */

MU_TEST(test_factor) {
   int factors2[] = {1, 2, 0};
   int factors4[] = {1, 2, 4, 0};
   int factors20[] = {1, 2, 4, 5, 10, 20, 0};
   long *result = NULL;
   int i = 0;

   result = factors_of(2);
   i = 0;
   while (factors2[i]) {
      mu_assert_int_eq(factors2[i], result[i]);
      i += 1;
   }
   free(result);

   result = factors_of(0);
   mu_assert_int_eq(true, result == NULL);
   result = factors_of(-1234);
   mu_assert_int_eq(true, result == NULL);

   result = factors_of(4);
   i = 0;
   while (factors4[i]) {
      mu_assert_int_eq(factors4[i], result[i]);
      i += 1;
   }
   free(result);

   result = factors_of(20);
   i = 0;
   while (factors4[i]) {
      mu_assert_int_eq(factors20[i], result[i]);
      i += 1;
   }
   free(result);

   result = factors_of(99);
   free(result);
   result = factors_of(100);
   free(result);
   result = factors_of(999);
   free(result);
   result = factors_of(1000);
   free(result);
   result = factors_of(9999);
   free(result);
   result = factors_of(10000);
   free(result);
   result = factors_of(99999);
   free(result);
   result = factors_of(100000);
   free(result);
   result = factors_of(999999);
   free(result);
   result = factors_of(1000000);
   free(result);
   result = factors_of(33100000);
   free(result);

   /*
     House 1 got 10 presents.
     House 2 got 30 presents.
     House 3 got 40 presents.
     House 4 got 70 presents.
     House 5 got 60 presents.
     House 6 got 120 presents.
     House 7 got 80 presents.
     House 8 got 150 presents.
     House 9 got 130 presents.
     */

   const long num_houses = 500000;
   long gifts[num_houses+1] = {0, 1, 3, 4, 7, 6, 12, 8, 15, 13};
   for (long h = 10; h < num_houses+1; h++) {
      gifts[h] = 0;
   }

   long num_factors[num_houses+1];
   memset(num_factors, 0, sizeof(num_factors));
   long sum_factors[num_houses+1];
   memset(sum_factors, 0, sizeof(sum_factors));
   long *the_factors[num_houses+1];
   memset(the_factors, 0, sizeof(the_factors));

   for (long h = 1; h < num_houses + 1; h++) {
      the_factors[h] = factors_of(h);
      long i = 0;
      while (the_factors[h][i]) {
         sum_factors[h] += the_factors[h][i];
         i += 1;
      }
      num_factors[h] = i;
   }

   char *fmt_prefix = "%3ld [f:%2ld][g:%2ld][e:%2ld] :";
   char *fmt_factor = " %ld";

   printf("\n\n");
   for (long h = 1; h < num_houses + 1; h++) {
      printf(fmt_prefix, h, num_factors[h], sum_factors[h], gifts[h]);
      long i = 0;
      while (true) {
         printf(fmt_factor, the_factors[h][i]);
         if (the_factors[h][i] == 0) {
            break;
         }
         i += 1;
      }
      printf("\n");
   }

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
   MU_RUN_TEST(test_factor);

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
