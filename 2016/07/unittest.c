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

bool got_args = false;
int num_args = 0;
char **the_args = NULL;
char *filename = NULL;
FILE *datafile = NULL;
char *test_string = NULL;

/*
 * unittest (filename | -f filename) | -e string ...
 */

bool
get_args(void) {
   if (num_args < 2) {
      got_args = true;
   }
   if (got_args) {
      return true;
   }

   if (num_args == 2) {
      filename = the_args[1];
      got_args = true;
      return got_args;
   }

   if (num_args > 2 && strcmp(the_args[1], "-f") == 0) {
      filename = the_args[2];
      got_args = true;
      return got_args;
   }

   if (strcmp(the_args[1], "-i") != 0) {
      fprintf(stderr, "error: illegal arguments to unittest\n");
      exit(EXIT_FAILURE);
   }

   int len = num_args;
   for (int i = 2; i < num_args; i++) {
      len += strlen(the_args[i]);
   }
   test_string = malloc(len+1);
   memset(test_string, 0, len+1);
   for (int i = 2; i < num_args; i++) {
      if (i != 2) {
         strcat(test_string, " ");
      }
      strcat(test_string, the_args[i]);
   }

   got_args = true;
   return got_args;
}

void
test_setup(void) {
   get_args();
   if (filename) {
      datafile = fopen(filename, "r");
      if (!datafile) {
         fprintf(stderr, "error: could not open test data file!\n");
         exit(EXIT_FAILURE);
      }
   }
}

void
test_teardown(void) {
   if (datafile) {
      fclose(datafile);
   }
}

/*
 * sample test shell.
 */

typedef struct test_data test_data;

struct test_data {
   bool valid;
   char *data;
};


const char *
bool_string(bool b) {
   return b ? "true" : "false";
}

MU_TEST(test_abba) {
   mu_should(has_good_abba("abba"));
   mu_should(has_good_abba("asdfasdfbccbgx"));
   mu_should(has_good_abba("xxxxxxxxxxxabba"));
   mu_shouldnt(has_good_abba("aaaaaaaaxabbc"));
   mu_shouldnt(has_good_abba("xxxxxxxxzabb"));
   mu_should(has_bad_abba("abba[cddcasdf]"));
   mu_shouldnt(has_bad_abba("abba[cddeghhj]"));
}

/*
 * for part one, the only abba patterns must be outside of [].
 */

test_data cases_one[] = {
   { true, "abba[mnop]qrst" },
   { false, "abcd[bddb]xyyx" },
   { false, "aaaa[qwer]tyui" },
   { true, "ioxxoj[asdfgh]zxcvbn" },
};

MU_TEST(test_one) {
   printf("\n");
   int count = 0;
   for (int i = 0; i < sizeof(cases_one) / sizeof(test_data); i++) {
      bool good = supports_tls(cases_one[i].data);
      printf("expected: %-5s  got: %-5s  case: '%s'\n", bool_string(cases_one[i].valid), bool_string(good), cases_one[i].data);
      if (good) {
         count += 1;
      }
   }
   mu_should(count == 2);
}

/*
 * for part two, an aba sequence outside of [] must have a corresponding
 * bab sequence inside them.
 */

test_data cases_two[] = {
   { true, "aba[bab]xyz" },
   { false, "xyx[xyx]xyx" },
   { true, "aaa[kek]eke" },
   {true, "zazbz[bzb]cdb" },
};

MU_TEST(test_two) {
   printf("\n");
   int count = 0;
   for (int i = 0; i < sizeof(cases_two) / sizeof(test_data); i++) {
      bool good = supports_ssl(cases_two[i].data);
      printf("expected: %-5s  got: %-5s  case: '%s'\n", bool_string(cases_two[i].valid), bool_string(good), cases_two[i].data);
      if (good) {
         count += 1;
      }
   }
   mu_should(count == 3);
}

MU_TEST(test_edges) {
   /* expected to match asdfd dfd as asdfdf at fdf */
   bool good = supports_ssl("asdfd[asdfdf]");
   mu_should(good);
}

/*
 * here we define the whole test suite.
 */
MU_TEST_SUITE(test_suite) {

   MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

   /* run your tests here */
   MU_RUN_TEST(test_edges);
   MU_RUN_TEST(test_one);
   MU_RUN_TEST(test_two);
   MU_RUN_TEST(test_abba);
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
