/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <assert.h>
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
/* one line of instructions per digit from keypad */

char *sample_instructions[5] = {
   "ULL\n",
   "RRDDD\n",
   "LURDL\n",
   "UUUUD\n",
   NULL
};

/*
 * sample test shell.
 */

extern keypad_def keypad_one_def;

MU_TEST(test_sample) {
   keypad_pos pos = { 1, 1 };
   int iline = 0;
   char *move = sample_instructions[iline];
   char *result = malloc(RESULT_MAX + 1);
   memset(result, 0, RESULT_MAX + 1);
   int p = 0;
   while (move) {
      if (*move == '\n') {
         result[p] = keypad_one_def.fnreader(pos.row, pos.col);
         p += 1;
         assert(p < RESULT_MAX);
         iline += 1;
         move = sample_instructions[iline];
         continue;
      }
      move_key(&pos, keypad_one_def, *move);
      move += 1;
   }
   printf("%s\n", result);
   mu_should(strcmp(result, "1985") == 0);
}

MU_TEST(test_file) {
   mu_should(datafile);
   rewind(datafile);

   keypad_pos pos = { 1, 1 };

   char *result = malloc(RESULT_MAX + 1);
   memset(result, 0, RESULT_MAX + 1);
   int p = 0;

   char buffer[INPUT_LINE_MAX];
   memset(buffer, 0, INPUT_LINE_MAX);

   while (fgets(buffer, INPUT_LINE_MAX - 1, datafile)) {
      char *move = buffer;
      while (*move) {
         if (*move == '\n') {
            move += 1;
            continue;
         }
         move_key(&pos, keypad_one_def, *move);
         move += 1;
      }
      result[p] = keypad_one_def.fnreader(pos.row, pos.col);
      p += 1;
      assert(p < RESULT_MAX);
   }
   printf("%s\n", result);

   mu_should(strcmp(result, "38961") == 0);
}

/*
 * here we define the whole test suite.
 */
MU_TEST_SUITE(test_suite) {

   MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

   /* run your tests here */
   MU_RUN_TEST(test_sample);
   MU_RUN_TEST(test_file);
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
