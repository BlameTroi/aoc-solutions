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

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * return an initialized empty circuit board.
 */

circuit_t *
initCircuit() {
   circuit_t * circuit = calloc(sizeof(circuit_t), 1);
   assert(circuit);
   return circuit;
}


/*
 * free any resources used by the circuit board, and the board itself.
 */

void
freeCircuit(
   circuit_t * circuit
) {
   free(circuit);
}


/*
 * add a connection as defined by the input string to the circuit board.
 * they are added sequentially, order is not relevant. returns false
 * on an error (parse failure or running out of room).
 */

int
addConnection(
   circuit_t *circuit,
   const char* iline
) {
   if (circuit->num_connections >= CONNECTIONS_MAX-1) {
      printf("circuit full %d\n", circuit->num_connections);
      return 0;
   }

   connection_t this = parseConnection(iline);
   if (strlen(this.wire) == 0) {
      printf("parse failure\n");
      return 0;
   }

   circuit->connections[circuit->num_connections] = this;
   circuit->num_connections += 1;

   return 1;
}


/*
 * state passed from part one to part two.
 */

char *resultWire = "a";
char *modifyInputWire = "b";
uint16_t partOneResult = 0;


/*
 * part one:
 *
 * load and parse the connections into a circuit and then run it.
 * what signal is on wire a once the circuit is complete?
 *
 * saves data to reconfigure the circuit for part two.
 */

