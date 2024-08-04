/* solution.c -- aoc 2016 11 -- troy brumley */

/*
 * the problem is a different spin on towers of hanoi i think. move
 * things around between four floors to get them all to the top floor
 * while honoring restrictions (a match rule instead of size
 * ordering).
 *
 * actually the class of problems is 'river crossing'.
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
 *
 * conclusions on part one:
 *
 * i and most everyone i've seen mention it on reddit or the web
 * regard this as a bitch of a problem to compute. there are so many
 * paths that if you don't figure out how to prune the search it just
 * takes too long.
 *
 * like hours or days depending on your system and language. many found
 * it quicker to write a 'game' interface and solve the problem manually.
 *
 * i tripped myself up a few times with byte ordering when i was working
 * through this, which slowed me down considerably, but even if i'd done
 * that right from the start i don't know that i would have figured out
 * the trick--which is that you don't need to compare every item to
 * prune duplicate paths from the search. the 'elements' of the two
 * kinds of only matters so long as the chip-generator constraint is
 * satisfied.
 */

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

#define TXBMISC_IMPLEMENTATION
#include "txbmisc.h"

#define TXBQU_IMPLEMENTATION
#include "txbqu.h"

/*
 * the number of distinct elements in the problem is the key
 * information. there are two in the illustrative sample and five in
 * the live data (see solution.h defines). each element will have
 * corresponding microchips and generators.
 *
 * num_elements will restrict the number of elements we check so we
 * don't waste cycles on impossible configurations.
 */

static int num_elements = 0;
static uint8_t all_elements = 0;

/*
 * avoid chasing duplicate paths. if we have already seen a particular
 * configuration, skip it. as long as a move state is legal, we only
 * care about the number of items on each floor for checking. at least
 * that's the reddit hive mind wisdom.
 */

static dupcheck *duplicates = NULL;
static int num_duplicates = 0;
static int duplicates_skipped = 0;
#define DUPLICATES_MAX 10000

void
init_duplicates(int size) {
   if (duplicates) {
      free(duplicates);
      duplicates = NULL;
      num_duplicates = 0;
   }
   duplicates = malloc((1 + size) * sizeof(dupcheck));
   memset(duplicates, 0, (1 + size) * sizeof(dupcheck));
   num_duplicates = 0;
   duplicates_skipped = 0;
}

dupcheck
state_as_dupcheck(
   state *m
) {
   dupcheck dm;
   memset(&dm, 0, sizeof(dupcheck));
   dm.elevator = m->elevator;
   dm.counts[f4][0] = one_bits_in(m->microchips[f4]);
   dm.counts[f4][1] = one_bits_in(m->generators[f4]);
   dm.counts[f3][0] = one_bits_in(m->microchips[f3]);
   dm.counts[f3][1] = one_bits_in(m->generators[f3]);
   dm.counts[f2][0] = one_bits_in(m->microchips[f2]);
   dm.counts[f2][1] = one_bits_in(m->generators[f2]);
   dm.counts[f1][0] = one_bits_in(m->microchips[f1]);
   dm.counts[f1][1] = one_bits_in(m->generators[f1]);
   return dm;
}

void
add_duplicate(state *m) {
   duplicates[num_duplicates] = state_as_dupcheck(m);
   num_duplicates += 1;
}

bool
is_duplicate(state *m) {
   dupcheck dm = state_as_dupcheck(m);
   for (int i = 0; i < num_duplicates; i++) {
      if (memcmp(&duplicates[i], &dm, sizeof(dupcheck)) == 0) {
         return true;
      }
   }
   return false;
}

/*
 * trace output support.
 */

