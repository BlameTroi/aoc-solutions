/* solution.c -- aoc 2016 05 -- troy brumley */

/*
 * The eight-character password for the door is generated one
 * character at a time by finding the MD5 hash of some Door ID (your
 * puzzle input) and an increasing integer index (starting with 0).
 *
 * A hash indicates the next character in the password if its
 * hexadecimal representation starts with five zeroes. If it does, the
 * sixth character in the hash is the next character of the password.
 *
 * our starting value is wtnhxymk
 */

#include <stdio.h>
#include <stdlib.h>

#include "solution.h"

#define TXBMD5_IMPLEMENTATION
#include "txbmd5.h"

#define DOOR_ID "wtnhxymk"
#define PASSWORD_LENGTH 8

/*
 * part one:
 *
 */

int
part_one(
   const char *fname
) {

   char *door_id = DOOR_ID;
   uint8_t digest[16];
   char password[PASSWORD_LENGTH+1];
   int len = 0;
   long odometer = 0;
   char buffer[1024];

   memset(digest, 0, sizeof(digest));
   memset(password, 0, sizeof(password));
   memset(buffer, 0, sizeof(buffer));

   const char digits[16] = "0123456789abcdef";

   while (len < PASSWORD_LENGTH) {
      snprintf(buffer, sizeof(buffer) - 1, "%s%ld", door_id, odometer);
      md5_string(buffer, digest);
      if (digest[0] == 0 && digest[1] == 0 && digest[2] < 16) {
         password[len] = digits[digest[2]];
         len += 1;
         /* for (int i = 0; i < PASSWORD_LENGTH; i++) { */
         /*    printf("%c", password[i] ? password[i] : '.'); */
         /* } */
         /* printf(" %ld\n", odometer); */
      }
      odometer += 1;
   }

   printf("part one: %s\n", password);

   return EXIT_SUCCESS;
}


/*
 * part two:
 */

int
part_two(
   const char *fname
) {
   char *door_id = DOOR_ID;
   uint8_t digest[16];
   char password[PASSWORD_LENGTH+1];
   int len = 0;
   long odometer = 0;
   char buffer[1024];

   memset(digest, 0, sizeof(digest));
   memset(password, 0, sizeof(password));
   memset(buffer, 0, sizeof(buffer));

   const char digits[16] = "0123456789abcdef";

   while (len < PASSWORD_LENGTH) {
      snprintf(buffer, sizeof(buffer) - 1, "%s%ld", door_id, odometer);
      md5_string(buffer, digest);
      if (digest[0] == 0 && digest[1] == 0 && digest[2] < 8) {
         int pos = digest[2];
         int idx = digest[3] >> 4;
         if (password[pos] == '\0') {
            password[pos] = digits[idx];
            /* for (int i = 0; i < PASSWORD_LENGTH; i++) { */
            /*    printf("%c", password[i] ? password[i] : '.'); */
            /* } */
            /* printf(" %ld\n", odometer); */
            len += 1;
         }
      }
      odometer += 1;
   }

   printf("part two: %s\n", password);

   return EXIT_SUCCESS;
}
