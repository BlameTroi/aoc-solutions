/* solution.c -- aoc 2015 14 -- troy brumley */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TXBMISC_IMPLEMENTATION
#include "txbmisc.h"

#define TXBSTR_IMPLEMENTATION
#include "txbstr.h"

#include "solution.h"


/*
 * clean up/initialize
 */

void
reset_state(int rel) {
   if (rel) {
      for (int i = 0; i < RACERS_MAX; i++) {
         if (racers[i].name) {
            free(racers[i].name);
         }
      }
   }
   memset(racers, 0, sizeof(racers));
   timer = 0;
   num_racers = 0;
}



/*
 * parse input into race_trace_t
 *
 * sample input:
 *
 * V             V           V                                  V
 * 1     2   3   4  5    6   7  8        9   10   11   12   13  14  15
 * Comet can fly 14 km/s for 10 seconds, but then must rest for 127 seconds.
 *
 */

void
parse_line(
   const char *iline
) {
   assert(num_racers < RACERS_MAX);
   char *s = strdup(iline);
   const char **tokens = split_string(s, " .,\n");

   race_trace_t *r = &racers[num_racers];
   r->name = strdup(tokens[1]);
   r->speed = strtol(tokens[4], NULL, 10);
   r->burst = strtol(tokens[7], NULL, 10);
   r->rest = strtol(tokens[14], NULL, 10);

   /* start off flying */
   r->flying = 1;
   r->ticks = r->burst;

   num_racers += 1;
   free((void *)tokens[0]);
   free(tokens);
}

/*
 * run one second of clock
 */

void
iterate(void) {

   for (int i = 0; i < num_racers; i++) {
      race_trace_t *r = &racers[i];
      /* flying or resting? */
      if (r->flying) {
         r->distance += r->speed;
      }
      /* decrement state timer */
      r->ticks -= 1;
      /* state change? */
      if (r->ticks == 0) {
         /* get next state's duration */
         if (r->flying) {
            r->ticks = r->rest;
         } else {
            r->ticks = r->burst;
         }
         r->flying = !r->flying;
      }
   }

   timer += 1;
}


/*
 * run the race. for part one the race is run and the winner
 * judged by distance. for part two the winner accumulates
 * points for how often they are in the lead.
 */

int
run_race_distance(int ticks) {
   int max_distance = 0;

   for (int i = 0; i < ticks; i++) {
      iterate();
   }

   for (int i = 0; i < num_racers; i++) {
      max_distance = max(max_distance, racers[i].distance);
   }

   return max_distance;
}


int
run_race_score(int ticks) {
   int max_score = 0;

   for (int i = 0; i < ticks; i++) {
      iterate();
      int leader = 0;
      for (int j = 0; j < num_racers; j++) {
         if (racers[j].distance > racers[leader].distance) {
            leader = j;
         }
      }
      racers[leader].score += 1;
   }

   for (int i = 0; i < num_racers; i++) {
      max_score = max(max_score, racers[i].score);
   }

   /* off by one */

   return max_score;
}

/*
 * part one:
 *
 */

int
partOne(
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

   int duration = (num_racers == 2) ? 1000 : 2503;

   printf("part one: %d\n", run_race_distance(duration));

   reset_state(1);

   fclose(ifile);
   return EXIT_SUCCESS;
}


/*
 * part two:
 *
 */

int
partTwo(
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

   int duration = (num_racers == 2) ? 1000 : 2503;

   printf("part two: %d\n", run_race_score(duration));

   reset_state(1);

   fclose(ifile);
   return EXIT_SUCCESS;
}
