/* solution.c -- aoc 2016 11 -- troy brumley */

/*
 * the problem is a different spin on towers of hanoi i think. move
 * things around between four floors to get them all to the top floor
 * while honoring restrictions (a match rule instead of size
 * ordering).
 *
 * what is the fewest number of moves required to get all parts to
 * the top floor of a four floor fabrication lab?
 *
 * the input dataset is several somewhat regular sentences but rather
 * than write a parser, i sketched the layout and codified it in
 * init_and_load() which sets the initial state for both the sample
 * from the problem description and the live data.
 *
 * live run: (??? steps from this)
 *
 *  1 elv prg prm
 *  2     cog cug rug plg
 *  3     com cum rum plm
 *  4 nil
 *
 * sample run: (11 steps from this)
 *
 *  1 elv hym lim
 *  2     hyg
 *  3     lig
 *  4 nil
 *
 * the rules of transport:
 *
 *  elevator will only move with one or two items.
 *
 *  elevator stops at each floor. this stop is the move counter.
 *
 * if a chip ends a move on a floor with any generators, that chip's
 * matching generator must be on the floor as well.
 *
 * so: hyg hym lig is legal,
 *
 * but: hym lig is not.
 *
 * there are only a few items and i decided to use bit masks. i don't
 * know if it will make any runtime difference, but fiddling bits is
 * something that needs to be done from time to time, so practice is
 * worthwhile.
 */

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

#define TXBMISC_IMPLEMENTATION
#include "txbmisc.h"

/*
 * a few globals
 */

/* running best result (fewest moves) seen */

int best = 0;

/* an overly large cache to remember moves already tried, no range checking */

typedef struct move_for_dupcheck move_for_dupcheck;
struct move_for_dupcheck {
   int elevator;
   int counts[LEVEL_DIM]; /* do we need to separate by gen or chip? */
};

static move_for_dupcheck attempted[100000];
static int attempts = 0;

/* all the possible bits on for generators and microchips */

element all_possible_parts = (all_as_generators | all_as_microchips);

/*
 * avoid chasing duplicate paths, if we have already seen a particular
 * configuration, skip it.
 *
 * while it seems possible to have to cycle someting past a block and
 * thus repeat, i deem it unlikely enough to clip.
 */

/*
 * compare two move states.
 */

bool
are_same(
   move *a,
   move *b
) {
   return a->elevator == b->elevator &&
          a->parts[f1] == b->parts[f1] &&
          a->parts[f2] == b->parts[f2] &&
          a->parts[f3] == b->parts[f3] &&
          a->parts[f4] == b->parts[f4];
}

/*
 * have we seen this move before? no attempt to order, just check them
 * all.
 */

bool
already_tried(move *m) {
   move_for_dupcheck mfd;
   mfd.elevator = m->elevator;
   mfd.counts[f1] = one_bits_in(m->parts[f1]);
   mfd.counts[f2] = one_bits_in(m->parts[f2]);
   mfd.counts[f3] = one_bits_in(m->parts[f3]);
   mfd.counts[f4] = one_bits_in(m->parts[f4]);
   for (int i = 0; i < attempts+1; i++) {
      move_for_dupcheck *p = &attempted[i];
      if (mfd.elevator == p->elevator &&
            mfd.counts[f1] == p->counts[f2] &&
            mfd.counts[f2] == p->counts[f2] &&
            mfd.counts[f3] == p->counts[f3] &&
            mfd.counts[f4] == p->counts[f4]) {
         return true;
      }
   }
   attempts += 1;
   attempted[attempts] = mfd;
   return false;
}

/*
 * debug print support
 */

printable printables[] = {
   { promethium, "Promethium", "PrG", "PrM" },
   { cobalt, "Cobalt", "CoG", "CoM" },
   { curium, "Curium", "CuG", "CuM" },
   { ruthenium, "Ruthenium", "RuG", "RuM" },
   { plutonium, "Plutonium", "PlG", "PlM" },
   { hydrogen, "Hydrogen", "HyG", "HyM" },
   { lithium, "Lithium", "LiG", "LiM" },
   { 0, "Error", "Err", "Err" }
};

printable *
get_printable(element e) {
   printable *p = printables;
   while (!(p->e & e)) {
      p += 1;
   }
   return p;
}

