/*  solution.c -- aoc 2015 02 -- troy brumley */

/*
 * how much wrapping paper is needed? gift dimensions are supplied in feet:
 * lxwxh. each gift is a perfect rectangle so its surface area is
 * 2*l*w + 2*w*h + 2*h*l. each gift requires a slack amount of paper
 * equal to the area of its smallest side.
 *
 * example:
 *
 * 2x3x4 requires 2*6 + 2*12 + 2*8 = 52 square feet of wrapping paper
 * plus 6 square feet of slack, for a total of 58 square feet.
 *
 * the ribbon required to wrap a present is the shortest distance
 * around its sides, or the smallest perimeter of any one face. each
 * present also requires a bow made out of ribbon as well; the feet of
 * ribbon required for the perfect bow is equal to the cubic feet of
 * volume of the present. don't ask how they tie the bow, though;
 * they'll never tell.
 *
 * example:
 *
 * 2x3x4 requires 2+2+3+3 = 10 feet to wrap plus 2 * 3 * 4 = 24 feet
 * for a bow.
 */


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TXBMISC_IMPLEMENTATION
#include "txbmisc.h"

/*
 * gift box metrics type, units are consistent.
 */
typedef struct gift_t {
   size_t l;
   size_t w;
   size_t h;
   size_t area;           /*  total surface area */
   size_t slack;          /*  extra wrap surface */
   size_t ribbon;         /*  shortest perimiter */
   size_t bow;            /*  bow length = volume units */
} gift_t;


/*
 * given an input line lxwxh\n, parse out the
 * dimensions and compute the metrics.
 */
gift_t
getWrap(
   char *line,
   size_t line_max
) {
   char *work = strdup(line);
   assert(work);
   char *pos = work;

   char *l = strsep(&pos, "x\n");
   assert(l);
   char *w = strsep(&pos, "x\n");
   assert(w);
   char *h = strsep(&pos, "x\n");
   assert(h);

   gift_t g;

   g.l = atoi(l);
   g.w = atoi(w);
   g.h = atoi(h);

   /* each surface's area */
   size_t lw = g.l * g.w;
   size_t wh = g.w * g.h;
   size_t hl = g.h * g.l;

   /* and each surface's perimiter */
   size_t plw = g.l*2 + g.w*2;
   size_t pwh = g.w*2 + g.h*2;
   size_t phl = g.h*2 + g.l*2;

   /* total surface area */
   g.area = 2*lw + 2*wh + 2*hl;

   /* extra slack is the minimum side area */
   g.slack = min(lw, wh);
   g.slack = min(g.slack, hl);

   /* base ribbon length for the wrap is minimum perimiter */
   g.ribbon = min(plw, pwh);
   g.ribbon = min(g.ribbon, phl);

   /* ribbon for bow is oddly enough the volume */
   g.bow = g.l * g.w * g.h;

   free(work);
   return g;
}


/*
 * do the work
 */
int
main(
   int argc,
   const char **argv
) {
   FILE *ifile;
   size_t totalWrap = 0;
   size_t totalRibbon = 0;

   if (argc < 2) {
      printf("usage: %s path-to-input\n", argv[0]);
      return EXIT_FAILURE;
   }

   ifile = fopen(argv[1], "r");
   if (!ifile) {
      printf("could not open file: %s\n", argv[1]);
      return EXIT_FAILURE;
   }

   const size_t LINE_MAX = 255;
   char iline[LINE_MAX+1];

   while (fgets(iline, LINE_MAX, ifile)) {
      gift_t g = getWrap(iline, LINE_MAX);
      totalWrap = totalWrap + g.area + g.slack;
      totalRibbon = totalRibbon + g.ribbon + g.bow;
   }

   if (ferror(ifile)) {
      printf("error: aborting\n");
      fclose(ifile);
      return EXIT_FAILURE;
   }

   printf("total wrapping paper needed: %zu\n", totalWrap);
   printf("total ribbon needed: %zu\n", totalRibbon);

   fclose(ifile);

   return EXIT_SUCCESS;
}
