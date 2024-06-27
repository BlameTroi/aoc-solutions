/* solution.h -- aoc 2015 07 -- troy brumley */
#ifndef SOLUTION_H
#define SOLUTION_H

#include <stdbool.h>
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

each 'wire' or rvalue has only one input. if this were a program,
123 -> x would read x = 123. so here the rvalue's are targets,
not lvalues.

we should be able to sort this on rvalues and build a structure
of all the wires and their inputs. that's step one.

*/


/*
 * limit macros.
 */


/* wire names are two lower case alpha characters, so allowing for
   NUL we get three. */

#define WIRE_MAX 3

/* an input line is rather short */

#define INPUT_LEN_MAX 64
#define TOKEN_LEN_MAX 10
#define TOKEN_COUNT_MAX 10

/* a circuit can have only so many connections, and only a subset of those
   can carry an input signal to start the circuit. */

#define CONNECTIONS_MAX 512
#define INPUT_VALUES_MAX 10


/*
 * data structures and types for connections and a circuit.
 */


/* a signal is provided to each wire by a gate, another wire, or some
   specific value. */

typedef enum sig_e {
   se_undefined = 0,        /* all enums should have zero for undefined/unset */
   se_gate,
   se_wire,
   se_value
} sig_e;


/* gates perform the functions you would expect. */

typedef enum gate_e {
   ge_undefined = 0,
   ge_and,
   ge_or,
   ge_not,
   ge_lshift,
   ge_rshift
} gate_e;


/* a gate has one or more inputs and one output. for and/or, regardless
   of order of input (16 AND w verse x OR 8) the wire identifier goes
   to wire 1. wire 2 being an empty string means use the mask value. */

typedef struct gate_t {
   gate_e type;                    /* and, or, not, lshift, rshift */
   char wire1[WIRE_MAX];           /* any of the above need at least one wire */
   char wire2[WIRE_MAX];           /* and, or could use two wires or a wire and */
   uint16_t mask;                  /* a mask value. */
   uint8_t bit_shift;              /* shift, 0-15, though 0 is in essence a nop */
} gate_t;


/* a connection is some set of inputs directed to one output wire. the
   connection structure holds all the possibilities, with the source_type
   and somtimes gate_type determining which fields are meaningful in a
   particular connection. */

typedef struct connection_t {
   char wire[WIRE_MAX];             /* who i am */
   bool received_signal;            /* have we? */
   uint16_t signal;                 /* if we have, this is what we got */
   sig_e source_type;               /* where i get my signal from */
   gate_t inp_gate;                 /* if it's a gate, it's described in here */
   char inp_wire[WIRE_MAX];         /* if it's a wire, the name of that wire */
   uint16_t inp_value;              /* if it's a hard coded value, we have that here */
} connection_t;


/* a circuit definition arbitrarily has a max of 512 connections. as
   a wire can have only one input, there should be no duplicates.

   the circuit is fired from connections that don't depend on others.
   so, the any value -> wire connections start the process. in the
   dataset there are two of these among the roughly 300 connections. */

typedef struct circuit_t {
   int num_connections;
   connection_t connections[CONNECTIONS_MAX];
} circuit_t;


/*
 * function prototypes for the problem domain:
 */


/* return an empty circuit board. */

circuit_t *
initCircuit(
   void
);


/* given a connection string, parse and add it to the circuit board.
   they are added sequentially. any error returns false. */

bool
addConnection(
   circuit_t *,         /* address of the circuit board */
   const char *         /* input string from dataset to parse and add */
);


/* parse an input line producing an intermediate block. this is exposed
   here for unit testing, connections should be added through the prior
   addConnection function. any error is reported by setting the output
   connection wire name to an empty string. */

connection_t
parseConnection(
   const char *                 /* input string from dataset to parse and add */
);


/* look up connection in circuit by terminal wire name. */
connection_t *
connectionFor(
   circuit_t *,
   const char *
);


/* flip the on switch to run the circuit. returns false the circuit failed. */

bool
runCircuit(
   circuit_t *             /* address of the circuit board */
);


/* free the circuit board memory. */

void
freeCircuit(
   circuit_t *            /* address of the circuit board */
);


/* the code for each of the day's parts. each expects the name of an input
   dataset or some other string to drive the part's code. the answer for
   the posed problem is printed to stdout. these return EXIT_SUCCESS or
   EXIT_FAILURE. */

int
part_one(
   const char *                       /* a file name */
);

int
part_two(
   const char *                       /* a file name */
);


/* end solution.h */
#endif /* SOLUTION_H */
