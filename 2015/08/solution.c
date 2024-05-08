/* solution.c -- aoc 2015 06 -- troy brumley */

#include "solution.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


/* how long is the input line as a compiled string */

int
compiledStringLength(char *c) {
   int i = 0;

   /* skip the opening quote */
   if (*c == '"') {
      c++;
   }

   /* count each character up until ending \0, then trim that last
      quote off the count as well. */
   while(*c) {
      i += 1;
      switch (*c) {
      case '\\':
         c += 1;
         switch (*c) {
         case '\\':
         case '\"':
            c += 1;
            break;
         case 'x':
            c += 3;
            break;
         default:
            assert(NULL);
         }
         break;
      default:
         c += 1;
      }
   }

   /* is the prior character a quote or newline? */
   if (*(c-1) == '\n') {
      i -= 1;
      c -= 1;
   }
   if (*(c-1) == '"') {
      i -= 1;
   }

   return i;
}


/* how long is the text in code */

int
sourceStringLength(char *c) {
   int i = 0;
   while (*c) {
      switch (*c) {
      case ' ':
      case '\n':
         break;
      default:
         i += 1;
      }
      c += 1;
   }

   return i;
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

   char iline[INPUT_LEN_MAX];

   int compiledSize = 0;
   int sourceSize = 0;
   while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
      compiledSize += compiledStringLength(iline);
      sourceSize += sourceStringLength(iline);
   }

   printf("source size: %d\n", sourceSize);
   printf("compiled size: %d\n", compiledSize);
   printf("part one: %d\n", sourceSize - compiledSize);

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

   while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
   }

   printf("part two: %d\n", 0);

   fclose(ifile);
   return EXIT_SUCCESS;
}
