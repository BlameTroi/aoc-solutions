/* solution.c -- aoc 2016 08 -- troy brumley */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

#define TXBSTR_IMPLEMENTATION
#include "txbstr.h"

/*
 * create a display and clear it.
 */

display *
create_display(
   void
) {
   display *d = malloc(sizeof(display));
   assert(d);
   memset(d, 0, sizeof(display));
   memcpy(d->tag, DISPLAY_TAG, sizeof(d->tag));
   clear_display(d);
   return d;
}

/*
 * set all pixels OFF.
 */

void
clear_display(
   display *d
) {
   assert(d && memcmp(d->tag, DISPLAY_TAG, sizeof(d->tag)) == 0);
   for (int i = 0; i < ROW_MAX; i++) {
      for (int j = 0; j < COLUMN_MAX; j++) {
         d->pixels[i][j] = OFF;
      }
   }
}

/*
 * part one wants to know how many pixels are on lit after running
 * a provided program.
 */

int
count_lit_pixels(
   display *d
) {
   assert(d && memcmp(d->tag, DISPLAY_TAG, sizeof(d->tag)) == 0);
   int count = 0;
   for (int row = 0; row < ROW_MAX; row++) {
      for (int column = 0; column < COLUMN_MAX; column++) {
         count += d->pixels[row][column] == ON ? 1 : 0;
      }
   }
   return count;
}

/*
 * return pixel state
 */

uint8_t
get_pixel(
   display *d,
   int row,
   int column
) {
   assert(d && memcmp(d->tag, DISPLAY_TAG, sizeof(d->tag)) == 0);
   assert(row >= 0 && row < ROW_MAX && column >= 0 && column <= COLUMN_MAX);
   return d->pixels[row][column];
}

/*
 * dump the display on stream file. if no file is specified, use stdout.
 */

void
show_display(
   display *d,
   FILE *f
) {
   assert(d && memcmp(d->tag, DISPLAY_TAG, sizeof(d->tag)) == 0);
   if (f == NULL) {
      f = stdout;
   }
   for (int row = 0; row < ROW_MAX; row++) {
      fprintf(f, "\n");
      for (int column = 0; column < COLUMN_MAX; column++) {
         fprintf(f, "%c", d->pixels[row][column] == ON ? 'X' : '_');
      }
   }
   fprintf(f, "\n");
}

/*
 * release the display storage block.
 */

void
destroy_display(
   display *d
) {
   assert(d && memcmp(d->tag, DISPLAY_TAG, sizeof(d->tag)) == 0);
   memset(d, 252, sizeof(display));
   free(d);
}

/*
 * create a new program. chip might be a better abstraction name. memory
 * to hold the program and an instruction counter are allocated.
 */

program *
create_program(
   void
) {
   program *p = malloc(sizeof(program));
   memset(p, 0, sizeof(program));
   memcpy(p->tag, PROGRAM_TAG, sizeof(p->tag));
   p->trace = false;
   return p;
}

/*
 * compile an instruction from a source string and place it at *ip,
 * typically an entry in the program.instructions array. source
 * structure isn't well thought out, but it is consistent:
 *
 * command     a       b
 *
 * where command has some irregular sweetening. for rotation, 'a' is a
 * 0 based row or column, and 'b' is the number of units. for rect
 * 'a' is width and 'b' is height.
 */

bool
compile_instruction(instruction *ip, const char *source) {
   memset(ip, 0, sizeof(instruction));
   const char **tokens = split_string(source, " byx=\n");
   int a;
   int b;

   /* rect's arguments are not zero based, even though the position
    * of painting is 0,0. */
   if (strcmp(tokens[1], "rect") == 0) {
      a = strtol(tokens[2], NULL, 10);
      if (a < 1 || a > COLUMN_MAX) {
         free_split(tokens);
         return false;
      }
      b = strtol(tokens[3], NULL, 10);
      if (b < 1 || b > ROW_MAX) {
         free_split(tokens);
         return false;
      }
      ip->fn = do_rect;
      ip->a = a;
      ip->b = b;
      free_split(tokens);
      return true;
   }

   if (strcmp(tokens[1], "rotate") == 0 && strcmp(tokens[2], "row") == 0) {
      a = strtol(tokens[3], NULL, 10);
      if (a < 0 || a >= ROW_MAX) {
         free_split(tokens);
         return false;
      }
      b = strtol(tokens[4], NULL, 10);
      ip->fn = do_rotate_row;
      ip->a = a;
      ip->b = b;
      free_split(tokens);
      return true;
   }

   if (strcmp(tokens[1], "rotate") == 0 && strcmp(tokens[2], "column") == 0) {
      a = strtol(tokens[3], NULL, 10);
      if (a < 0 || a >= COLUMN_MAX) {
         free_split(tokens);
         return false;
      }
      b = strtol(tokens[4], NULL, 10);
      ip->fn = do_rotate_column;
      ip->a = a;
      ip->b = b;
      free_split(tokens);
      return true;
   }
   free_split(tokens);
   return false;
}

/*
 * load a program from a file, reads each statement and and compiles
 * it into the program instruction memory. file should be opened and
 * properly position when calling.
 */

