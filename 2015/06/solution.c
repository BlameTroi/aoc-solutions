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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * the day's real code.
 */

/* part one: */
int
partOne(char *fname) {
   printf("part one\n");
   return EXIT_SUCCESS;
}

/* part two: */
int
partTwo(char *fname) {
   printf("part two\n");
   return EXIT_SUCCESS;
}


/* create a grid of lights: */
lights_t *initGrid(void) {
   lights_t *g = calloc(sizeof(lights_t), 1);
   assert(g);
   return g;
}

/* release the grid of lights: */
void
freeGrid(lights_t *g) {
   free(g);
}

/*
 * parse the input command:
 *
 * (turn on|turn off|toggle) x0,y0 through x1,y1
 *
 * if the cmd_t.cmd is invalid, some error was found and
 * the block is not usable. more in depth diagnostics are
 * not needed for advent of code type work.
 */

cmd_t
parseCmd(char *iline, int len) {

   /* fill the shell */
   cmd_t cmd;                          /* our answer */
   cmd_e hold = invalid;               /* the command, plugged in at end */
   cmd.cmd = invalid;                  /* build invalid shell */
   cmd.p0.x = 0;
   cmd.p0.y = 0;
   cmd.p1.x = 0;
   cmd.p1.y = 0;

   /* identify command */
   char *pos = NULL;                   /* start position in string for scans */
   char *nxt = NULL;                   /* end pos of scan */
   const char *ton = "turn on ";       /* command prefixes */
   const char *toff = "turn off ";
   const char *togg = "toggle ";
   const char *tthr = "through ";      /* delimiter between coordinates */

   /* check prefix and determine command type. if at any time we have
      an error, just return with the cmd code set to invalid. */

   /* length of prefix determins the start of the first coordinate pair. */
   if (strncmp(iline, ton, strlen(ton)) == 0) {
      pos = iline+strlen(ton);
      hold = on;
   } else if (strncmp(iline, toff, strlen(toff)) == 0) {
      pos = iline+strlen(toff);
      hold = off;
   } else if (strncmp(iline, togg, strlen(togg)) == 0) {
      pos = iline+strlen(togg);
      hold = toggle;
   } else {
      /* not recognized */
      return cmd;
   }

   /* get the coordinates, exit early with an invalid block if any parse
      errors occur. */

   /* first corner */
   cmd.p0.x = strtol(pos, &nxt, 10);
   if (pos == nxt || *nxt != ',') {
      return cmd;
   }
   pos = nxt + 1;
   cmd.p0.y = strtol(pos, &nxt, 10);
   if (pos == nxt || *nxt != ' ') {
      return cmd;
   }
   pos = nxt + 1;

   /* delimiter between coordinate pairs is " through ", return invalid
      if it isn't found. */
   if (strncmp(pos, tthr, strlen(tthr)) != 0) {
      return cmd;
   }

   /* second corner */
   pos = pos + strlen(tthr);
   cmd.p1.x = strtol(pos, &nxt, 10);
   if (pos == nxt || *nxt != ',') {
      return cmd;
   }
   pos = nxt + 1;
   cmd.p1.y = strtol(pos, &nxt, 10);
   if (pos == nxt || *nxt != '\n') {
      return cmd;
   }

   /* all fields processed, mark command as valid with the */
   /* command code and return. */
   cmd.cmd = hold;
   return cmd;
}
