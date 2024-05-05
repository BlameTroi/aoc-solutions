/* solution.h -- aoc 2015 07 -- troy brumley */
#ifndef SOLUTION_H
#define SOLUTION_H

#include <stddef.h>
#include <stdint.h>

/*
 * some assembly required
 *
 * instructions describe a circuit. logic gates (and, or, not, shifts)
 * are connected by wires. each wire has only one input but can have
 * multiple outputs. each gate only passes on a signal once all its
 * inputs have arrived. finally, a signal value can be given to a
 * wire (turn the circuit on) to start the process.
 */

/*

this is not a program, it is a circuit. gates don't process and send until
all inputs arrive. notifications could be fun.

123 -> x
456 -> y
x AND y -> d
x OR y -> e
x LSHIFT 2 -> f
y RSHIFT 2 -> g
NOT x -> h
NOT y -> i

all tokens are space delimited
lower case text are 'wires' that carry 16 bit unsigned values
integers are 16 bit unsigned values
-> is direct copy
AND OR LSHIFT # RSHIFT # NOT do what you would expect

since a wire has only one input, it can appear on the right
side of -> only once in the stream.

char * strtok (char *string, const char *delimiters);
*/


/*
 * data structures:
 */


/*
 * each 'wire' or rvalue has only one input. if this were a program,
 * 123 -> x would read x = 123. so here the rvalue's are targets,
 * not lvalues.
 *
 * we should be able to sort this on rvalues and build a structure
 * of all the wires and their inputs. that's step one.
 */


/* wire names are two lower case alpha characters, so allowing for
   NUL we get three. */

#define WIRE_MAX 3


/* a signal is provided to each wire by a gate, another wire, or some
   specific value. */

typedef enum sig_e {
   se_gate = 1,
   se_wire,
   se_value
} sig_e;


/* gates perform the functions you would expect. */

typedef enum gate_e {
   ge_and = 1,
   ge_or,
   ge_not,
   ge_lshift,
   ge_rshift
} gate_e;


/* a gate has one or more inputs and one output. */

typedef struct gate_t {
   gate_e type;
   char wire1[WIRE_MAX];           /* and, or not, shifts */
   char wire2[WIRE_MAX];           /* and, or */
   uint8_t bit_shift;              /* shift, 0-16, though 0 is in essence a nop */
} gate_t;


/* parsed input line */

typedef struct parsed_t {
   char wire[WIRE_MAX];             /* who i am */
   sig_e source_type;               /* where i get my signal from */
   gate_t inp_gate;                 /* if it's a gate, it's described in here */
   char inp_wire[WIRE_MAX];         /* if it's a wire, the name of that wire */
   uint16_t inp_value;              /* if it's a hard coded value, we have that here */
} parsed_t;


/*
 * functions:
 */

/*
 * the code for each of the day's parts.
 *
 * each expects the name of an input dataset or
 * some other string to drive the part's code.
 *
 * should print the result to stdout.
 *
 * returns EXIT_SUCCESS or EXIT_FAILURE.
 */

int
partOne(
      char *                       /* a file name */
      );

int
partTwo(
      char *                       /* a file name */
      );

/*
 * parse an input line producing an intermediate block.
 */

parsed_t
parse(
      const char *iline,
      size_t len
      );



/* end solution.h */
#endif /* SOLUTION_H */
