// solution.c -- aoc 2015 ?? -- troy brumley


#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(int argc, char **argv) {
   FILE *ifile;


   if (argc < 2) {
      printf("usage: %s path-to-input\n", argv[0]);
      return EXIT_FAILURE;
   }

   ifile = fopen(argv[1], "r");
   if (!ifile) {
      printf("could not open file: %s\n", argv[1]);
      return EXIT_FAILURE;
   }

   while(EOF != (ch = fgetc(ifile))) {
      if (<someerror>) {
         printf("error: aborting\n");
         fclose(ifile);
         return EXIT_FAILURE;
      }
   }

   printf("answers here\n");

   fclose(ifile);

   return EXIT_SUCCESS;
}
