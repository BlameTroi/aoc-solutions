// solution.c -- aoc 2015 04 -- troy brumley

// santa needs some crypto currency

// find MD5 hashes which (as hex digits) start with at least five
// zeroes. we are given a secret key. find the lowest positive number
// that produces such a hash when appended to the key.
//
// example: if key is abcdef the smallest number that gets the desired
// hash pattern is 609043 (000001dbbfa...).
//
// accept the key from the command line. there is nothing cute to do
// here, the solution is brute force.

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CommonCrypto/CommonCrypto.h>

#define HASH_MAX (1024)

void computeMd5(char *str, unsigned char digest[16]);

int
main(int argc, char **argv) {

   if (argc < 2) {
      printf("usage: %s secret-key\n", argv[0]);
      return EXIT_FAILURE;
   }

   char *baseKey = argv[1];
   size_t keyLen = strlen(baseKey);
   char *workarea = calloc(HASH_MAX, 1);
   assert(workarea);
   strcpy(workarea, baseKey);
   uint64_t odometer = 0;
   unsigned char digest[16];
   int done5 = 0;
   int done6 = 0;

   // we'll assume that this will eventually work
   while (!done5 || !done6) {
      odometer += 1;
      memset(workarea+keyLen, 0, HASH_MAX-1-keyLen);
      sprintf(workarea+keyLen, "%llu", odometer);
      computeMd5(workarea, digest);
      if (digest[0]) continue;
      if (digest[1]) continue;
      if (digest[2] < 16) {
         if (!done5) {
            printf("(5)odometer: %llu\n", odometer);
            done5 = 1;
         }
      }
      if (digest[2]) continue;
      if (!done6) {
         printf("(6)odometer: %llu\n", odometer);
         done6 = 1;
      }
   }

   return EXIT_SUCCESS;
}

#pragma clang diagnostic ignored "-Wdeprecated-declarations"
void computeMd5(char *str, unsigned char digest[16]) {

   CC_MD5_CTX ctx;
   CC_MD5_Init(&ctx);
   CC_MD5_Update(&ctx, str, strlen(str));
   CC_MD5_Final(digest, &ctx);

}
