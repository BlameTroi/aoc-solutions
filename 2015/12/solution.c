/* solution.c -- aoc 2015 12 -- troy brumley */

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "solution.h"


/*
 * parsing accounting data out of a json file.
 *
 * for part one, the structure of the file isn't really significant.
 * just report the sum of all the numbers in the file, which is one
 * long string of json text, unformatted, with no line breaks.
 *
 * for part two, ignore any array holding an attribute value of "red",
 * this includes any children of the array.
 *
 * no numbers will be embedded in strings.
 *
 * examples:
 *
 * [1,2,3] and {"a":2,"b":4} both have a sum of 6.
 *
 * [3] and {"a":{"b":4},"c":-1} both have a sum of 3.
 *
 * {"a":[-1,1]} and [-1,{"a":1}] both have a sum of 0.
 *
 * [] and {} both have a sum of 0.
 */


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

   /* get enough storage to load the whole file, is about 27K */
   struct stat istat;
   if (fstat(ifile->_file, &istat)) {
      printf("error trying to stat file\n");
      return EXIT_FAILURE;
   }

   char *buf = calloc(istat.st_size + 32, 1);
   assert(buf);
   fread(buf, istat.st_size, 1, ifile);

   int sum = sumNumbers(buf, NULL);
   printf("part one: %d\n", sum);

   free(buf);
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

   /* get enough storage to load the whole file, is about 27K */
   struct stat istat;
   if (fstat(ifile->_file, &istat)) {
      printf("error trying to stat file\n");
      return EXIT_FAILURE;
   }

   char *buf = calloc(istat.st_size + 32, 1);
   assert(buf);
   fread(buf, istat.st_size, 1, ifile);

   int scrubbed = scrubObjects(buf, "red");
   int sum = sumNumbers(buf, NULL);
   printf("part two: %d\n(scrubbed %d)\n", sum, scrubbed);

   free(buf);
   fclose(ifile);
   return EXIT_SUCCESS;
}


/*
 * sum the numbers in the string. negative values are allowed.
 */

inline bool
isValid(char c);

bool
isValid(
   char c
) {
   return isdigit(c) ||
          c == '-';
}

/*
 * if an object in the json stream has the the string attribute value
 * passed, scrub that object from the stream. returns number of
 * objects scrubbed.
 *
 * attributes are enclosed in quotes and checked for a prefixing colon.
 * scrubObjects(str, "red") will search for "\"red\" preceeded by a colon.
 *
 * assumes that object markers won't be embedded in quotes and that
 * stream is basically valid.
 */

int
scrubObjects(
   char *s,
   char *a
) {
   /* quick guards */
   if (s == NULL || *s == '\0' | strlen(s) <= strlen(a)+3) {
      return 0;
   }

   int scrubbed = 0;

   /* construct proper string for search ':"a"' */
   char *valAttribute = calloc(3 + strlen(a) + 1, 1);
   valAttribute[0] = ':';
   valAttribute[1] = '"';
   int i = 0;
   while (a[i]) {
      valAttribute[i+2] = a[i];
      i += 1;
   }
   valAttribute[i+2] = '"';

   /* prime the pump and start searching. as we are looking for the
      attribute without whitespace, there's no need to worry about the
      attribute value being a string in an array or as a key within an
      object. */

   char *pos = strstr(s, valAttribute);
   while (pos) {

      /* find the start of the current object, {, assuming that there
         are no braces in strings. */

      int nesting = 0;
      char *bwd = pos - 1;
      while (bwd >= s) {
         if (*bwd == '{' && nesting == 0) {
            break;
         }
         if (*bwd == '}') {
            nesting += 1;
         } else if (*bwd == '{') {
            nesting -= 1;
         }
         bwd -= 1;
      }
      assert(bwd >= s);

      /* find the end of the current object, }, again assuming that
         there are no braces in strings. yes, i should have keep track
         of depth so i could have strted the scan at pos, but meh. */

      nesting = 0;
      char *fwd = bwd + 1;
      while (*fwd) {
         if (*fwd == '}' && nesting == 0) {
            break;
         }
         if (*fwd == '{') {
            nesting += 1;
         } else if (*fwd == '}') {
            nesting -= 1;
         }
         fwd += 1;
      }
      assert(*fwd == '}');

      /* remove the object by overwriting it with blanks */
      fwd += 1;
      memset(bwd, ' ', fwd - bwd);
      scrubbed += 1;

      /* and keep searching */
      pos = strstr(fwd, valAttribute);
   }

   /* in a general solution i'd use a real parser and also do things
      like look for whitespace, but the aoc data is helpful here so
      we're done. */

   free(valAttribute);
   return scrubbed;
}


/*
 * sum the numbers found in the string buffer bounded by pointers from
 * and to inclusive. if to is NULL, go to the end of the string.
 */

int
sumNumbers(
   char *from,
   char *to
) {
   int sum = 0;

   const char *beg = from;
   char *end = from;

   if (to == NULL) {
      to = from + strlen(from);
   }

   while (*from && from <= to) {
      if (!isValid(*from)) {
         from += 1;
         continue;
      }
      beg = from;
      int cur = strtol(beg, &end, 10);
      sum += cur;
      from = end;
   }

   return sum;
}
