/* solution.h -- aoc 2015 23 -- troy brumley */
#ifndef SOLUTION_H
#define SOLUTION_H

/*
 * a very simple computer chip, two registers and a few instructions.
 *
 * registers a and b hold any non-negative integer.
 *
 * hlf r          -- halve the register and continue with next instruction
 * tpl r          -- triple the register and continue with next instruction
 * inc r          -- add 1 to register and continue with next instruction
 * jmp offset     -- jump relative, the jmp is +0, all instructions are length 1
 * jie r, offset  -- jump if register is even
 * jio r, offset  -- jump if register is odd
 *
 * no subtraction? no load?
 *
 * registers start at zero. execution stops at the end of program memory.
 *
 * execute the provided program and report the value in register b.
 *
 */

#include <stdbool.h>

/*
 * the cpu:
 */

#define INV 0
#define HLF 1
#define TPL 2
#define INC 3
#define JMP 4
#define JIE 5
#define JIO 6

#define RA 1
#define RB 2

typedef struct itab itab;

struct itab {
	char *mne;
	int opc;
	bool reg;
	bool offset;
};

typedef struct inst inst;

struct inst {
	int opc;
	int reg;
	int offset;
};

#define RAM_MAX 255

typedef struct cpu cpu;

struct cpu {
	int pic;
	unsigned long a;
	unsigned long b;
	inst ram[RAM_MAX];
};

/* how long may a line in the test dataset be? */
#define INPUT_LEN_MAX 255

cpu *
create(
        int
);

bool
ipl(
        cpu *,
        inst *
);

void
run(
        cpu *
);

inst
assemble(
        char *
);

/*
 * the code for each of the day's parts.
 *
 * each expects the name of an input dataset or some other string to
 * drive the part's code.
 *
 * should print the result to stdout.
 *
 * returns EXIT_SUCCESS or EXIT_FAILURE.
 */


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
