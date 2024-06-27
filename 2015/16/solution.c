/* solution.c -- aoc 2015 16 -- troy brumley */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TXBSTR_IMPLEMENTATION
#include "txbstr.h"

#include "solution.h"

/* map keywords to positions in the data array. this is also a
   convenient place to put comparison operators. they are
   tweaked in part two. */

map_t map[DATA_MAX] = {
   { "children", CHIL_IX, '='},
   { "cats", CATS_IX, '='},
   { "samoyeds", SAMO_IX, '=' },
   { "pomeranians", POME_IX, '=' },
   { "akitas", AKIT_IX, '=' },
   { "vizslas", VISZ_IX, '=' },
   { "goldfish", FISH_IX, '=' },
   { "trees", TREE_IX, '=' },
   { "cars", CARS_IX, '=' },
   { "perfumes", PERF_IX, '='}
};


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

   reset_state(0);
   while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
      parse_line(iline);
   }

   /* problem qualificiation, search the aunts for a match on this */

   char *ticker = "children: 3, cats: 7, samoyeds: 2, pomeranians: 3, akitas: 0, vizslas: 0, goldfish: 5, trees: 3, cars: 2, perfumes: 1\n";

   facts_t cond = parse_condition(ticker);

   printf("number of aunts that match: %d\n", satisfy_count(cond));
   int aunt_sue = satisfy_ix(cond, 0);
   printf("part one: %d\n", aunts[aunt_sue].id);

   reset_state(1);
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

   reset_state(0);
   while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
      parse_line(iline);
   }

   /* problem qualificiation, search the aunts for a match on these readings
      but now the sensor reports floors for cats and trees, and ceilings for
      pomeranians and goldfish. */

   char *ticker = "children: 3, cats: 7, samoyeds: 2, pomeranians: 3, akitas: 0, vizslas: 0, goldfish: 5, trees: 3, cars: 2, perfumes: 1\n";
   map[get_data_ix("cats")].op = '>';
   map[get_data_ix("trees")].op = '>';
   map[get_data_ix("pomeranians")].op = '<';
   map[get_data_ix("goldfish")].op = '<';

   facts_t cond = parse_condition(ticker);

   printf("number of aunts that match: %d\n", satisfy_count(cond));
   int aunt_sue = satisfy_ix(cond, 0);
   printf("part two: %d\n", aunts[aunt_sue].id);

   reset_state(1);

   fclose(ifile);
   return EXIT_SUCCESS;
}


void
print_aunt(facts_t *aunt) {
   printf("%3d: ", aunt->id);
   for (int i = 0; i < DATA_MAX; i++) {
      printf("%3d", aunt->data[i]);
   }
   printf("\n");
}


/*
 * parse an input line
 *     V   V       V   V         V  V
 * 1   2   3       4   5         6  7
 * Sue 11: akitas: 10, perfumes: 4, vizslas: 1
 *
 */

void
parse_line(
   const char *iline
) {
   assert(num_aunts < AUNT_MAX);
   const char **t = split_string(iline, " :,\n");
   facts_t *this = &aunts[num_aunts];
   this->id = strtol(t[2], NULL, 10);
   int i = 3;
   while (t[i] && strlen(t[i]) != 0) {
      this->data[get_data_ix(t[i])] = strtol(t[i+1], NULL, 10);
      i += 2;
   }
   num_aunts += 1;
   free((void *)t[0]);
   free(t);
}


/*
 * find the index for the name of a data attribute.
 */

int
get_data_ix(
   const char *tag
) {
   int n = strlen(tag);
   for (int i = 0; i < DATA_MAX; i++) {
      if (strncmp(tag, map[i].tag, n) == 0) {
         return map[i].ix;
      }
   }
   assert(0);
   return -1;
}


/*
 * create a facts entry for checking against the aunts'
 * dataset.
 *
 * iline is a string of key value pairs, "key: value,".
 */

facts_t
parse_condition(
   const char *iline
) {
   facts_t cond;
   cond.id = -1;
   for (int i = 0; i < DATA_MAX; i++) {
      cond.data[i] = NIL_VALUE;
   }

   const char **t = split_string(iline, " :,\n");

   int i = 1;
   while (t[i] && strlen(t[i]) != 0) {
      cond.data[get_data_ix(t[i])] = strtol(t[i+1], NULL, 10);
      i += 2;
   }

   free((void *)t[0]);
   free(t);

   return cond;
}


/*
 * how many aunts satisfy a set of facts?
 */

int
satisfy_count(facts_t cond) {
   int n = 0;
   for (int i = 0; i < num_aunts; i++) {
      if (satisfies(cond, i)) {
         n += 1;
      }
   }
   return n;
}


/*
 * does this aunt satisfy a set of conditions? a nil (-1) value
 * in a data point means that we can not consider this point
 * so it is ignored.
 */

bool
satisfies(facts_t cond, int ix) {
   if (ix < 0 || ix >= AUNT_MAX) {
      return false;
   }

   for (int i = 0; i < DATA_MAX; i++) {

      /* if we don't know about this attribute, skip */
      if (aunts[ix].data[i] == NIL_VALUE) {
         continue;
      }

      /* if our query value is unsure, skip */
      if (cond.data[i] == NIL_VALUE) {
         continue;
      }

      /* known value on both sides, compare */
      switch (map[i].op) {
      case '=':
         if (aunts[ix].data[i] != cond.data[i]) {
            return false;
         }
         break;
      case '<':
         if (aunts[ix].data[i] >= cond.data[i]) {
            return false;
         }
         break;
      case '>':
         if (aunts[ix].data[i] <= cond.data[i]) {
            return false;
         }
         break;
      default:
         assert(NULL);
         return false;
      }
   }

   return true;
}


/*
 * return the index of the first aunt that satisfies a set of conditions
 * starting from a particular index. returns -1 if none is found.
 */

int
satisfy_ix(facts_t cond, int start) {
   if (start < 0 || start >= num_aunts) {
      return -1;
   }
   for (int i = start; i < num_aunts; i++) {
      if (satisfies(cond, i)) {
         return i;
      }
   }
   return -1;
}


/*
 * reset storage
 */

void
reset_state(int rel) {
   if (rel) {
      /* no dynamic allocations at the moment */
   }
   num_aunts = 0;
   for (int i = 0; i < AUNT_MAX; i++) {
      aunts[i].id = NIL_VALUE;
      for (int j = 0; j < DATA_MAX; j++) {
         aunts[i].data[j] = NIL_VALUE;
      }
   }
}
