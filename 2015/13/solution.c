/* solution.c -- aoc 2015 13 -- troy brumley */

/* another travelling salesman problem, but this time the
   scoring is bidirectional. */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TXBMISC_H_IMPLEMENTATION
#include "txbmisc.h"
#define TXBPMUTE_H_IMPLEMENTATION
#include "txbpmute.h"
#define TXBSTR_H_IMPLEMENTATION
#include "txbstr.h"

#include "solution.h"

int
evaluate(int[]);

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

   reset_state(0);

   char iline[INPUT_LEN_MAX];

   while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
      parse_line(iline);
   }

   int path[NODES_MAX];
   for (int i = 0; i < node_count; i++) {
      path[i] = i;
   }

   int max_happiness = evaluate(path);
   while (permute_next(node_count, path)) {
      max_happiness = max(max_happiness, evaluate(path));
   }

   printf("part one: %d\n", max_happiness);

   reset_state(1);
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

   reset_state(0); /* should have been properly reset by part one, but ... */

   char iline[INPUT_LEN_MAX];

   while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
      parse_line(iline);
   }

   /* for part two, we add ourself to the seating arrangement but all the
      happiness deltas between you and everyone else is 0. so, add a row
      and column for yusef, the table has room, and run with the process
      again with. it seems this should be no change, but there is. */

   node_index("yusef");

   int path[NODES_MAX];
   for (int i = 0; i < node_count; i++) {
      path[i] = i;
   }

   int max_happiness = evaluate(path);
   while (permute_next(node_count, path)) {
      max_happiness = max(max_happiness, evaluate(path));
   }

   printf("part two: %d\n", max_happiness);

   reset_state(1);
   fclose(ifile);
   return EXIT_SUCCESS;
}


/*
 * initialize for a new run. rel is a boolean to signal that any
 * storage allocated to node_names should be released.
 */

void
reset_state(int rel) {
   node_count = 0;
   for (int i = 0; i < NODES_MAX; i++) {
      if (rel && node_names[i]) {
         free(node_names[i]);
      }
      node_names[i] = NULL;
      memset(node_delta[i], 0, sizeof(node_delta[i]));
   }
}


/* add a new node name to the list of names. should only be called
   from node_index. does no checking beyond an assertion that the list
   is not overflowed. the node name stored is a string duplicate of
   the name passed, no restrictions are placed on the value passed,
   but you should be sure to free the string if you clear the list. */

int
add_node_name(char *s) {
   assert(node_count <= NODES_MAX);
   node_names[node_count] = strdup(s);
   node_count += 1;
   return node_count - 1;
}


/* return the index of a name in the list of node names. if the
   node does not yet exist, it is added to the list. this function
   assumes it is called as part of the load and parse process. */

int
node_index(char *s) {
   if (node_count == 0) {
      return add_node_name(s);
   }
   for (int i = 0; i < node_count; i++) {
      if (strcmp(node_names[i], s) == 0) {
         return i;
      }
   }
   return add_node_name(s);
}


/*
 * parse a line of input to identify two node names and the happiness
 * delta. this is bi-directional, so we only load the from-to values.
 *
 * V           V    V                                     V
 * 1     2     3    4  5         6     7  8       9    10 11
 * alice would gain 54 happiness units by sitting next to bob.
 * bob would gain 83 happiness units by sitting next to alice.
 *
 * from_name, to_name, delta
 */

void
parse_line(char *iline) {
   const int from_tok = 1;
   const int to_tok = 11;
   const int delta_tok = 4;
   const int dir_tok = 3;

   char *s = strdup(iline);
   char **t = splitString(s, " .\n");

   int idx_from = node_index(t[from_tok]);
   int idx_to = node_index(t[to_tok]);
   node_delta[idx_from][idx_to] = strtol(t[delta_tok], NULL, 10);
   if (strcmp(t[dir_tok], "lose") == 0) {
      node_delta[idx_from][idx_to] = -node_delta[idx_from][idx_to];
   }

   /* clean up and return */
   free(t[0]);
   free(t);
}

/*
 * evaluate the overall joy and happiness of each seating arrangement. as
 * this is bidirectional, work forward and backwards through an iteration.
 */

int
evaluate(int p[]) {
   int r = 0;

   /* forward */
   for (int i = 0; i < node_count - 1; i++) {
      r += node_delta[p[i]][p[i+1]];
   }
   r += node_delta[p[node_count-1]][p[0]];

   /* backward */
   for (int i = 0; i < node_count - 1; i++) {
      r += node_delta[p[i+1]][p[i]];
   }
   r += node_delta[p[0]][p[node_count-1]];

   return r;
}
