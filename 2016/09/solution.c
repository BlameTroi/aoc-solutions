/* solution.c -- aoc 2016 09 -- troy brumley */

#include <stdio.h>
#include <stdlib.h>

#include "solution.h"

#define TXBABORT_IMPLEMENTATION
#include "txbabort.h"
#define TXBSTR_IMPLEMENTATION
#include "txbstr.h"
#define TXBMISC_IMPLEMENTATION
#include "txbmisc.h"
#define TXBSB_IMPLEMENTATION
#include "txbsb.h"
#define TXBRS_IMPLEMENTATION
#include "txbrs.h"
#define TXBPAT_IMPLEMENTATION
#include "txbpat.h"

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

   /* i know the file is around 20k in size */
   char *data_buf = malloc(32767);
   assert(data_buf);
   memset(data_buf, 0, 32767);
   fgets(data_buf, 32766, ifile);

   rscb *compressed = rs_create_string(data_buf);

   /* not needed now that data is in read stream. */
   memset(data_buf, 253, 32767);
   free(data_buf);
   fclose(ifile);

   /* expand into a string buffer, it should be at least as large as
    * the input. */
   sbcb *uncompressed = sb_create_blksize(rs_length(compressed));

   /* expand compressed runs into a smaller buffer. */
   sbcb *run = sb_create_blksize(8192);

   /* read, process, output ... same as it always was */
   int c;
   while ((c = rs_getc(compressed)) != EOF) {

      /* if not in a run, just send straight to output */
      if (c != '(') {
         if (c > ' ') {
            sb_putc(uncompressed, c);
         }
         continue;
      }

      /* read run length ... (lengthxtimes) */
      int run_length = 0;
      int repeat_count = 0;
      while (is_digit(c = rs_getc(compressed))) {
         run_length = run_length * 10 + (c - '0');
      }
      assert(c == 'x');
      while (is_digit(c = rs_getc(compressed))) {
         repeat_count = repeat_count * 10 + (c - '0');
      }
      assert(c == ')');

      /* copy run */
      sb_reset(run);
      while (run_length) {
         assert(!rs_at_end(compressed));
         sb_putc(run, rs_getc(compressed));
         run_length -= 1;
      }

      /* write out repeat times */
      char *temp = sb_to_string(run);
      while (repeat_count) {
         sb_puts(uncompressed, temp);
         repeat_count -= 1;
      }
      memset(temp, 253, strlen(temp));
      free(temp);
   }

   printf("input length: %lu\n", rs_length(compressed));

   printf("part one: %lu\n", sb_length(uncompressed));

   free(uncompressed);
   free(compressed);
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

   FILE *ifile = fopen(fname, "r");
   if (!ifile) {
      fprintf(stderr, "error: could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }

   /* i know the file is around 20k in size */
   char *data_buf = malloc(32767);
   assert(data_buf);
   memset(data_buf, 0, 32767);
   fgets(data_buf, 32766, ifile);

   rscb *compressed = rs_create_string(data_buf);

   /* not needed now that data is in read stream. */
   memset(data_buf, 253, 32767);
   free(data_buf);
   fclose(ifile);

   /* expand into a string buffer, it should be at least as large as
    * the input. */
   sbcb *uncompressed = sb_create_blksize(rs_length(compressed));

   /* expand compressed runs into a smaller buffer. */
   sbcb *run = sb_create_blksize(8192);

   /* search patterns */
   const cpat *run_marker = compile_pattern("\\(\\d+x\\d+\\)");

   /* read, process, output ... same as it always was */
   int c;
   while ((c = rs_getc(compressed)) != EOF) {

      /* if not in a run, just send straight to output */
      if (c != '(') {
         if (c > ' ') {
            sb_putc(uncompressed, c);
         }
         continue;
      }

      /* read run length ... (lengthxtimes) */
      int run_length = 0;
      int repeat_count = 0;
      while (is_digit(c = rs_getc(compressed))) {
         run_length = run_length * 10 + (c - '0');
      }
      assert(c == 'x');
      while (is_digit(c = rs_getc(compressed))) {
         repeat_count = repeat_count * 10 + (c - '0');
      }
      assert(c == ')');

      /* copy run */
      sb_reset(run);
      int count = run_length;
      while (count) {
         assert(!rs_at_end(compressed));
         sb_putc(run, rs_getc(compressed));
         count -= 1;
      }

      /* write out repeat times */
      char *temp = sb_to_string(run);
      count = repeat_count;
      while (count) {
         sb_puts(uncompressed, temp);
         count -= 1;
      }

      /* report on the run for analysis */
      if (match(temp, run_marker)) {
         printf("embedded run inside (%dx%d): '%s'\n", run_length, repeat_count, temp);
      }

      memset(temp, 253, strlen(temp));
      free(temp);
   }

   printf("input length: %lu\n", rs_length(compressed));

   printf("part two: %lu\n", sb_length(uncompressed));

   free((void *)run_marker);
   free(run);
   free(uncompressed);
   free(compressed);
   fclose(ifile);

   return EXIT_SUCCESS;
}
