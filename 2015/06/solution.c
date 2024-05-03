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

   FILE *ifile;

   ifile = fopen(fname, "r");
   if (!ifile) {
      printf("could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }

   /* line from input */
   char *iline;
   /* length, expected to have a trailing \n */
   size_t ilen;

   lights_t *g = initGrid(1);

   while ((iline = fgetln(ifile, &ilen))) {
      cmd_t c = parseCmd(iline, ilen);
      doCmd(g, c);
   }

   printf("part one: %ld\n", g->lit);

   fclose(ifile);
   return EXIT_SUCCESS;
}

/* part two: */
int
partTwo(char *fname) {
   printf("part two\n");
   return EXIT_SUCCESS;
}


/* create a grid of lights: */
lights_t *initGrid(int digital) {
   lights_t *g = calloc(sizeof(lights_t), 1);
   assert(g);
   if (digital) {
      g->digital = 1;
      g->fnon = turnOnD;
      g->fnoff = turnOffD;
      g->fntog = toggleD;
   } else {
      g->digital = 0;
      /*
      g->fnon = turnOnA;
      g->fnoff = turnOffA;
      g->fntog = toggleA;
      */
   }
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
   cmd_e hold = e_invalid;               /* the command, plugged in at end */
   cmd.cmd = e_invalid;                  /* build invalid shell */
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
      hold = e_on;
   } else if (strncmp(iline, toff, strlen(toff)) == 0) {
      pos = iline+strlen(toff);
      hold = e_off;
   } else if (strncmp(iline, togg, strlen(togg)) == 0) {
      pos = iline+strlen(togg);
      hold = e_toggle;
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

/*
 * how many lights are on?
 */
long
numberOn(lights_t *g) {
   return g->lit;
}

/*
 * what is their total intensity if analog mode?
 */
long
totalIntensity(lights_t *g) {
   return g->intensity;
}

/*
 * turn single light on, off, or toggle it.
 */
void
turnOnD(lights_t *g, coord_t p) {
   if (isLit(g, p)) {
      return;
   }
   g->bulb[p.x][p.y] = 1;
   g->lit += 1;
}

void
turnOffD(lights_t *g, coord_t p) {
   if (!isLit(g, p)) {
      return;
   }
   g->bulb[p.x][p.y] = 0;
   g->lit -= 1;
}

void
toggleD(lights_t *g, coord_t p) {
   isLit(g, p) ? g->fnoff(g, p) : g->fnon(g, p);
}


/*
 * single light's status.
 */
int
isLit(lights_t *g, coord_t p) {
   return g->bulb[p.x][p.y];
}



/*
 * by your command...
 */
int min(int a, int b) { return a < b ? a : b; }

void
doCmd(lights_t *g, cmd_t c) {

   /* get light indices in proper ordering */
   int x1 = min(c.p0.x, c.p1.x);
   int x2 = x1 == c.p0.x ? c.p1.x : c.p0.x;
   int y1 = min(c.p0.y, c.p1.y);
   int y2 = y1 == c.p0.y ? c.p1.y : c.p0.y;

   /* get function for command */
   void (*fn)(lights_t *, coord_t) = NULL;
   switch (c.cmd) {
   case e_on:
      fn = g->fnon;
      break;
   case e_off:
      fn = g->fnoff;
      break;
   case e_toggle:
      fn = g->fntog;
      break;
   case e_invalid:
      break;
   /* does nothing, should not occur, will accvio when called */
   }

   /* do whatever we need to */
   int i, j;
   for (i = x1; i <= x2; i++) {
      coord_t p = {i, -1};
      for (j = y1; j <= y2; j++) {
         p.y = j;
         (*fn)(g, p);
      }
   }
}
