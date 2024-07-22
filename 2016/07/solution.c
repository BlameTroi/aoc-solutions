/* solution.c -- aoc 2016 07 -- troy brumley */

/*
 * - abba[mnop]qrst supports TLS (abba outside square brackets).
 *
 * - abcd[bddb]xyyx does not support TLS (bddb is within square brackets,
 *   even though xyyx is outside square brackets).
 *
 * - aaaa[qwer]tyui does not support TLS (aaaa is invalid; the interior
 *   characters must be different).
 *
 * - ioxxoj[asdfgh]zxcvbn supports TLS (oxxo is outside square brackets,
 *   even though it's within a larger string).
 */

/*
 * an aba is any such sequence outside of square brackets
 *  a bab is any such sequence inside of square brackets
 *
 * to be valid, an aba must have a matching (same chars, different
 * positions) bab.
 *
 * - aba[bab]xyz supports SSL (aba outside square brackets with
 *   corresponding bab within square brackets).
 *
 * - xyx[xyx]xyx does not support SSL (xyx, but no corresponding yxy).
 *
 * - aaa[kek]eke supports SSL (eke in supernet with corresponding kek in
 *   hypernet; the aaa sequence is not related, because the interior
 *   character must be different).
 *
 * - zazbz[bzb]cdb supports SSL (zaz has no corresponding aza, but zbz
 *   has a corresponding bzb, even though zaz and zbz overlap).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

#define TXBSTR_IMPLEMENTATION
#include "txbstr.h"

/*
 * is the character at position i inside an aba sequence?
 */

static bool
aba_here(
   const char *str,
   int i
);

static inline bool
aba_here(
   const char *str,
   int i
) {
   if (str[i+2] == 0 || str[i+1] == 0 || str[i] == 0) {
      return false;
   }
   return str[i+2] == str[i] && str[i+1] != str[i];
}

/*
 * is the character at position i inside an abba sequence?
 */

static bool
abba_here(
   const char *str,
   int i
);

static inline bool
abba_here(
   const char *str,
   int i
) {
   if (i == 0) {
      return false;
   }
   if (str[i-1] != str[i]) { /* ab?? */
      if (str[i+1] == str[i]) { /* abb? */
         if (str[i-1] == str[i+2]) { /* abba? */
            return true;
         }
      }
   }
   return false;
}

/*
 * for part one, a 'good' abba sequence is one outside of [].
 */

bool
has_good_abba(
   const char *str
) {
   int len = strlen(str);
   int i = 1;
   bool in_hypernet = false;
   while (i <= len-2) {
      if (in_hypernet && str[i] != ']') {
         i += 1;
         continue;
      }
      if (str[i] == '[') {
         in_hypernet = true;
         i += 1;
         continue;
      }
      if (str[i] == ']') {
         in_hypernet = false;
         i += 1;
         continue;
      }
      if (abba_here(str, i)) {
         return true;
      }
      i += 1;
   }
   return false;
}

/*
 * and also for part one, a bad abba sequence is one inside [].
 */

bool
has_bad_abba(
   const char *str
) {
   int len = strlen(str);
   int i = 1;
   bool in_hypernet = false;
   while (i <= len-2) {
      if (!in_hypernet && str[i] != '[') {
         i += 1;
         continue;
      }
      if (str[i] == ']') {
         in_hypernet = false;
         i += 1;
         continue;
      }
      if (str[i] == '[') {
         in_hypernet = true;
         i += 1;
         continue;
      }
      if (abba_here(str, i)) {
         return true;
      }
      i += 1;
   }
   return false;
}

/*
 * for part one, an input record is valid if it meets the following
 * criteria:
 *
 * an abba sequence is only found outside of [].
 */

bool
supports_tls(
   const char *str
) {
   return has_good_abba(str) && !has_bad_abba(str);
}

/*
 * for part two, an input record is valid if it meets the following
 * criteria:
 *
 * an aba sequence outside of [] has a corresponding bab sequence inside
 * of [].
 */

bool
supports_ssl(
   const char *str
) {
   int len = strlen(str);

   /* split out supernet and hypernet portions of the input address.
    * anything outside of [] is supernet, and anything inside [] is
    * hypernet. */

   char *supernet = dup_string(str);
   char *hypernet = dup_string(str);

   /* for supernet values, replace anything inside [] with blank */
   char *p = supernet;
   while (*p) {
      if (*p == '[') {
         while (*p && *p != ']') {
            *p = ' ';
            p += 1;
         }
         if (*p == '\0') {
            break;
         }
         if (*p == ']') {
            *p = ' ';
         }
      }
      p += 1;
   }

   /* for hypernet values, replace anything outside [] with blank */
   p = hypernet;
   while (*p) {
      if (*p == '[') {
         *p = ' ';
         while (*p && *p != ']') {
            p += 1;
         }
         if (*p == '\0') {
            break;
         }
      }
      *p = ' ';
      p += 1;
   }

   /* for each aba pattern found in the supernet portion, see if there
    * is a corresponding bab pattern in the hypernet portion. */
   int i = 0;
   bool found = false;
   char bab[4];
   while (!found && i < len - 2) {
      if (supernet[i] == ' ') {
         i += 1;
         continue;
      }
      if (aba_here(supernet, i)) {
         bab[0] = supernet[i+1];
         bab[1] = supernet[i];
         bab[2] = bab[0];
         bab[3] = '\0';
         int j = 0;
         while (!found && j < len - 2) {
            if (hypernet[j] == ' ') {
               j += 1;
               continue;
            }
            if (aba_here(hypernet, j)) {
               found = memcmp(&hypernet[j], bab, 3) == 0;
            }
            j += 1;
         }
      }
      i += 1;
   }

   /* finish cleaning up and return */
   free(supernet);
   free(hypernet);
   return found;
}

/*
 * part one:
 */

int
part_one(
   const char *fname
) {

   FILE *ifile = fopen(fname, "r");
   if (!ifile) {
      fprintf(stderr, "error: could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }

   char iline[INPUT_LINE_MAX];

   int count = 0;
   while (fgets(iline, INPUT_LINE_MAX - 1, ifile)) {
      if (supports_tls(iline)) {
         count += 1;
      }
   }

   printf("part one: %d\n", count);

   fclose(ifile);
   return EXIT_SUCCESS;
}


/*
 * part two:
 */

int
part_two(
   const char *fname
) {
   FILE *ifile;

   ifile = fopen(fname, "r");
   if (!ifile) {
      fprintf(stderr, "error: could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }
   char iline[INPUT_LINE_MAX];

   int count = 0;
   while (fgets(iline, INPUT_LINE_MAX - 1, ifile)) {
      if (supports_ssl(iline)) {
         count += 1;
      }
   }

   printf("part two: %d\n", count);

   fclose(ifile);
   return EXIT_SUCCESS;
}
