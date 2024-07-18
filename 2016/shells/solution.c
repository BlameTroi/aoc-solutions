/* solution.c -- aoc 2016 xx -- troy brumley */

#include <stdio.h>
#include <stdlib.h>

#include "solution.h"

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
part_two(
   const char *fname
) {
   FILE *ifile;

   ifile = fopen(fname, "r");
   if (!ifile) {
      fprintf(stderr, "error: could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }
   char iline[INPUT_LEN_MAX];

   while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
   }

   printf("part two: %d\n", 0);

   fclose(ifile);
   return EXIT_SUCCESS;
}