int
partOne(
   const char *fname
) {

   FILE *ifile = fopen(fname, "r");
   if (!ifile) {
      printf("could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }

   char iline[INPUT_LEN_MAX];

   circuit_t *circuit = initCircuit();
   while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
      if (!addConnection(circuit, iline)) {
         printf("error trying to add connection '%s'\n", iline);
         return EXIT_FAILURE;
      }
   }

   runCircuit(circuit);
   connection_t *this = connectionFor(circuit, resultWire);
   partOneResult = this->signal;
   printf("part one: %d\n", this->signal);

   freeCircuit(circuit);

   fclose(ifile);
   return EXIT_SUCCESS;
}


/*
 * part two:
 *
 * "take the signal you got on wire a in part one, override wire b to
 * that signal, and reset the other wires (including wire a). what
 * new signal is ultimately provided to wire a?"
 *
 * since we're already built to reload the dataset, we'll do that
 * and use values saved from partOne to configure the circuit before
 * we run it.
 */

int
partTwo(
   const char *fname
) {
   FILE *ifile;

   ifile = fopen(fname, "r");
   if (!ifile) {
      printf("could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }
   char iline[INPUT_LEN_MAX];

   circuit_t *circuit = initCircuit();
   while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
      if (!addConnection(circuit, iline)) {
         printf("error trying to add connection '%s'\n", iline);
         return EXIT_FAILURE;
      }
   }

   /* modify the input for wire b to be the result from wire a
      in partOne */

   connection_t *this = connectionFor(circuit, modifyInputWire);
   this->inp_value = partOneResult;

   runCircuit(circuit);

   this = connectionFor(circuit, resultWire);
   printf("part two: %d\n", this->signal);

   freeCircuit(circuit);

   fclose(ifile);
   return EXIT_SUCCESS;
}


/*
 * assorted validation predicates.
 */

/* an input wire name is one or two lower case letters. */

int
validWireName(
   const char *str
) {
   int i = strlen(str);
   if (i < 1 || i > 2) {
      return 0;
   }
   while (i > 0) {
      i -= 1;
      if (str[i] < 'a' || str[i] > 'z') {
         return 0;
      }
   }
   return 1;
}


/* a valid input value (signal) is a 16 bit unsigned number. */

bool
validInputValue(
   const char *str
) {
   int i = strlen(str);
   if (i < 1 || i > 5) {
      return 0;
   }
   while (i > 0) {
      i -= 1;
      if (str[i] < '0' || str[i] > '9') {
         return 0;
      }
   }
   long j = strtol(str, NULL, 10);
   if (j > 65535) {
      return 0;
   }
   return 1;
}


/* a valid bit shift value is 1-15. */

bool
validBitShift(
   const char *str
) {

   int i = strlen(str);
   if (i < 1 || i > 2) {
      return 0;
   }
   while (i > 0) {
      i -= 1;
      if (str[i] < '0' || str[i] > '9') {
         return 0;
      }
   }
   long j = strtol(str, NULL, 10);
   if (j > 15) {
      return 0;
   }
   return 1;
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

connection_t
parseConnection(
   const char *iline
) {

   connection_t pb;                    /* empty result block */
   memset(&pb, 0, sizeof(pb));

   /* the split tokens */
   const int MAXTOKLEN = 8;     /* should be enough */
   const int MAXTOKENS = 10;    /* arbitrary */
   char tokens[MAXTOKENS][MAXTOKLEN];  /* holding */
   memset(tokens, 0, sizeof(tokens));  /* cleared */

   /* split for ease of access. the second to last token should be our
      assignment operator ->, and the last should be a wire name. if
      we overrun the splits array, it's an error too. */

   char *cpy = strdup(iline);
   char sep[] = " \n";             /* space between, newline at end */
   char *tok;                      /* current token */
   int numtokens = 0;
   int i;

   for (i = 0; i < MAXTOKENS; i++) {
      tok = strtok((i == 0 ? cpy : NULL), sep);
      if (tok == NULL) {
         numtokens = i;
         break;
      }
      strlcpy(tokens[i], tok, MAXTOKLEN);
   }

   free(cpy);                     /* clean up after yourself */

   /* the minimum valid input is ### -> wire */
   if (numtokens < 3) {
      return pb;
   }

   /* is the tail correct?
    * the format of the tail is "-> wire".
    */
   if (strcmp(tokens[numtokens-2], "->") != 0) {
      return pb;
   }
   if (!validWireName(tokens[numtokens-1])) {
      return pb;
   }

   /* remember position */
   int name_at = numtokens-1;


   /*
    * parse each type of connection definition, returning a valid pb only
    * once we are sure the syntax is correct.
    *
    * the general pattern for these is to first check for a gate type
    * if we think this is a gate connection, do further checks for
    * valid data. if the data is valid, plug it in. regardless, if the
    * connection looked enough like a gate to check for validity, we
    * return to caller.
    */


   /*
    * the two non-gate connections are hard wired signal and a direct
    * wire to wire.
    *
    * (signal) (->) (wire)
    * (wire) (->) (wire)
    */
   if (numtokens == 3) {
      if (validInputValue(tokens[0])) {
         pb.source_type = se_value;
         strlcpy(pb.wire, tokens[name_at], WIRE_MAX);
         pb.inp_value = strtol(tokens[0], NULL, 10);
      } else if (validWireName(tokens[0])) {
         pb.source_type = se_wire;
         strlcpy(pb.wire, tokens[name_at], WIRE_MAX);
         strlcpy(pb.inp_wire, tokens[0], WIRE_MAX);
      }
      return pb;
   }


   /*
    * 'and' and 'or' gates.
    *
    * (wire or mask) (AND|OR) (wire or mask) (->) (wire)
    */
   if (numtokens == 5 && (strcmp(tokens[1], "AND") == 0 || strcmp(tokens[1], "OR") == 0)) {
      if (validWireName(tokens[0]) && validWireName(tokens[2])) {
         strlcpy(pb.inp_gate.wire1, tokens[0], WIRE_MAX);
         strlcpy(pb.inp_gate.wire2, tokens[2], WIRE_MAX);
      } else if (validWireName(tokens[0]) && validInputValue(tokens[2])) {
         strlcpy(pb.inp_gate.wire1, tokens[0], WIRE_MAX);
         pb.inp_gate.mask = strtol(tokens[2], NULL, 10);
      } else if (validInputValue(tokens[0]) && validWireName(tokens[2])) {
         strlcpy(pb.inp_gate.wire1, tokens[2], WIRE_MAX);
         pb.inp_gate.mask = strtol(tokens[0], NULL, 10);
      } else {
         /* this is an error, return the NUL block. */
         return pb;
      }
      /* on of the three valid gate expression forms was true, finish the block
         and return it. */
      pb.source_type = se_gate;
      pb.inp_gate.type = (strcmp(tokens[1], "AND") == 0) ? ge_and : ge_or;
      strlcpy(pb.wire, tokens[name_at], WIRE_MAX);
      return pb;
   }


   /*
    * shifters.
    *
    * (wire) ((L|R)SHIFT) (bits) (->) (wire)
    */
   if (numtokens == 5 && (strcmp(tokens[1], "LSHIFT") == 0 || strcmp(tokens[1], "RSHIFT") == 0)) {
      if (validWireName(tokens[0]) && validBitShift(tokens[2])) {
         pb.source_type = se_gate;
         pb.inp_gate.type = (strcmp(tokens[1], "LSHIFT") == 0) ? ge_lshift : ge_rshift;
         strlcpy(pb.inp_gate.wire1, tokens[0], WIRE_MAX);
         pb.inp_gate.bit_shift = strtol(tokens[2], NULL, 10);
         strlcpy(pb.wire, tokens[name_at], WIRE_MAX);
      }
      return pb;
   }

   /*
    * invert or not.
    *
    * (NOT) (wire) (->) (wire)
    */
   if (numtokens == 4 && strcmp(tokens[0], "NOT") == 0 && validWireName(tokens[1])) {
      pb.source_type = se_gate;
      strlcpy(pb.wire, tokens[name_at], WIRE_MAX);
      pb.inp_gate.type = ge_not;
      strlcpy(pb.inp_gate.wire1, tokens[1], WIRE_MAX);
      return pb;
   }

   /* if there's a hole in the bucket, pb.wire will still be NUL and
      that is the error signal back to callers. */

   return pb;
}


/*
 * return the connection for the named wired. this is a sequential
 * search. since the data is small and unordered, performance isn't
 * an issue.
 */

connection_t *
connectionFor(
   circuit_t *circuit,
   const char *wire
) {
   int i = 0;
   for (i = 0; i < circuit->num_connections; i++) {
      if (strcmp(wire, circuit->connections[i].wire) == 0) {
         return &circuit->connections[i];
      }
   }
   return NULL;
}


/*
 * post routines to update wires if input is available. returns
 * true or false. if the wire has already been posted, the
 * request is ignored.
 */


/* a hard coded value is passed to the wire. this is how the circuit
   starts. just pass it through. */

int
postValueTo(
   circuit_t *circuit,
   connection_t *this
) {
   if (this->received_signal) {
      return this->received_signal;
   }
   this->signal = this->inp_value;
   this->received_signal = 1;
   return this->received_signal;
}


/* a wire to wire connection requires that the input wire is hot (has
   received its signal). */

int
postWireTo(
   circuit_t *circuit,
   connection_t *this
) {
   if (this->received_signal) {
      return this->received_signal;
   }
   connection_t *that = connectionFor(circuit, this->inp_wire);
   if (that->received_signal) {
      this->signal = that->signal;
      this->received_signal = 1;
   }
   return this->received_signal;
}


/* there are five possible gates. in each case, all inputs over
   wires must be hot for the gate to process. */

int
postGateTo(
   circuit_t *circuit,
   connection_t *this
) {
   if (this->received_signal) {
      return this->received_signal;
   }

   connection_t *that = NULL;

   switch (this->inp_gate.type) {

   case ge_and:
   case ge_or:
      /* these have either two wires or one wire and one mask */
      that = connectionFor(circuit, this->inp_gate.wire1);
      if (!that->received_signal) {
         /* first/only wire is not hot */
         break;
      }
      if (strlen(this->inp_gate.wire2) > 0) {
         connection_t *other = connectionFor(circuit, this->inp_gate.wire2);
         if (!other->received_signal) {
            /* still waiting on this wire */
            break;
         }
         this->received_signal = that->received_signal; /* other would work too */
         if (this->inp_gate.type == ge_and) {
            this->signal = that->signal & other->signal;
         } else {
            this->signal = that->signal | other->signal;
         }
         break;
      }
      /* the lone wire is hot, apply mask */
      this->received_signal = that->received_signal;
      if (this->inp_gate.type == ge_and) {
         this->signal = that->signal & this->inp_gate.mask;
      } else {
         this->signal = that->signal | this->inp_gate.mask;
      }
      break;

   case ge_not:
      /* invert the incoming signal */
      that = connectionFor(circuit, this->inp_gate.wire1);
      if (that->received_signal) {
         this->signal = ~that->signal;
         this->received_signal = that->received_signal;
      }
      break;

   case ge_lshift:
   case ge_rshift:
      /* shift in the appropriate direction */
      that = connectionFor(circuit, this->inp_gate.wire1);
      if (that->received_signal) {
         this->signal =
            this->inp_gate.type == ge_lshift ?
            that->signal << this->inp_gate.bit_shift :
            that->signal >> this->inp_gate.bit_shift;
         this->received_signal = that->received_signal;
      }
      break;

   case ge_undefined:
      /* a should not occur */
      assert(NULL);
   }

   return this->received_signal;
}


/*
 * run the circuit.
 *
 * iterate over all the connections until all endpoints have data or
 * we've exhausted our run limit.
 *
 * for the circuit to run there needs to be one or more connections
 * of hard coded signal value to a wire, such as: 123 -> a.
 */

int
runCircuit(
   circuit_t *circuit
) {

   int runaway = 0;
   int notDone = 1;
   while (notDone) {
      notDone = 0;
      /* for each connection that hasn't received a signal, see if we can
         satisfy it's input */
      int i;
      for (i = 0; i < circuit->num_connections; i++) {
         connection_t *this = &circuit->connections[i];
         if (this->received_signal) {
            continue;
         }
         /* printf("%d %d %d\n", runaway, notDone, i); */
         /* if we had to check a connection, we're stilll working */
         notDone += 1;
         switch (this->source_type) {
         case se_gate:
            postGateTo(circuit, this);
            break;
         case se_wire:
            /* check if the other end of the wire is hot */
            postWireTo(circuit, this);
            break;
         case se_value:
            /* simple case, updates signal received immediately. */
            postValueTo(circuit, this);
            break;
         case se_undefined:
            /* this is a should not occur */
            assert(NULL);
            break;
         }
      }
      runaway += 1;
      if (runaway > 9999) {
         printf("runaway loop, breaking out\n");
         break;
      }
   }

   return 0;
}