bool
load_program(
   program *p,
   FILE *f
) {
   assert(p && memcmp(p->tag, PROGRAM_TAG, sizeof(p->tag)) == 0);
   assert(f);
   char buffer[INPUT_LINE_MAX];
   p->ip = 0;
   while (fgets(buffer, INPUT_LINE_MAX - 1, f)) {
      if (p->ip >= INSTRUCTION_MAX) {
         return false;
      }
      if (!compile_instruction(&p->instructions[p->ip], buffer)) {
         return false;
      }
      p->ip += 1;
   }
   return true;
}

/*
 * for tracing or debugging, print the source form of the instruction
 * on open file stream f. if f is not set, stdout is used.
 */

void
show_instruction(
   instruction *i,
   FILE *f
) {
   if (f == NULL) {
      f = stdout;
   }
   if (i->fn == do_rect) {
      fprintf(f, "rect %dx%d\n", i->a, i->b);
      return;
   }
   if (i->fn == do_rotate_row) {
      fprintf(f, "rotate row y=%d by %d\n", i->a, i->b);
      return;
   }
   if (i->fn == do_rotate_column) {
      fprintf(f, "rotate column x=%d by %d\n", i->a, i->b);
      return;
   }
   assert(NULL);
}

/*
 * execute the program instructions from first to last, updating the
 * state of the display. the display is not cleared before running, the
 * client set the display to the desired starting state.
 *
 * if program.trace not null, it should be an open file stream pointer.
 * execution will be logged there.
 */

bool
run_program(
   program *p,
   display *d
) {
   assert(d && memcmp(d->tag, DISPLAY_TAG, sizeof(d->tag)) == 0);
   assert(p && memcmp(p->tag, PROGRAM_TAG, sizeof(p->tag)) == 0);
   if (p->instructions[0].fn == NULL) {
      return false;
   }
   p->ip = 0;
   while (p->instructions[p->ip].fn) {
      instruction *i = &p->instructions[p->ip];
      (i->fn)(d, i->a, i->b);
      if (p->trace) {
         fprintf(p->trace, "\ntrace ip=%d  instruction=", p->ip);
         show_instruction(i, p->trace);
         show_display(d, p->trace);
      }
      p->ip += 1;
   }
   return true;
}

/*
 * release program storage block.
 */

void
destroy_program(
   program *p
) {
   assert(p && memcmp(p->tag, PROGRAM_TAG, sizeof(p->tag)) == 0);
   memset(p, 252, sizeof(program));
   free(p);
}

/*
 * rect AxB
 *
 * turns on all of the pixels in a rectangle at the top-left of the
 * screen which is A wide and B tall.
 */

void
do_rect(
   display *d,
   uint8_t a,
   uint8_t b
) {
   assert(d && memcmp(d->tag, DISPLAY_TAG, sizeof(d->tag)) == 0);
   for (int row = 0; row < b; row++) {
      for (int column = 0; column < a; column++) {
         d->pixels[row][column] = ON;
      }
   }
}

/*
 * rotate row y=A by B
 *
 * rotate row y=A by B shifts all of the pixels in row A (0 is the
 * top row) right by B pixels. Pixels that would fall off the right
 * end appear at the left end of the row.
 */

void
do_rotate_row(
   display *d,
   uint8_t a,
   uint8_t b
) {
   assert(d && memcmp(d->tag, DISPLAY_TAG, sizeof(d->tag)) == 0);
   assert(a >= 0 && a <= ROW_MAX);
   for (int i = 0; i < b; i++) {
      uint8_t c = d->pixels[a][COLUMN_MAX-1];
      for (int j = COLUMN_MAX-1; j > 0; j--) {
         d->pixels[a][j] = d->pixels[a][j-1];
      }
      d->pixels[a][0] = c;
   }
}

/*
 * rotate column x=A by B
 *
 * rotate column x=A by B shifts all of the pixels in column A (0 is
 * the left column) down by B pixels. Pixels that would fall off the
 * bottom appear at the top of the column.
 */

void
do_rotate_column(
   display *d,
   uint8_t a,
   uint8_t b
) {
   assert(d && memcmp(d->tag, DISPLAY_TAG, sizeof(d->tag)) == 0);
   assert(a >= 0 && a <= COLUMN_MAX);
   for (int i = 0; i < b; i++) {
      uint8_t c = d->pixels[ROW_MAX-1][a];
      for (int j = ROW_MAX-1; j > 0; j--) {
         d->pixels[j][a] = d->pixels[j-1][a];
      }
      d->pixels[0][a] = c;
   }
}

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
      fprintf(stderr, "error: could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }

   program *p = create_program();
   display *d = create_display();
   load_program(p, ifile);
   run_program(p, d);
   show_display(d, stdout);
   int count = count_lit_pixels(d);
   destroy_program(p);
   destroy_display(d);

   fclose(ifile);

   printf("part one: %d\n", count);

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
      fprintf(stderr, "error: could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }
   char iline[INPUT_LINE_MAX];

   while (fgets(iline, INPUT_LINE_MAX - 1, ifile)) {
   }

   printf("part two: %d\n", 0);

   fclose(ifile);
   return EXIT_SUCCESS;
}
