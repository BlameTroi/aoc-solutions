/* solution.c -- aoc 2015 06 -- troy brumley */

/*
 * probably a fire hazard
 *
 * lights in a 0,0 -> 999,999 grid are turned off and on
 * as directed, how many lights are left lit at the end
 * of the run if all the lights are off at the start.
 *
 * (turn on|turn off|toggle) x0,y0 through x1,y1
 */

#include "solution.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * the day's real code.
 */

/* part one: */
int
partOne(char *fname) {
   FILE *ifile;

   ifile = fopen(fname, "r");
   if (!ifile) {
      printf("could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }

   char *iline;
   size_t ilen;

   while ((iline = fgetln(ifile, &ilen))) {
   }

   printf("part one: %d\n", 0);

   fclose(ifile);
   return EXIT_SUCCESS;
}

/* part two: */
int
partTwo(char *fname) {
   FILE *ifile;

   ifile = fopen(fname, "r");
   if (!ifile) {
      printf("could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }

   char *iline;
   size_t ilen;

   while ((iline = fgetln(ifile, &ilen))) {
   }

   printf("part two: %d\n", 0);

   fclose(ifile);
   return EXIT_SUCCESS;
}

/*
 * a valid wire name is one or two lower case letters.
 */

bool
valid_wire_name(char *str) {
   if (strlen(str) < 1 || strlen(str) > 2) {
      return false;
   }
   size_t i;
   for (i = 0; i < strlen(str); i++) {
      if (str[i] < 'a' || str[i] > 'z') {
         return false;
      }
   }
   return true;
}

bool
valid_inp_value(char *str) {
   if (strlen(str) < 1 || strlen(str) > 5) {
      return false;
   }
   size_t i;
   for (i = 0; i < strlen(str); i++) {
      if (str[i] < '0' || str[i] > '9') {
         return false;
      }
   }
   long j = strtol(str, NULL, 10);
   if (j > 65535) {
      return false;
   }
   return true;
}

bool
valid_bit_shift(char *str) {
   if (strlen(str) < 1 || strlen(str) > 2) {
      return false;
   }
   size_t i;
   for (i = 0; i < strlen(str); i++) {
      if (str[i] < '0' || str[i] > '9') {
         return false;
      }
   }
   long j = strtol(str, NULL, 10);
   if (j > 15) {
      return false;
   }
   return true;
}


/*
 * parse an input line producing an intermediate block.
 *
 * since error handling in advent of code is minimal, the only
 * indication of an error is the name of the wire coming back as
 * an empty string.
 *
 * the input format is:
 *
 * (single wire | gate expression | literal value) -> wire name
 */

parsed_t
parse(const char *iline, size_t len) {

   parsed_t pb;                    /* empty result block */
   memset(&pb, 0, sizeof(pb));

   /* the split tokens */
   const size_t MAXTOKLEN = 8;     /* should be enough */
   const size_t MAXTOKENS = 10;    /* arbitrary */
   char tokens[MAXTOKENS][MAXTOKLEN];  /* holding */
   memset(tokens, 0, sizeof(tokens));  /* cleared */

   /* split for ease of access. the second to last token should be our
      assignment operator ->, and the last should be a wire name. if
      we overrun the splits array, it's an error too. */

   char *cpy = strdup(iline);
   char *sep = " \n";              /* space between, newline at end */
   char *tok;                      /* current token */
   size_t numtokens = 0;
   size_t i;

   for(i = 0; i < MAXTOKENS; i++) {
      tok = strtok((i == 0 ? cpy : NULL), sep);
      if (tok == NULL) {
         numtokens = i;
         break;
      }
      strcpy(tokens[i], tok);
   }
   free (cpy);                     /* clean up after yourself */

   /* the minimum valid input is ### -> wire */
   if (numtokens < 3) {
      return pb;
   }

   /* is the tail correct? */
   if (strcmp(tokens[numtokens-2], "->") != 0) {
      return pb;
   }
   if (!valid_wire_name(tokens[numtokens-1])) {
      return pb;
   }

   /* yes, keep track of wire name */
   size_t name_at = numtokens-1;

   /* hard wired signal
    * (signal) (->) (wire)
    */
   if (numtokens == 3 && valid_inp_value(tokens[0])) {
      pb.source_type = se_value;
      strcpy(pb.wire, tokens[name_at]);
      pb.inp_value = strtol(tokens[0], NULL, 10);
      return pb;
   }

   /* wire to wire
    * (wire) (->) (wire)
    */
   if (numtokens == 3 && valid_wire_name(tokens[0])) {
      pb.source_type = se_wire;
      strcpy(pb.wire, tokens[name_at]);
      strcpy(pb.inp_wire, tokens[0]);
      return pb;
   }

   /* the general pattern for these is to first check for a gate type
      if we think this is a gate connection, do further checks for
      valid data. if the data is valid, plug it in. regardless, if the
      connection looked enough like a gate to check for validity, we
      return to caller. */

   /* and or gates
    * (wire) (AND) (wire) (->) (wire)
    */
   if (numtokens == 5 && (strcmp(tokens[1], "AND") == 0 || strcmp(tokens[1], "OR") == 0)) {
      if (valid_wire_name(tokens[0]) && valid_wire_name(tokens[2])) {
         pb.source_type = se_gate;
         pb.inp_gate.type = (strcmp(tokens[1], "AND") == 0) ? ge_and : ge_or;
         strcpy(pb.inp_gate.wire1, tokens[0]);
         strcpy(pb.inp_gate.wire2, tokens[2]);
         strcpy(pb.wire, tokens[name_at]);
      }
      return pb;
   }

   /* shifters
    * (wire) (LSHIFT) (bits) (->) (wire)
    */
   if (numtokens == 5 && (strcmp(tokens[1], "LSHIFT") == 0 || strcmp(tokens[1], "RSHIFT") == 0)) {
      if (valid_wire_name(tokens[0]) && valid_bit_shift(tokens[2])) {
         pb.source_type = se_gate;
         pb.inp_gate.type = (strcmp(tokens[1], "LSHIFT") == 0) ? ge_lshift : ge_rshift;
         strcpy(pb.inp_gate.wire1, tokens[0]);
         pb.inp_gate.bit_shift = strtol(tokens[2], NULL, 10);
         strcpy(pb.wire, tokens[name_at]);
      }
      return pb;
   }

   /* not
    * (NOT) (wire) (->) (wire)
    */
   if (numtokens == 4 && strcmp(tokens[0], "NOT") == 0 && valid_wire_name(tokens[1])) {
      pb.source_type = se_gate;
      strcpy(pb.wire, tokens[name_at]);
      pb.inp_gate.type = ge_not;
      strcpy(pb.inp_gate.wire1, tokens[1]);
      return pb;
   }

   /* if there's a hole in the bucket, pb.wire will still be NUL and
      that is the error signal back to callers. */

   return pb;
}
