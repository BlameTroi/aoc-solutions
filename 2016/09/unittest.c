/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"
#include "solution.h"

#include "txbmisc.h"
#include "txbsb.h"
#include "txbrs.h"

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

/*
 * the format compresses a sequence of characters. whitespace is
 * ignored. to indicate that some sequence should be repeated, a
 * marker is added to the file, like (10x2). to decompress this
 * marker, take the subsequent 10 characters and repeat them 2 times.
 * then, continue reading the file after the repeated data. the marker
 * itself is not included in the decompressed output.
 *
 */

char *cases[][2] = {
   { "advent", "advent" },
   { "a(1x5)bc", "abbbbbc" },
   { "(3x3)xyz", "xyzxyzxyz" },
   { "a(2x2)bcd(2x2)efg", "abcbcdefefg" },
   { "(6x1)(1x3)a", "(1x3)a" }, /* (1x3) is not a marker when consummed by (6x1) */
   { "x(8x2)(3x3)abcy", "x(3x3)abc(3x3)abcy" },
   { NULL, NULL }
};

MU_TEST(test_test) {
   printf("\n");
   int i = 0;
   while (cases[i][0]) {
      rscb *rs = rs_create_string(cases[i][0]);
      sbcb *sb = sb_create();
      int c;
      while ((c = rs_getc(rs)) != EOF) {
         if (c != '(') {
            if (c > ' ') {
               sb_putc(sb, c);
            }
            continue;
         }
         int run_length = 0;
         int repeat_count = 0;
         while (is_digit(c = rs_getc(rs))) {
            run_length = run_length * 10 + (c - '0');
         }
         assert(c == 'x');
         while (is_digit(c = rs_getc(rs))) {
            repeat_count = repeat_count * 10 + (c - '0');
         }
         assert(c == ')');
         sbcb *run = sb_create();
         while (run_length) {
            assert(!rs_at_end(rs));
            sb_putc(run, rs_getc(rs));
            run_length -= 1;
         }
         char *temp = sb_to_string(run);
         while (repeat_count) {
            sb_puts(sb, temp);
            repeat_count -= 1;
         }
         sb_destroy(run);
         free(temp);
      }

      printf("   input: len(%3lu) '%s'\n", strlen(cases[i][0]), cases[i][0]);
      char *temp = sb_to_string(sb);
      printf("  output: len(%3lu) '%s'\n", strlen(temp), temp);
      printf("expected: len(%3lu) '%s'\n", strlen(cases[i][1]), cases[i][1]);
      printf("rs_position: %lu\n", rs_position(rs));
      printf("rs_at_end: %s\n", rs_at_end(rs) ? "yes" : "no");
      printf("sb_length: %lu\n", sb_length(sb));

      mu_should(strcmp(cases[i][1], temp) == 0);
      mu_should(strlen(cases[i][1]) == strlen(temp));
      mu_should(strlen(temp) == sb_length(sb));
      printf("\n");

      free(temp);
      sb_destroy(sb);
      rs_destroy_string(rs);

      i += 1;
   }

   mu_should(true);
   mu_shouldnt(false);
}

MU_TEST(test_from_file) {
   mu_should(filename);
   printf("\n");
   /* i know the file is < 20k bytes */
   char *data = malloc(32767);
   memset(data, 0, 32767);
   fgets(data, 32766, datafile);
   printf("size of input data: %lu\n", strlen(data));

   rscb *rs = rs_create_string(data);
   sbcb *sb = sb_create();
   int c;
   while ((c = rs_getc(rs)) != EOF) {
      if (c != '(') {
         if (c > ' ') {
            sb_putc(sb, c);
         }
         continue;
      }
      int run_length = 0;
      int repeat_count = 0;
      while (is_digit(c = rs_getc(rs))) {
         run_length = run_length * 10 + (c - '0');
      }
      assert(c == 'x');
      while (is_digit(c = rs_getc(rs))) {
         repeat_count = repeat_count * 10 + (c - '0');
      }
      assert(c == ')');
      sbcb *run = sb_create();
      while (run_length) {
         assert(!rs_at_end(rs));
         sb_putc(run, rs_getc(rs));
         run_length -= 1;
      }
      char *temp = sb_to_string(run);
      while (repeat_count) {
         sb_puts(sb, temp);
         repeat_count -= 1;
      }
      sb_destroy(run);
      free(temp);
   }
   char *temp = sb_to_string(sb);
   printf("input length: %lu\n", strlen(data));
   printf("output length: %lu\n", strlen(temp));
   printf("difference in lengths %ld\n", (strlen(data) - strlen(temp)));
   free(sb);
   free(rs);
   free(data);
   free(temp);

}


/*
 * here we defini the whole test suite.
 */
MU_TEST_SUITE(test_suite) {

   MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

   /* run your tests here */
   MU_RUN_TEST(test_test);
   MU_RUN_TEST(test_from_file);
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
