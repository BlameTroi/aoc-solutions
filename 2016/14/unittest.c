/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"
#include "solution.h"

void
print_md5_digest(uint8_t *digest) {
   printf("\ndigest: ");
   for (int i = 0; i < 4; i++) {
      printf("%02x", digest[i]);
   }
   printf(" ");
   for (int i = 4; i < 8; i++) {
      printf("%02x", digest[i]);
   }
   printf("  ");
   for (int i = 8; i < 12; i++) {
      printf("%02x", digest[i]);
   }
   printf(" ");
   for (int i = 12; i < 16; i++) {
      printf("%02x", digest[i]);
   }
   printf("\n");
}


bool got_args = false;
int num_args = 0;
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
   printf("\n");
   hash_check *hc;

   /* abc18 -> 0034e0923cc38887a57bd7b1d4f953df */
   hc = create_hash_check("abc", 18, 0);
   mu_shouldnt(hc->has_quintuples);
   mu_should(hc->has_triple);
   mu_should(hc->triple_digit == 8);
   mu_shouldnt(hc->quintuples[8]);
   free(hc);

   /* abc39 -> 347dac6ee8eeea4652c7476d0f97bee5 */
   hc = create_hash_check("abc", 39, 0);
   print_hash_check(hc, "abc", 39);
   mu_shouldnt(hc->has_quintuples);
   mu_should(hc->has_triple);
   mu_should(hc->triple_digit == 14);
   free(hc);

   /* abc92 -> ae2e85dd75d63e916a525df95e999ea0 */
   hc = create_hash_check("abc", 92, 0);
   print_hash_check(hc, "abc", 92);
   mu_should(hc->has_triple);
   mu_should(hc->triple_digit == 9);
   free(hc);

   /* abc200 -> 83501e9109999965af11270ef8d61a4f */
   hc = create_hash_check("abc", 200, 0);
   print_hash_check(hc, "abc", 200);
   mu_should(hc->has_triple);
   mu_should(hc->has_quintuples);
   mu_should(hc->triple_digit == 9);
   mu_should(hc->quintuples[9]);
   free(hc);

   /* abc816 -> 3aeeeee1367614f3061d165a5fe3cac3 */
   hc = create_hash_check("abc", 816, 0);
   print_hash_check(hc, "abc", 816);
   mu_should(hc->has_triple);
   mu_should(hc->has_quintuples);
   mu_should(hc->quintuples[14]);
   mu_should(hc->triple_digit == 14);
   free(hc);

   /* abc22728 -> 26ccc731a8706e0c4f979aeb341871f0 */
   hc = create_hash_check("abc", 22728, 0);
   print_hash_check(hc, "abc", 22728);
   mu_should(hc->has_triple);
   mu_shouldnt(hc->has_quintuples);
   mu_should(hc->triple_digit == 12);
   mu_shouldnt(hc->quintuples[12]);
   free(hc);
}

MU_TEST(test_debug) {
   printf("\n");

   hash_check *hc;

   hc = create_hash_check("abc", 7857, 0);
   print_hash_check(hc, "abc", 7857);
   free(hc);
   hc = 0;

   hc = create_hash_check("abc", 8717, 0);
   print_hash_check(hc, "abc", 8717);
   free(hc);
   hc = 0;

   long idx = -1;
   int quints = 0;
   while (quints < 32) {
      if (hc) {
         free(hc);
         hc = 0;
      }
      idx += 1;
      hc = create_hash_check("abc", idx, 0);
      if (!hc) {
         continue;
      }
      if (!hc->has_quintuples) {
         continue;
      }
      print_hash_check(hc, "abc", idx);
      quints += 1;
   }
   free(hc);

}

MU_TEST(test_loop) {
   printf("\n");
   char *salt = "ahsbgdzn";
   /* salt = "abc"; */
   long idx = 0;
   hash_check *hc;
   int triples = 0;
   int quintuples = 0;
   while (idx <= 22728 + 1001) {
      hc = create_hash_check(salt, idx, 0);
      if (!hc) {
         idx += 1;
         continue;
      }
      if (strcmp(salt, "abc") == 0 && idx == 816) {
         printf("816: ");
         mu_should(hc->has_triple);
         mu_should(hc->has_quintuples);
         printf("\n");
      }
      if (hc->has_triple || hc->has_quintuples) {
         if (hc->has_triple) {
            triples += 1;
         }
         if (hc->has_quintuples) {
            quintuples += 1;
         }
         print_hash_check(hc, salt, idx);
      }
      free(hc);
      idx += 1;
   }
   printf("triples %d\n", triples);
   printf("quintuples %d\n", quintuples);
   mu_should(triples > quintuples);
}

/*
 * here we defini the whole test suite.
 */
MU_TEST_SUITE(test_suite) {

   MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

   /* run your tests here */
   MU_RUN_TEST(test_test);
   MU_RUN_TEST(test_loop);
   MU_RUN_TEST(test_debug);
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
