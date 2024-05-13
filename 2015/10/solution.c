/* solution.c -- aoc 2015 10 -- troy brumley */


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

#define TXBMISC_H_IMPLEMENTATION
#include "txbmisc.h"
#define TXBSTR_H_IMPLEMENTATION
#include "txbstr.h"

/*
 * how many copies of a character start at position c?
 */

int
runlen(char *c) {
   char *p = c;
   while (*p == *c) {
      p += 1;
   }
   return p - c;
}


/*
 * how many decimal digits are required in the output buffer for
 * this number?
 */

int
digitsNeeded(int n) {
   assert(n > 0 && n < 1000);
   if (n < 10) {
      return 1;
   }
   if (n < 100) {
      return 2;
   }
   return 3;
}


/*
 * seesay -- see the digits and say the digits
 *
 * a look ahead reading of a string of digits:
 *
 * - for any lone digit, prefix it with '1'.
 * - for a run of digits, collapse into one digit and prefix with
 *   the repeat count.
 *
 * 11 -> 21, 222 -> 32, etc.
 *
 * returns an allocated string at least large enough to hold the
 * result string.
 */

char *
seesay(char *s) {

   /* safety first */
   assert(s);
   int n = strlen(s);
   assert(n);
   for (int i = 0; i < n; i++) {
      assert(s[i] >= '0' && s[i] <= '9');
   }

   /* allocate our output buffer in chunks of */
   const int chunklen = 256;
   int m = min(n * 2, chunklen);
   char *r = malloc(m);

   n = 0;    /* position in output buffer */
   while (*s) {
      int rl = runlen(s);
      int dn = digitsNeeded(rl);

      /* grow buffer if needed */
      if (n + 1 + dn > m - 2) {
         m = m + chunklen;
         r = realloc(r, m);
      }

      /* store run length and then the value */
      snprintf(r + n, dn + 1, "%d", rl);
      n += dn;
      *(r + n) = *s;
      n += 1;
      *(r + n) = '\0';
      s += rl;
   }

   return r;
}


/*
 * part one:
 *
 */

int
partOne(char *fname) {

   FILE *ifile = fopen(fname, "r");
   if (!ifile) {
      printf("could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }

   /* input is one line, our starting string and a repeat count */

   char iline[INPUT_LEN_MAX];

   if (!fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
      printf("error reading input\n.");
      return EXIT_FAILURE;
   }

   char **list = splitString(iline, " ");
   char *see = strdup(list[1]);
   int n = strtol(list[2], NULL, 10);
   char *say;
   while (n) {
      say = seesay(see);
      free(see);
      see = say;
      n -= 1;
   }

   printf("part one: %d\n", (int)strlen(say));

   free(say);
   free(list[0]);
   free(list);

   fclose(ifile);
   return EXIT_SUCCESS;
}


/*
 * part two:
 *
 */

int
partTwo(char *fname) {
   FILE *ifile;

   ifile = fopen(fname, "r");
   if (!ifile) {
      printf("could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }
   char iline[INPUT_LEN_MAX];

   if (!fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
      printf("error reading input\n.");
      return EXIT_FAILURE;
   }

   char **list = splitString(iline, " ");
   char *see = strdup(list[1]);
   int n = strtol(list[2], NULL, 10);
   /* for part two, increase repetitions by 10 */
   n += 10;
   char *say;
   while (n) {
      say = seesay(see);
      free(see);
      see = say;
      n -= 1;
   }

   printf("part two: %d\n", (int)strlen(say));

   free(say);
   free(list[0]);
   free(list);

   fclose(ifile);
   return EXIT_SUCCESS;
}
