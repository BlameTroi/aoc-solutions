/* solution.h -- aoc 2016 23 -- troy brumley */

#ifndef SOLUTION_H
#define SOLUTION_H

#define INPUT_LINE_MAX 4096

#include <stdbool.h>
#include <stdio.h>

enum reg {
	a = 0,
	b,
	c,
	d
};
typedef enum reg reg;

enum opcode {
	inv = 0,
	cpy,
	inc,
	dec,
	jnz,
	tgl,
	nop
};
typedef enum opcode opcode;

typedef struct operand operand;
struct operand {
	bool reg;
	int opr;
};

typedef struct instruction instruction;
struct instruction {
	opcode opc;
	operand op1;
	operand op2;
	bool invalid;
};

typedef struct cpu cpu;
struct cpu {
	int ip;
	int registers[4];
	instruction core[32];
};

instruction
assemble(char *);

cpu *
load(FILE *ifile);

bool
execute(cpu *);

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
