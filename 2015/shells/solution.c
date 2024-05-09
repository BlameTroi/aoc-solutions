/* solution.c -- aoc 2015 07 -- troy brumley */

#include "solution.h"

#include <stdio.h>
#include <stdlib.h>


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

   while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
   }

   printf("part one: %d\n", 0);

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
