/* solution.c -- aoc 2016 xx -- troy brumley */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

#define MESSAGE_MAX 16
int frequency[MESSAGE_MAX][255];

void
clear_frequency(void) {
   memset(frequency, 0, sizeof(frequency));
}

void
update_frequency(const char *str) {
   assert(strlen(str) <= MESSAGE_MAX);
   for (int i = 0; i < strlen(str); i++) {
      frequency[i][(unsigned int)str[i]] += 1;
   }
}

const char *
most_frequent(void) {
   char *res = malloc(MESSAGE_MAX+1);
   memset(res, 0, MESSAGE_MAX+1);
   for (int i = 0; i < MESSAGE_MAX; i++) {
      int mx = 0;
      for (int j = 1; j < 256; j++) {
         if (frequency[i][j] > frequency[i][mx]) {
            mx = j;
         }
      }
      res[i] = mx;
   }
   return res;
}

const char *
least_frequent(void) {
   char *res = malloc(MESSAGE_MAX+1);
   memset(res, 0, MESSAGE_MAX+1);
   for (int i = 0; i < MESSAGE_MAX; i++) {
      int mx = 0;
      for (int j = 1; j < 256; j++) {
         if (frequency[i][j] && mx == 0) {
            mx = j;
         }
         if (frequency[i][j] && frequency[i][j] < frequency[i][mx]) {
            mx = j;
         }
      }
      res[i] = mx;
   }
   return res;
}

/*
 * part one:
 *
 */

int
part_one(
   const char *fname
) {

   FILE *ifile = fopen(fname, "r");
   if (!ifile) {
      fprintf(stderr, "error: could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }

   char iline[INPUT_LINE_MAX];

   clear_frequency();
   while (fgets(iline, INPUT_LINE_MAX - 1, ifile)) {
      update_frequency(iline);
   }
   const char *res = most_frequent();

   printf("part one: %s\n", res);

   free((void *)res);

   fclose(ifile);
   return EXIT_SUCCESS;
}


/*
 * part two:
 *
 */

int
part_two(
   const char *fname
) {
   FILE *ifile = fopen(fname, "r");
   if (!ifile) {
      fprintf(stderr, "error: could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }

   char iline[INPUT_LINE_MAX];

   clear_frequency();
   while (fgets(iline, INPUT_LINE_MAX - 1, ifile)) {
      update_frequency(iline);
   }
   const char *res = least_frequent();

   printf("part two: %s\n", res);

   free((void *)res);

   fclose(ifile);
   return EXIT_SUCCESS;
}