void
report(move *m) {
   printf("\nmove %d\n", m->no);
   if (is_goal(m)) {
      printf("*** goal ***\n");
   }
   printf("%s\n", is_valid(m) ? "valid" : "INVALID");
   for (int i = f1; i <= f4; i++) {
      printf(" %d |%c| ", i+1, i == m->elevator ? 'E' : ' ');
      element e = 1;
      while (e & all_elements) {
         printable *p = get_printable(e);
         if (p->e) {
            printf(" %s %s",
                   m->parts[i] & (to_generator(e)) ? p->gen : "   ",
                   m->parts[i] & (to_microchip(e)) ? p->chip : "   ");
         }
         e = e << 1;
      }
      printf("\n");
   }
   printf("\n");
}

void
report_sl(move *m, char *buffer, int maxlen) {
   snprintf(
      buffer, maxlen,
      "%3d %c %c %1d  %04X %04X %04X %04X",
      m->no,
      is_goal(m) ? 'G' : ' ',
      is_valid(m) ? ' ' : 'E',
      m->elevator+1,
      m->parts[f1], m->parts[f2], m->parts[f3], m->parts[f4]
   );
}

/*
 * is this move a valid combination?
 *
 * if a chip ends a move on a floor with any generators, that chip's
 * matching generator must be on the floor as well.
 *
 * so: hyg hym lig is legal,
 *
 * but: hym lig is not.
 */

bool
is_valid(move *m) {

   /* i want to unroll this loop. at each level if there are
    * microchips, there must be a matching generator for each
    * microchip if there are any generators. */

   for (int i = f1; i <= f4; i++) {
      if (m->parts[i] == 0) {
         continue;
      }
      element generators = from_generator((m->parts[i] & all_as_generators));
      element microchips = from_microchip((m->parts[i] & all_as_microchips));
      if (generators == 0 || microchips == 0 || generators == microchips) {
         continue;
      }
      if ((microchips & generators) != microchips) {
         return false;
      }
   }

   return true;
}

/*
 * the desired end is everything on the fourth floor.
 */

bool
is_goal(move *m) {
   return m->elevator == f4 &&
          m->parts[f4] != 0 &&
          m->parts[f1] == 0 &&
          m->parts[f2] == 0 &&
          m->parts[f3] == 0;
}

/*
 * recursively seek to reach the goal of everything on the fourth
 * floor.
 *
 * seeking along a path is pruned if we come to an already tried
 * configuration,
 *
 * seeking along a path is pruned if we have taken more steps that the
 * best attempt so far.
 *
 * move legality is checked at entry instead of before each recursive
 * call.
 *
 * returns number of moves to goal or INT_MAX for any pruning or
 * error.
 */

static int seekers = 0;

int
seek_r(
   move *m
) {
   m->no += 1;
   seekers += 1;
   int seeker = seekers;
   char buffer[255];
   memset(buffer, 0, 255);
   report_sl(m, buffer, 254);
   printf(">>>seek_r %3d %s\n", seeker, buffer);

   if (m->no > best) {
      printf("<<<seek_r %3d pruned, %d > %d\n", seeker, m->no, best);
      return INT_MAX;
   }
   if (is_goal(m)) {
      best = min(best, m->no);
      printf("<<<seek_r %3d done, goal state reached at %d\n", seeker, m->no);
      return best;
   }
   if (!is_valid(m)) {
      printf("<<<seek_r %3d pruned %d is an invalid state\n", seeker, m->no);
      return INT_MAX;
   }
   if (already_tried(m)) {
      printf("<<<seek_r %3d pruned %d is a duplicate state\n", seeker, m->no);
      return INT_MAX;
   }

   /* i don't see a bfs approach here so we'll just hit everything that
    * is possible from a valid position. */

   /* moves are restricted to one floor in any legal direction. */
   int v = m->elevator;
   bool can_go_up = v < f4;
   bool can_go_down = v > f1;
   move nm;

   int num_parts = one_bits_in(m->parts[v]);
   assert(num_parts);

   if (num_parts == 1) {
      if (can_go_up) {
         nm = *m;
         nm.elevator += 1;
         nm.parts[nm.elevator] |= nm.parts[v];
         nm.parts[v] = 0;
         best = min(best, seek_r(&nm));
      }
      if (can_go_down) {
         nm = *m;
         nm.elevator -= 1;
         nm.parts[nm.elevator] |= nm.parts[v];
         nm.parts[v] = 0;
         best = min(best, seek_r(&nm));
      }
      printf("<<<seek_r %3d %s  best: %d\n", seeker, buffer, best);
      return best;
   }

   /* more than one item on the floor, try to move every possible
    * combination of one or two items from what we have. */

   int possible_masks = 0;
   int checked_masks = 0;
   element odometer = 1;
   while (odometer) {
      if (one_bits_in(odometer) < 3) {
         possible_masks += 1;
         if ((m->parts[v] & odometer) == odometer) {
            checked_masks += 1;
            if (can_go_up) {
               nm = *m;
               nm.elevator += 1;
               nm.parts[nm.elevator] |= odometer;
               nm.parts[v] ^= odometer;
               best = min(best, seek_r(&nm));
            }
            if (can_go_down) {
               nm = *m;
               nm.elevator -= 1;
               nm.parts[nm.elevator] |= odometer;
               nm.parts[v] ^= odometer;
               best = min(best, seek_r(&nm));
            }
         }
      }
      odometer += 1;
   }

   /* printf("odometer           %04X\n", odometer); */
   /* printf("all elements       %04X\n", all_elements); */
   /* printf("all as microchips  %04X\n", all_as_microchips); */
   /* printf("all as generators  %04X\n", all_as_generators); */
   /* printf("all as parts       %04X\n", all_as_parts); */
   /* printf("all_possible_parts %04X\n", all_possible_parts); */
   /* printf("possibles          %4d\n", possible_masks); */
   /* printf("checked            %4d\n", checked_masks); */

   printf("<<<seek_r %3d %s  best: %d\n", seeker, buffer, best);

   return best;
}

