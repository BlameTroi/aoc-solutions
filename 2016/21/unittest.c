/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"
#include "solution.h"
#include "txbstr.h"

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
/*For example, suppose you start with abcde and perform the following
operations:
 */
MU_TEST(test_sample) {
   char *password = dup_string("abcde");
   int n = strlen(password);

   swap_position(password, n, 4, 0);
   printf("\n%s\n", password);
   mu_should(equal_string(password, "ebcda"));
   swap_letter(password, n, 'd', 'b');
   printf("\n%s\n", password);
   mu_should(equal_string(password, "edcba"));
   reverse_positions(password, n, 0, 4);
   printf("\n%s\n", password);
   mu_should(equal_string(password, "abcde"));

   rotate_left(password, n, 1);
   printf("\n%s\n", password);
   mu_should(equal_string(password, "bcdea"));
   move_position(password, n, 1, 4);
   printf("\n%s\n", password);
   mu_should(equal_string(password, "bdeac"));

   move_position(password, n, 3, 0);
   printf("\n%s\n", password);
   mu_should(equal_string(password, "abdec"));
   rotate_position(password, n, 'b');
   printf("\n%s\n", password);
   mu_should(equal_string(password, "ecabd"));
   rotate_position(password, n, 'd');
   printf("\n%s\n", password);
   mu_should(equal_string(password, "decab"));
}

MU_TEST(test_undo) {
   char *password = dup_string("abcde");
   int n = strlen(password);

   printf("\n%s\n", password);
   processor(password, n, "swap position 4 with position 0");
   printf("\n%s\n", password);
   mu_should(equal_string(password, "ebcda"));
   undoer(password, n, "swap position 4 with position 0");
   printf("\n%s\n", password);
   mu_should(equal_string(password, "abcde"));
   swap_position(password, n, 4, 0);
   mu_should(equal_string(password, "ebcda"));

   processor(password, n, "swap letter d with letter b");
   printf("\n%s\n", password);
   mu_should(equal_string(password, "edcba"));
   undoer(password, n, "swap letter d with letter b");
   mu_should(equal_string(password, "ebcda"));
   swap_letter(password, n, 'd', 'b');
   mu_should(equal_string(password, "edcba"));

   processor(password, n, "reverse positions 0 through 4");
   printf("\n%s\n", password);
   mu_should(equal_string(password, "abcde"));
   undoer(password, n, "reverse positions 0 through 4");
   mu_should(equal_string(password, "edcba"));
   reverse_positions(password, n, 0, 4);
   mu_should(equal_string(password, "abcde"));


   processor(password, n, "rotate left 1 step");
   printf("\n%s\n", password);
   mu_should(equal_string(password, "bcdea"));
   move_position(password, n, 1, 4);
   printf("\n%s\n", password);
   mu_should(equal_string(password, "bdeac"));

   free(password);
   password = dup_string("01234567");
   n = strlen(password);
   printf("\n%s\n", password);
   processor(password, n, "move position 6 to position 3");
   printf("\n%s\n", password);


   free(password);
}

/*
  rotate based on position of letter d finds the index of letter d (4), then rotates the string right once, plus a number of times equal to that index, plus an additional time because the index was at least 4, for a total of 6 right rotations: decab.

  ratate based is only partially positional, but how to undo?

  rotate based on the position of 2 below would therefore rotate
  right 1 + 2 (index of 2) = 3

  01234567
  56701234

  to get back it's a rotate left 3, but how do we know that?

  pos 2 is now 5

  rotate based on position of 4 would be
  right + 1 + 4 (index of 4) + 1 = 6

  01234567
  12345670

  01234567   by pos 7
  right + 1 + 7 + 1 = 9

  70123456

{ { 1, 1 }, { 3, 2 }, { 5, 3 }, { 7, 4 }, { 2,  5 }, { 4, 7 }, { 6, 8 },  { 0, 1 } }
01234567
70123456     <- after rotate based on position of character '0'
 ^           1 + 0
 1
01234567
67012345     <- after rotate based on position of character '1'
   ^         1 + 1
   2
01234567
56701234     <- after rotate based on position of character '2'
     ^       1 + 2
     3
01234567
45670123     <- after rotate based on position of character '3'
       ^     1 + 3
       4
01234567
23456701     <- after rotate based on position of character '4'
  ^          1 + 4 + 1
  5
01234567
12345670     <- after rotate based on position of character '5'
    ^        1 + 5 + 1
    7
01234567
01234567     <- after rotate based on position of character '6'
      ^      1 + 6 + 1
      8
01234567
70123456     <- after rotate based on position of character '7'
^            1 + 7 + 1
9 % 8 = 1

 */
MU_TEST(test_rotate) {
   char *password = dup_string("01234567");
   int n = strlen(password);
   strcpy(password, "01234567");
   printf("\n%s\n", password);
   rotate_position(password, n, '2');
   printf("\n%s     <- after rotate based on position of character '2'\n", password);
   printf("\n");
   for (int i = 0; i < strlen(password); i++) {
      char chr = '0' + i;
      strcpy(password, "01234567");
      printf("\n%s", password);
      rotate_position(password, n, chr);
      printf("\n%s     <- after rotate based on position of character '%c'\n", password, chr);
   }

   char *cmd = dup_string("rotate based on position of letter c               ");
   char cmd_len = strlen(cmd);
   for (int i = 0; i < strlen(password); i++) {
      strcpy(password, "abcdefgh");
      printf("\n%s", password);
      snprintf(cmd, cmd_len, "rotate based on position of letter %c", password[i]);
      processor(password, n, cmd);
      printf("\n%s     <- after %s\n", password, cmd);
      undoer(password, n, cmd);
      printf("\n%s     <- after undo %s\n", password, cmd);
   }

   mu_should(true);
   free(password);
}

MU_TEST(test_indices) {
   char *original = dup_string("01234567");
   char *rotated = dup_string("01234567");
   char *trial = dup_string("01234567");

   int n = strlen(original);
   int indices[] = { -1, -1, -1, -1, -1, -1, -1, -1 };

   for (int based = 0; based < 8; based++) {
      bool solved = false;
      strcpy(rotated, original);
      rotate_position(rotated, n, '0' + based);
      printf("\nsolving %s based %c %s ", original, '0' + based, rotated);
      int x = 0;
      while (!solved && x < 8) {
         strcpy(trial, rotated);
         rotate_right(trial, n, x);
         printf("%c", '0' + x);
         if (equal_string(trial, original)) {
            solved = true;
            indices[based] = x;
            printf("<");
         }
         x += 1;
      }
      if (!solved) {
         printf("**error**");
      }
   }

   printf("\nindex table:\n");
   for (int based = 0; based < 8; based++) {
      printf("%d -> %d\n", based, indices[based]);
   }

}

/*
 * sample test shell.
 */

MU_TEST(test_test) {
   mu_should(true);
   mu_shouldnt(false);
}

/*
 * here we define the whole test suite.
 */
MU_TEST_SUITE(test_suite) {

   MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

   /* run your tests here */
   MU_RUN_TEST(test_test);
   MU_RUN_TEST(test_sample);
   MU_RUN_TEST(test_undo);
   MU_RUN_TEST(test_rotate);
   MU_RUN_TEST(test_indices);
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
