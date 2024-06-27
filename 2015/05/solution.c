/* solution.c -- aoc 2015 05 -- troy brumley */

/* aoc 2015 5 -- naughty or nice list? */

/*
 * use string pattern matching to determine if a string is 'naughty'
 * or 'nice' with rather odd rules for each part. part one was
 * amenable to a hand rolled solution, but part two was better served
 * by regular expressions. while there are posix compliant regular
 * expressions in macos, i went with pcre2 and redid part one using
 * them.
 *
 * input test file for the main run is 1000 lines of 16 lower case
 * characters each.
 *
 * input short test file created from problem.org has five strings,
 * the first two of which are nice. the second string is only three
 * characters long but it's still nice.
 */


/* standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * pcre2 support, 8 bit characters. link against the appropriate
 * code unit width library. '-lpcre2-8'.
 */
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>


/* forward declarations */

int part_one(char *fname);
int part_two(char *fname);


/* mainline */
int
main(int argc, char ** argv) {

   if (argc < 2) {
      printf("usage: %s path-to-input\n", argv[0]);
      return EXIT_FAILURE;
   }

   if (part_one(argv[1]) != EXIT_SUCCESS) {
      printf("error in part one\n");
      return EXIT_FAILURE;
   }

   if (part_two(argv[1]) != EXIT_SUCCESS) {
      printf("error in part two\n");
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}


/*
 * for part one a nice string is one that has:
 *
 *  at least three vowels (aeiou only).
 *  '(.*[aeiou]){3,}'
 *
 *  at least one letter that appears twice in a row, like xx, abcdde (dd).
 *  '([[:alpha:]])\1' or '(?=.*(\w)\1)'
 *
 *  but none of the pairs ab, cd, pq, xy.
 *  '^(?!.*(ab|cd|pq|xy)).*$' -- negative look ahead
 *
 *  the combined pattern that i tested with pcregrep is:
 *
 *  pcregrep '^(?!.*(ab|cd|pq|xy))(?=(.*[aeiou]){3})(?=.*(\w)\3).*$' <dataset.txt | wc
 *
 *  which gives the right answer of 238. the backrefs, numbering, and
 *  syntax isn't as clear to me as i'd like.
 */

int
part_one(char *fname) {

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

   /* regular expression overhead */
   pcre2_code *re;           /* compiled regular expression */
   PCRE2_SPTR pattern;       /* pattern source */
   int errornumber;          /* error in compile of re */
   PCRE2_SIZE erroroffset;   /* position of error in re */
   PCRE2_SPTR subject;       /* string to run pattern against */
   size_t subject_length;    /* its length, \n optional */
   pcre2_match_data *match_data;   /* result of match */

   /*                       none of these          3 of these   and at least repeat */
   pattern = (PCRE2_SPTR)"^(?!.*(ab|cd|pq|xy))(?=(.*[aeiou]){3})(?=.*(\\w)\\3).*$";
   /*                      1                  2                 3 */

   /* compile the pattern */
   re = pcre2_compile(pattern,
                      PCRE2_ZERO_TERMINATED,
                      0,
                      &errornumber,
                      &erroroffset,
                      NULL);

   if (re == NULL) {
      /* compile failed, report error */
      PCRE2_UCHAR buffer[256];
      pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
      printf("PCRE2 compilation failed at offset %d: %s\n", (int)erroroffset, buffer);
      return EXIT_FAILURE;
   }

   match_data = pcre2_match_data_create_from_pattern(re, NULL);

   size_t nice = 0;
   char strbuf[256];

   while((iline = fgetln(ifile, &ilen))) {
      memset(strbuf, 0, 255);
      memcpy(strbuf, iline, ilen-1);           /* omit newline */
      /* printf("%s", strbuf); */
      subject = (PCRE2_SPTR)strbuf;
      subject_length = strlen((char *)subject);
      int rc = pcre2_match(re,
                           subject,
                           subject_length,
                           0,
                           0,
                           match_data,
                           NULL);
      if (rc == PCRE2_ERROR_NOMATCH) {
         /* printf("\n"); */
      } else if (rc < 0) {
         printf("error matching on %s - error %d\n", subject, rc);
      } else {
         nice += 1;
         /* printf(" - nice %zu\n", nice); */
      }
   }
   pcre2_match_data_free(match_data);

   printf("\npart one: %zu\n", nice);
   pcre2_code_free(re);
   fclose(ifile);

   return EXIT_SUCCESS;
}


/*
 * for part two a nice string is one that has
 *
 * a pair of any two letters that appears at least twice without overlapping.
 * (?=(\\w\\w).+\\1)
 *
 * at least one letter that repeats with exactly one letter between them.
 * (?=(\w)\w\1)
 *
 * and for reasons i haven't yet understood, the second expression fails on
 * 'xxx' and it should not.
 *
 * since i've proved to myself that i can do regex from c, i'll revert to a
 * roll my own solution for this part.
 */

int
part_two(char *fname) {

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
   /* final total */
   size_t nice = 0;

   /* there are two conditions to check for and no disqualifiers. */
   /* x-x and xy-*xy, with overlaps allowed */
   while((iline = fgetln(ifile, &ilen))) {
      int gotOne = 0;      /* hit on x-x */
      int atOne = 0;
      int gotTwo = 0;      /* hit on xy-*xy */
      int atTwo = 0;
      int i = 0;
      int j = 0;
      if (iline[0] == '#') {
         continue;
      }
      /* check one, at least one letter repeats with exactly one */
      /* letter between the repeats, xyx or zzz are valid. */
      for (i = 2; i < ilen; i++) {
         if (iline[i] == iline[i-2]) {
            gotOne = 1;
            atOne = i - 2;
            break;
         }
      }
      /* check pair preceeded by itself with at least one intervening */
      /* character. xxxx is ok, xxyxx is ok, abcab is ok, aaa is not. */
      /* 01234 */
      for (i = 0; i < ilen; i++) {
         for (j = i+2; j < ilen; j++) {
            if (memcmp(iline+i, iline+j, 2) == 0) {
               gotTwo = 1;
               atTwo = i;
               break;
            }
         }
         if (gotTwo) {
            break;
         }
      }
      /* did we get both one and two? */
      if (gotOne && gotTwo) {
         nice += 1;
      }
   }

   printf("\npart two: %zu\n", nice);

   fclose(ifile);
   return EXIT_SUCCESS;
}
