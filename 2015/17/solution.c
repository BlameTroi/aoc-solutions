/* solution.c -- aoc 2015 17 -- troy brumley */

/*
 * given an assortment of containers and a volume to store, how many
 * combinations will store exactly that volume. for part two, find
 * the minimum number of containers possible and how many combinations
 * of that many containers exist.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"


void
reset_state(int rel) {
   if (rel) {
      /* not used for this day */
   }
   num_containers = 0;
   target_volume = 0;
   memset(containers, 0, sizeof(containers));
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
   reset_state(0);

   while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
      assert(num_containers < CONTAINER_MAX);
      if (!target_volume) {
         target_volume = strtol(iline, NULL, 10);
      } else {
         containers[num_containers] = strtol(iline, NULL, 10);
         num_containers += 1;
      }
   }

   /* either or combinations like this are counting in binary */

   unsigned int combinator = (1 << num_containers) - 1;
   printf("%u possible combinations\n", combinator);

   int combinations = 0;

   int how_many_combinations[num_containers];

   memset(how_many_combinations, 0, sizeof(how_many_combinations));

   /* counting down through the possible combinations until
      we hit zero */

   while (combinator != 0) {

      /* shift right to left through the current combination
         identifying containers to use (bit 1). */
      unsigned int shifter = 1;

      /* as a container is used, its bit is turned off in
         this tracker. if we run out of volume and there
         are non-zero bits left, the combination is not
         a solution. */

      unsigned int tracker = combinator;

      int i = 0;
      int j = 0;

      int left = target_volume;
      while (shifter < combinator && left > 0) {

         /* if this container is available, use it and track it */
         if (shifter & combinator) {
            tracker ^= shifter;
            left -= containers[i];
            j += 1;
         }

         i += 1;
         shifter <<= 1;
      }

      /* if we used the exact amount and no containers are left,
         it's a valid combination. */

      if (left == 0 && tracker == 0) {
         combinations += 1;
         /* j is post incremented so -1 here */
         how_many_combinations[j-1] += 1;
      }

      /* next trial combination */
      combinator -= 1;
   }

   printf("part one: %d\n", combinations);

   for (int i = 0; i < num_containers; i++) {
      if (how_many_combinations[i]) {
         printf("part two: %d\n", how_many_combinations[i]);
         break;
      }
   }
   reset_state(1);
   fclose(ifile);
   return EXIT_SUCCESS;
}


/*
 * part two is so interleaved with part one that i kept it
 * all there.:
 *
 */

int
partTwo(char *fname) {
   /*   FILE *ifile;

      ifile = fopen(fname, "r");
      if (!ifile) {
         printf("could not open file: %s\n", fname);
         return EXIT_FAILURE;
      }
      char iline[INPUT_LEN_MAX];

      while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
      }

      printf("part two: %d\n", 0);

      fclose(ifile); */
   return EXIT_SUCCESS;
}
