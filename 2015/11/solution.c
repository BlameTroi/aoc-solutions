/* solution.c -- aoc 2015 11 -- troy brumley */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

/*
 * to help him remember his new password after the old one expires,
 * santa has devised a method of coming up with a password based on
 * the previous one. corporate policy dictates that passwords must be
 * exactly eight lowercase letters (for security reasons), along with
 * some new odious requirements so that a simple increment is not
 * sufficient.
 *
 * keep incrementing until a valid password is generated.
 *
 * original rules:

 * rule: 8 lowercase letters
 *
 * rule: odometer passwords, a-z are digits 0-25
 *
 * new rules:
 *
 * rule: at least one increasing sequential straight 'abc', 'bcd' are
 *       legal, 'abd' is not.
 *
 * rule: no visually confusing letters: 'i', 'o', and 'l' are not
 *       legal.
 *
 * rule: two distinct letter pairs: 'aa', 'bb', but 'aaa' does not
 *       count as two pair, they overlap. 'aaaa' should be ok.
 *
 */


/* limits */

#define PASSWORD_MAX 8
#define PASSWORD_TRIES_MAX 5000000


/*
 * part one:
 *
 * the way this problem ended up working, a separate part two was not
 * needed. both answers are produced in part one and part two is a
 * no-op.
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

   if (!fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
      printf("error reading input file\n");
      return EXIT_FAILURE;
   }

   fclose(ifile);


   /* assume iline is a valid starting password and work it */
   iline[PASSWORD_MAX] = '\0';

   /* current password provided, increment until we get the next valid one */
   int n = 0;
   while (n < PASSWORD_TRIES_MAX && !password_p(password_increment(iline))) {
      n += 1;
   }

   if (password_p(iline)) {
      printf("\npart one after %d tries gets %s\n", n, iline);
   } else {
      printf("\npart one error did not find password\n");
      return EXIT_FAILURE;
   }

   /* and what will the password be after that new one expires? */
   n = 0;
   while (n < PASSWORD_TRIES_MAX && !password_p(password_increment(iline))) {
      n += 1;
   }

   if (password_p(iline)) {
      printf("\npart two after another %d tries gets %s\n", n, iline);
   } else {
      printf("\npart two error did not find password\n");
      return EXIT_FAILURE;
   }

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
   return EXIT_SUCCESS;
   /* this is handled in part one
   FILE *ifile;
   ifile = fopen(fname, "r");
   if (!ifile) {
      printf("could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }
   char iline[INPUT_LEN_MAX];

   while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
   }

   printf("part two: %d\n", 0);

   fclose(ifile);
   return EXIT_SUCCESS;
   */
}


/*
 * a helper for password_p to find the next pair of repeating
 * characters.
 */

const char *
pairFrom(const char *p) {
   while (*p) {
      if (*p == *(p+1)) {
         return p;
      }
      p += 1;
   }
   return NULL;
}


/*
 * is a string a valid password under the new password rules? returns
 * 0 if the password is not valid.
 *
 * these are the rules:
 *
 * rule: 8 lowercase letters
 *
 * rule: odometer passwords, a-z are digits 0-25
 *
 * rule: at least one increasing sequential straight 'abc', 'bcd' are
 *       legal, 'abd' is not.
 *
 * rule: no visually confusing letters: 'i', 'o', and 'l' are not
 *       legal.
 *
 * rule: two distinct letter pairs: 'aa', 'bb', but 'aaa' does not
 *       count as two pair, they overlap. 'aaaa' should be ok.
 *
 */

bool
password_p(
   const char *candidate
) {

   /* eliminate via guards */
   if (candidate == NULL || strlen(candidate) != PASSWORD_MAX) {
      return 0;
   }

   /* work pointers */
   const char *p;
   const char *q;

   /* lower case but not i, o, or l */
   p = candidate;
   while (*p) {
      if (*p < 'a' || *p > 'z') {
         return false;
      }
      if (*p == 'i' || *p == 'o' || *p == 'l') {
         return false;
      }
      p += 1;
   }

   /* is there at least one straight? */
   int f = 0;
   p = candidate;
   while (*p) {
      /* stop when we run to the end */
      if (!*(p + 2)) {
         break;
      }
      /* do these three ascend? */
      if (*p < *(p + 1) && *(p + 1) < *(p + 2)) {
         f = (*(p + 2) - *(p + 1) == 1) && (*(p + 1) - *p == 1);
         if (f) {
            break;
         }
      }
      p += 1;
   }
   if (!f) {
      return false;
   }

   /* are there two non-overlapping pairs? */
   p = candidate;
   p = pairFrom(p);
   if (!p) {
      return false;
   }
   q = pairFrom(p+2);
   if (!q) {
      return false;
   }

   /* that's the end of the rules */
   return true;
}


/*
 * increment the provided password value viewing it as a base 26 digit
 * string a-z as 0-25. password is updated in place and also the function
 * return value for ease of coding elsewhere.
 *
 * note: this requires that the parameter is writeable and not a string
 * literal. strdup is your friend.
 */

char *
password_increment(
   char *p
) {

   char c;                /* character work */
   int r = 1;             /* adding 1, becomes carry as propogated */
   int n = PASSWORD_MAX;  /* units digit, we work right to left */
   while (n && r) {
      n -= 1;
      c = *(p+n) + r;     /* new digit = current + 1 + carry */
      r = 0;              /* clear carry */
      if (c > 'z') {      /* is there a carry for the next digit? */
         c = 'a' + c - 1 - 'z'; /* adjust and propogate */
         r = 1;
      }
      *(p+n) = c;           /* and update digit */
   }

   /* any overflow is ignored */
   return p;
}
