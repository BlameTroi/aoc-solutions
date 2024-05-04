/* solution.c -- aoc 2015 06 -- troy brumley */

/*
 * probably a fire hazard
 *
 * lights in a 0,0 -> 999,999 grid are turned off and on
 * as directed, how many lights are left lit at the end
 * of the run if all the lights are off at the start.
 *
 * (turn on|turn off|toggle) x0,y0 through x1,y1
 */

#include "solution.h"

#include <stdio.h>
#include <stdlib.h>

/*
 * the day's real code.
 */

/* part one: */
int
partOne(char *fname) {
   FILE *ifile;

   ifile = fopen(fname, "r");
   if (!ifile) {
      printf("could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }

   char *iline;
   size_t ilen;

   while ((iline = fgetln(ifile, &ilen))) {
   }

   printf("part one: %ld\n", 0);

   fclose(ifile);
   return EXIT_SUCCESS;
}

/* part two: */
int
partTwo(char *fname) {
   FILE *ifile;

   ifile = fopen(fname, "r");
   if (!ifile) {
      printf("could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }

   char *iline;
   size_t ilen;

   while ((iline = fgetln(ifile, &ilen))) {
   }

   printf("part two: %ld\n", 0);

   fclose(ifile);
   return EXIT_SUCCESS;
}
