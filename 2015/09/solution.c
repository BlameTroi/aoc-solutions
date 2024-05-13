/* solution.c -- aoc 2015 07 -- troy brumley */


#include <stdio.h>
#include <stdlib.h>

#define TXBMISC_H_IMPLEMENTATION
#include "txbmisc.h"
#define TXBSTR_H_IMPLEMENTATION
#include "txbstr.h"
#define TXBPMUTE_H_IMPLEMENTATION
#include "txbpmute.h"

#include "solution.h"

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