int
seek_goal(
   move *start
) {
   seekers = 0;
   best = INT_MAX;
   best = seek_r(start);
   printf("seek goal = logged moves %d  result %d\n", attempts, best);

   return best;
}

/*
 * use either the live problem data or the sample.
 */

void
init_and_load(
   move *m,
   bool use_live
) {

   attempts = -1;
   memset(attempted, 0, sizeof(attempts));

   memset(m, 0, sizeof(*m));
   m->no = 0;
   m->elevator = f1;

   /* for the live run, five elements */

   if (use_live) {
      /*  1 elv prg prm
          2     cog cug rug plg
          3     com cum rum plm
          4 nil */
      m->parts[f1] = to_generator(promethium) | to_microchip(promethium);
      m->parts[f2] = to_generator(cobalt | curium | ruthenium | plutonium);
      m->parts[f3] = to_microchip(cobalt | curium | ruthenium | plutonium);
   }

   /* for the exmaple run from the site, two elements */

   if (!use_live) {
      /*  1 elv hym lim
          2     hyg
          3     lig
          4 nil */
      m->parts[f1] = to_microchip(hydrogen | lithium);
      m->parts[f2] = to_generator(hydrogen);
      m->parts[f3] = to_generator(lithium);
   }
}

/*
 * part one:
 */

int
part_one(
   const char *fname
) {

   /* using sample i got 20 as a result, which is higher than optimal, but it's
    * a start. trying live next. that fails at the moment, switching back to
    * smaple. */

   move start;
   init_and_load(&start, false);
   int res = seek_goal(&start);

   printf("part one: %d\n", res);

   /* init_and_load(&start, true); */
   /* res = seek_goal(&start); */
   /* printf("alternate %d\n", res); */

   return EXIT_SUCCESS;
}


/*
 * part two:
 */

int
part_two(
   const char *fname
) {

   printf("part two: %d\n", 0);

   return EXIT_SUCCESS;
}
/* element generator_elements = from_generator(m->parts[v] & all_as_generators); */
/* element microchip_elements = from_microchip(m->parts[v] & all_as_microchips); */
/* int num_microchips = one_bits_in(microchip_elements); */
/* int num_generators = one_bits_in(generator_elements); */

/* for one part, it's a quick check */

/* if (num_parts == 1) { */
/*    if (can_go_up) { */
/*       nm = *m; */
/*       nm.elevator += 1; */
/*       nm.parts[nm.elevator] |= nm.parts[v]; */
/*       nm.parts[v] = 0; */
/*       best = min(best, seek_r(&nm)); */
/*    } */
/*    if (can_go_down) { */
/*       nm = *m; */
/*       nm.elevator -= 1; */
/*       nm.parts[nm.elevator] |= nm.parts[v]; */
/*       nm.parts[v] = 0; */
/*       best = min(best, seek_r(&nm)); */
/*    } */
/*    /\* nothing else to do with from here so *\/ */
/*    return best; */
/* } */
