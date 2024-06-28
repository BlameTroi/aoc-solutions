/* solution.c -- aoc 2015 20 -- troy brumley */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
      printf("could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }

   char iline[INPUT_LEN_MAX];

   int target_presents = 0;
   while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
      target_presents = atol(iline);
   }
   int last_elf = target_presents / 10;
   int last_house = last_elf;
   int house[last_house];
   memset(house, 0, sizeof(house));

   int elf = 0;
   int visit = 0;
   while (elf < last_elf) {
      elf += 1;
      for (visit = elf; visit < last_house; visit += elf) {
         house[visit] += elf * 10;
      }
   }
   int i = 0;
   while (house[i] < target_presents) {
      i += 1;
   }
   printf("part one: %d\n", i);

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
      printf("could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }
   char iline[INPUT_LEN_MAX];

   int target_presents = 0;
   while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
      target_presents = atol(iline);
   }
   int last_elf = target_presents / 11;
   int last_house = last_elf;
   int house[last_house];
   memset(house, 0, sizeof(house));

   int elf = 0;
   int visit = 0;
   while (elf < last_elf) {
      elf += 1;
      visit = elf;
      int visited = 0;
      while (visit < last_house && visited < 50) {
         house[visit] += elf * 11;
         visited += 1;
         visit += elf;
      }
   }
   int i = 0;
   while (house[i] < target_presents) {
      i += 1;
   }
   printf("part two: %d\n", i);

   fclose(ifile);
   return EXIT_SUCCESS;
}