char *
trace_line(
   state *m,
   char *buffer,
   int maxlen
) {
   snprintf(
      buffer, maxlen,
      "%c%c  %5d  %c[%02X %02X]  %c[%02X %02X]  %c[%02X %02X]  %c[%02X %02X]",
      is_goal(m) ? 'G' : ' ',
      is_valid(m) ? ' ' : 'E',
      m->no,
      m->elevator == f1 ? '>' : ' ', m->microchips[f1], m->generators[f1],
      m->elevator == f2 ? '>' : ' ', m->microchips[f2], m->generators[f2],
      m->elevator == f3 ? '>' : ' ', m->microchips[f3], m->generators[f3],
      m->elevator == f4 ? '>' : ' ', m->microchips[f4], m->generators[f4]
   );
   return buffer;
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
is_valid(state *m) {
   for (int f = f1; f <= f4; f++) {
      if (m->generators[f] != 0 &&
            (m->microchips[f] & m->generators[f]) != m->microchips[f]) {
         return false;
      }
   }
   return true;
}

/*
 * the desired end is everything on the fourth floor.
 */

bool
is_goal(
   state *m
) {
   return m->elevator == f4 &&
          m->microchips[f4] != 0 && m->generators[f4] != 0 &&
          m->microchips[f3] == 0 && m->generators[f3] == 0 &&
          m->microchips[f2] == 0 && m->generators[f2] == 0 &&
          m->microchips[f1] == 0 && m->generators[f1] == 0;
}

/*
 * from a given valid start state, provide a queue of possible valid
 * next states. caller is responsible for deallocating the returned
 * queue via qu_destroy().
 *
 * candidate moves are created by cloning the current state and then
 * making all the possible moves of one and two items from that
 * state.
 */

/* small helpers */
static state *
move_up(state *m, uint8_t microchips, uint8_t generators) {
   state *nm = malloc(sizeof(*nm));
   memcpy(nm, m, sizeof(*nm));
   nm->no += 1;
   nm->elevator += 1;
   nm->microchips[nm->elevator] |= microchips;
   nm->generators[nm->elevator] |= generators;
   nm->microchips[m->elevator] ^= microchips;
   nm->generators[m->elevator] ^= generators;
   return nm;
}
static state *
move_down(state *m, uint8_t microchips, uint8_t generators) {
   state *nm = malloc(sizeof(*nm));
   memcpy(nm, m, sizeof(*nm));
   nm->no += 1;
   nm->elevator -= 1;
   nm->microchips[nm->elevator] |= microchips;
   nm->generators[nm->elevator] |= generators;
   nm->microchips[m->elevator] ^= microchips;
   nm->generators[m->elevator] ^= generators;
   return nm;
}

qucb *
next_moves(state *m) {
   assert(is_valid(m));
   qucb *q = qu_create();

   /* moves are restricted to one floor in any legal direction. */
   int v = m->elevator;
   bool can_go_up = v < f4;
   bool can_go_down = v > f1;

   /* how many of each type are on the current floor? */
   int chips = one_bits_in(m->microchips[v]);
   int gens = one_bits_in(m->generators[v]);

   /* new move buffer. */
   state *nm = NULL;

   /* /\* i did the "no iteration" cases first for testing, and am leaving */
   /*  * them in even though the iteration would also get the job done. */
   /*  * */
   /*  * if there's only one item on the floor, or one of each kind, */
   /*  * there's no need to do anything more so there are early return */
   /*  * points. */
   /*  * */
   /*  * each return has an assert that at least one valid move was */
   /*  * created. *\/ */

   /* /\* if there's just one microchip on the floor, move it *\/ */

   /* if (chips == 1) { */
   /*    if (can_go_up) { */
   /*       nm = move_up(m, m->microchips[v], 0); */
   /*       if (is_valid(nm)) { */
   /*          qu_enqueue(q, nm); */
   /*       } else { */
   /*          free(nm); */
   /*       } */
   /*       nm = NULL; */
   /*    } */
   /*    if (can_go_down) { */
   /*       nm = move_down(m, m->microchips[v], 0); */
   /*       if (is_valid(nm)) { */
   /*          qu_enqueue(q, nm); */
   /*       } else { */
   /*          free(nm); */
   /*       } */
   /*       nm = NULL; */
   /*    } */
   /*    /\* microchip without generator, we're done. *\/ */
   /*    if (gens == 0) { */
   /*       assert(!qu_empty(q)); */
   /*       return q; */
   /*    } */
   /* } */

   /* /\* as with the lone microchip, so with the lone generator. *\/ */

   /* if (gens == 1) { */
   /*    if (can_go_up) { */
   /*       nm = move_up(m, 0, m->generators[v]); */
   /*       if (is_valid(nm)) { */
   /*          qu_enqueue(q, nm); */
   /*       } else { */
   /*          free(nm); */
   /*       } */
   /*       nm = NULL; */
   /*    } */
   /*    if (can_go_down) { */
   /*       nm = move_down(m, 0, m->generators[v]); */
   /*       if (is_valid(nm)) { */
   /*          qu_enqueue(q, nm); */
   /*       } else { */
   /*          free(nm); */
   /*       } */
   /*       nm = NULL; */
   /*    } */
   /*    /\* generator without microchip, we're done. *\/ */
   /*    if (chips == 0) { */
   /*       assert(!qu_empty(q)); */
   /*       return q; */
   /*    } */
   /* } */

   /* /\* if we had one of each, also move them together. *\/ */

   /* if (chips == 1 && gens == 1) { */
   /*    if (can_go_up) { */
   /*       nm = move_up(m, m->microchips[v], m->generators[v]); */
   /*       if (is_valid(nm)) { */
   /*          qu_enqueue(q, nm); */
   /*       } else { */
   /*          free(nm); */
   /*       } */
   /*       nm = NULL; */
   /*    } */
   /*    if (can_go_down) { */
   /*       nm = move_down(m, m->microchips[v], m->generators[v]); */
   /*       if (is_valid(nm)) { */
   /*          qu_enqueue(q, nm); */
   /*       } else { */
   /*          free(nm); */
   /*       } */
   /*       nm = NULL; */
   /*    } */
   /*    /\* and there is nothing left to do so return. *\/ */
   /*    assert(!qu_empty(q)); */
   /*    return q; */
   /* } */

   /* otherwise there's some looping to do */

   uint8_t generators = m->generators[v];
   uint8_t microchips = m->microchips[v];

   uint8_t this_element = 0;
   uint8_t this_num = 0;
   uint8_t other_num = 0;
   uint8_t other_element = 0;

   while (this_num < num_elements) {
      this_element = 1 << this_num;

      /* is there a generator for this element? */
      if (generators & this_element) {

         /* yes, first move only it. */
         if (can_go_up) {
            nm = move_up(m, 0, this_element);
            if (is_valid(nm)) {
               qu_enqueue(q, nm);
            } else {
               free(nm);
            }
            nm = NULL;
         }
         if (can_go_down) {
            nm = move_down(m, 0, this_element);
            if (is_valid(nm)) {
               qu_enqueue(q, nm);
            } else {
               free(nm);
            }
            nm = NULL;
         }

         /* now move it and one of everything else on the floor */
         other_num = 0;

         while (other_num < num_elements) {
            other_element = 1 << other_num;

            /* if there's a generator for the other element element, move it and
             * this one. there will be an extra move created but it will be
             * a duplicate and ignored. */

            if (generators & other_element) {
               if (can_go_up) {
                  nm = move_up(m, 0, this_element | other_element);
                  if (is_valid(nm)) {
                     qu_enqueue(q, nm);
                  } else {
                     free(nm);
                  }
                  nm = NULL;
               }
               if (can_go_down) {
                  nm = move_down(m, 0, this_element | other_element);
                  if (is_valid(nm)) {
                     qu_enqueue(q, nm);
                  } else {
                     free(nm);
                  }
                  nm = NULL;
               }
            }

            /* and now move the generator and any microchip. */

            if (microchips & other_element) {
               if (can_go_up) {
                  nm = move_up(m, other_element, this_element);
                  if (is_valid(nm)) {
                     qu_enqueue(q, nm);
                  } else {
                     free(nm);
                  }
                  nm = NULL;
               }
               if (can_go_down) {
                  nm = move_down(m, other_element, this_element);
                  if (is_valid(nm)) {
                     qu_enqueue(q, nm);
                  } else {
                     free(nm);
                  }
                  nm = NULL;
               }
            }
            other_num += 1;
         }
      }

      /* and now do all that stuff above from a microchip perspective.
       * so, is there a microchip for this element? */

      if (microchips & this_element) {
         /* yes, first move only it. */
         if (can_go_up) {
            nm = move_up(m, this_element, 0);
            if (is_valid(nm)) {
               qu_enqueue(q, nm);
            } else {
               free(nm);
            }
            nm = NULL;
         }
         if (can_go_down) {
            nm = move_down(m, this_element, 0);
            if (is_valid(nm)) {
               qu_enqueue(q, nm);
            } else {
               free(nm);
            }
            nm = NULL;
         }

         /* now move it and one of everything else on the floor */
         other_num = 0;

         while (other_num < num_elements) {
            other_element = 1 << other_num;

            /* if there's a generator for the other element element, move it
             * and this microchip. */

            if (generators & other_element) {
               if (can_go_up) {
                  nm = move_up(m, this_element, other_element);
                  if (is_valid(nm)) {
                     qu_enqueue(q, nm);
                  } else {
                     free(nm);
                  }
                  nm = NULL;
               }
               if (can_go_down) {
                  nm = move_down(m, this_element, other_element);
                  if (is_valid(nm)) {
                     qu_enqueue(q, nm);
                  } else {
                     free(nm);
                  }
                  nm = NULL;
               }
            }

            /* and now move the generator and any microchip. as with the
             * generator way above, a duplicate will appear here but it
             * will be pruned later. */

            if (microchips & other_element) {
               if (can_go_up) {
                  nm = move_up(m, other_element | this_element, 0);
                  if (is_valid(nm)) {
                     qu_enqueue(q, nm);
                  } else {
                     free(nm);
                  }
                  nm = NULL;
               }
               if (can_go_down) {
                  nm = move_down(m, other_element | this_element, 0);
                  if (is_valid(nm)) {
                     qu_enqueue(q, nm);
                  } else {
                     free(nm);
                  }
                  nm = NULL;
               }
            }
            other_num += 1;
         }
      }

      this_num += 1;
   }

   assert(!qu_empty(q));
   return q;
}

/*
 * seek a path to the goal state of everything on the fourth floor.
 *
 * seeking along a path is pruned if we come to an already tried
 * configuration,
 */

int
seek_goal(
   state *start
) {

   qucb *trials = qu_create();
   init_duplicates(DUPLICATES_MAX); /* wastefully large */

   state *copy = malloc(sizeof(state));
   memcpy(copy, start, sizeof(state));

   qu_enqueue(trials, copy);
   add_duplicate(copy);

   char buffer[256];
   memset(buffer, 0, 256);

   state *m = NULL;
   while (!qu_empty(trials)) {
      m = qu_dequeue(trials);
      printf("%s\n", trace_line(m, buffer, 255));
      if (is_goal(m)) {
         printf("\nGOAL\n");
         printf("num_duplicates %d\n", num_duplicates);
         printf("duplicates_skipped %d\n", duplicates_skipped);
         /* TODO leak and reporting */
         break;
      }
      qucb *possibilities = next_moves(m);
      while (!qu_empty(possibilities)) {
         state *next = qu_dequeue(possibilities);
         if (is_duplicate(next)) {
            duplicates_skipped += 1;
            free(next);
            next = NULL;
         } else {
            add_duplicate(next);
            qu_enqueue(trials, next);
            next = NULL;
         }
      }
      qu_destroy(possibilities);
   }

   return m->no;
}

/*
 * use either the live problem data or the sample.
 */

void
init_and_load(
   state *m,
   bool use_live
) {

   memset(m, 0, sizeof(*m));
   m->no = 0;
   m->elevator = f1;

   /* for the live run, five elements */

   if (use_live) {
      /*  1 elv prg prm
          2     cog cug rug plg
          3     com cum rum plm
          4 nil */
      m->generators[f1] = promethium;
      m->microchips[f1] = promethium;
      m->generators[f2] = cobalt | curium | ruthenium | plutonium;
      m->microchips[f3] = cobalt | curium | ruthenium | plutonium;
      all_elements = promethium | cobalt | curium | ruthenium | plutonium;
      num_elements = LIVE_ELEMENTS;
   }

   /* for the exmaple run from the site, two elements */

   if (!use_live) {
      /*  1 elv hym lim
          2     hyg
          3     lig
          4 nil */
      m->microchips[f1] = hydrogen | lithium;
      m->generators[f2] = hydrogen;
      m->generators[f3] = lithium;
      all_elements = hydrogen | lithium;
      num_elements = SAMPLE_ELEMENTS;
   }
}

/*
 * part one:
 */

int
part_one(
   const char *fname
) {

   state start;
   init_and_load(&start, true);
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
