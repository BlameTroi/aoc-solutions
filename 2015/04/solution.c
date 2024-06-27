/*  solution.c -- aoc 2015 04 -- troy brumley */

/*
 * santa needs some crypto currency!
 *
 * find MD5 hashes which (as hex digits) start with at least five
 * zeroes. we are given a secret key. find the lowest positive number
 * that produces such a hash when appended to the key.
 *
 * example: if key is abcdef the smallest number that gets the desired
 * hash pattern is 609043 (000001dbbfa...).
 *
 * accept the key from the command line. there is nothing cute to do
 * here, the solution is brute force.
 */
/* i suspect most people doing this in c would have used openssl, or */
/* rather libressl, but i gave the apple wrappers a try. */

#include <CommonCrypto/CommonDigest.h>

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* maximum length of the text to digest. it actually can't be more */
/* than 21 + length of key, but memory is rather cheap. */

#define HASH_MAX (1024)

void
computeMd5(
   const char *str,
   unsigned char digest[16]
);

int
main(
   int argc,
   const char **argv
) {

   if (argc < 2) {
      printf("usage: %s secret-key\n", argv[0]);
      return EXIT_FAILURE;
   }

   const char *baseKey = argv[1];
   size_t keyLen = strlen(baseKey);
   char *workarea = calloc(HASH_MAX, 1);
   assert(workarea);
   strcpy(workarea, baseKey);
   uint64_t odometer = 0;
   unsigned char digest[16];
   int done5 = 0;
   int done6 = 0;

   /* no check for runaway here. the expectation is that we */
   /* will get an answer within the limits of the problem. */
   while (!done5 || !done6) {
      odometer += 1;
      memset(workarea+keyLen, 0, HASH_MAX-1-keyLen);
      sprintf(workarea+keyLen, "%llu", odometer);
      computeMd5(workarea, digest);
      /* checking for leading zeros is actually pretty easy. */
      if (digest[0]) {
         continue;
      }
      if (digest[1]) {
         continue;
      }
      if (digest[2] < 16) {
         if (!done5) {
            printf("(5)odometer: %llu\n", odometer);
            done5 = 1;
         }
      }
      if (digest[2]) {
         continue;
      }
      if (!done6) {
         printf("(6)odometer: %llu\n", odometer);
         done6 = 1;
      }
   }

   return EXIT_SUCCESS;
}

/* yes, md5 isn't all that secure, but big deal. */
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
void
computeMd5(
   const char *str,
   unsigned char digest[16]) {

   CC_MD5_CTX ctx;
   CC_MD5_Init(&ctx);
   CC_MD5_Update(&ctx, str, strlen(str));
   CC_MD5_Final(digest, &ctx);

}
