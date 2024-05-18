/* solution.c -- aoc 2015 15 -- troy brumley */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define TXBMISC_H_IMPLEMENTATION
#include "txbmisc.h"

#define TXBSTR_H_IMPLEMENTATION
#include "txbstr.h"

#include "solution.h"

/*
 * clear counters and such:
 */

void
reset_state(int rel) {
   if (rel) {
      for (int i = 0; i < INGREDIENTS_MAX; i++) {
         if (ingredients[i].name) {
            free(ingredients[i].name);
         }
      }
   }
   memset(ingredients, 0, sizeof(ingredients));
   max_score = 0;
   num_ingredients = 0;
}


/*
 * get ingredient attributes
 *
 * V                      V              V          V          V           V
 * 1             2        3   4          5   6      7  8       9  10       11
 * Butterscotch: capacity -1, durability -2, flavor 6, texture 3, calories 8
 *
 */

void
parse_line(char *iline) {
   assert(num_ingredients < INGREDIENTS_MAX);

   char **t = splitString(iline, " ,:\n");

   ingredient_t* g = &ingredients[num_ingredients];
   g->name = strdup(t[1]);

   g->attributes[CAP] = strtol(t[3], NULL, 10);
   g->attributes[DUR] = strtol(t[5], NULL, 10);
   g->attributes[FLA] = strtol(t[7], NULL, 10);
   g->attributes[TEX] = strtol(t[9], NULL, 10);

   g->attributes[CAL] = strtol(t[11], NULL, 10);

   num_ingredients += 1;

   free(t[0]);
   free(t);
}


/*
 * units is an array of portion sizes for the ingredients. what
 * is the score for the proposed recipe.
 */

long
score(int *units) {
   long attributes[SCOREABLE_MAX];
   memset(attributes, 0, sizeof(attributes));

   for (int i = 0; i < num_ingredients; i++) {
      for (int j = 0; j < SCOREABLE_MAX; j++) {
         attributes[j] += ingredients[i].attributes[j] * units[i];
      }
   }

   long r = 1;
   for (int j = 0; j < SCOREABLE_MAX; j++) {
      if (attributes[j] <= 0) {
         r = 0;
         break;
      }
      r *= attributes[j];
   }
   return r;
}


/*
 * how many calories does this recipe have in one cookie?
 */

long
calories(int *units) {
   long r = 0;
   for (int i = 0; i < num_ingredients;  i++) {
      r += units[i] * ingredients[i].attributes[CAL];
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

   char iline[INPUT_LEN_MAX];

   reset_state(0);
   while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
      parse_line(iline);
   }

   /* i can't get a pretty generator working, i wanted
      something that would generate possible ways of
      splitting the 100 units between the recipes and
      then permuting over the recipes, but that got too
      convoluted. this isn't sufficiently generalizable
      but it has the virtue of working. */

   int max_score = -1;
   int units[INGREDIENTS_MAX];
   const int limit = 100;
   const int limit_plus = limit + 1;
   for (int i = 0; i < limit_plus; i++) {
      units[0] = i;
      for (int j = 0; j < limit_plus-i; j++) {
         units[1] = j;
         int ij = i + j;
         for (int k = 0; k < limit_plus-ij; k++) {
            units[2] = k;
            int ijk = ij + k;
            for (int l = 0; l < limit_plus-ijk; l++) {
               if (ijk+l != limit) {
                  continue;
               }
               units[3] = l;
               int this_score = score(units);
               if (this_score) {
                  max_score = max(max_score, this_score);
               }
            }
         }
      }
   }

   reset_state(1);
   printf("part one: %d\n", max_score);

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

   reset_state(0);
   while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
      parse_line(iline);
   }

   int max_score = -1;
   int units[INGREDIENTS_MAX];
   const int limit = 100;
   const int limit_plus = limit + 1;
   const int target_calories = 500;
   for (int i = 0; i < limit_plus; i++) {
      units[0] = i;
      for (int j = 0; j < limit_plus-i; j++) {
         units[1] = j;
         int ij = i + j;
         for (int k = 0; k < limit_plus-ij; k++) {
            units[2] = k;
            int ijk = ij + k;
            for (int l = 0; l < limit_plus-ijk; l++) {
               if (ijk+l != limit) {
                  continue;
               }
               units[3] = l;
               int this_score = score(units);
               if (this_score && calories(units) == target_calories) {
                  max_score = max(max_score, this_score);
               }
            }
         }
      }
   }

   reset_state(1);

   printf("part two: %d\n", max_score);

   fclose(ifile);
   return EXIT_SUCCESS;
}
