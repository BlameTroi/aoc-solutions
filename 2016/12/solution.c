/* solution.c -- aoc 2016 12 -- troy brumley */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"
#define TXBSTR_IMPLEMENTATION
#include "txbstr.h"

char *
disassembled(instruction n, char *buffer, int buflen) {
	char *sopc;
	char sopr1[32];
	memset(sopr1, 0, 32);
	char sopr2[32];
	memset(sopr2, 0, 32);
	switch (n.opc) {
	case inv:
		sopc = "inv";
		break;
	case cpyr:
		sopc = "cpy";
		snprintf(sopr1, 31, "%c", n.op1 + 'a');
		snprintf(sopr2, 31, "%c", n.op2 + 'a');
		break;
	case cpyv:
		sopc = "cpy";
		snprintf(sopr1, 31, "%d", n.op1);
		snprintf(sopr2, 31, "%c", n.op2 + 'a');
		break;
	case inc:
		sopc = "inc";
		snprintf(sopr1, 31, "%c", n.op1 + 'a');
		break;
	case dec:
		sopc = "dec";
		snprintf(sopr1, 31, "%c", n.op1 + 'a');
		break;
	case jnzr:
		sopc = "jnz";
		snprintf(sopr1, 31, "%c", n.op1 + 'a');
		snprintf(sopr2, 31, "%d", n.op2);
		break;
	case jnzv:
		sopc = "jnz";
		snprintf(sopr1, 31, "%d", n.op1);
		snprintf(sopr2, 31, "%d", n.op2);
		break;
	case nop:
		sopc = "nop";
		break;
	}
	snprintf(
	        buffer, buflen, "%s %s %s",
	        sopc, sopr1, sopr2);
	return buffer;
}

char *
trace_line(cpu *m, char *buffer, int buflen) {
	char dis[256];
	memset(dis, 0, 256);
	snprintf(
	        buffer, buflen, "[%8d] [%8d] [%8d] [%8d]  %d->%s",
	        m->registers[a], m->registers[b], m->registers[c], m->registers[d],
	        m->ip, disassembled(m->core[m->ip], dis, 255));
	return buffer;
}

bool
is_register_operand(const char *opr) {
	if (strlen(opr) > 1)
		return false;
	return opr[0] == 'a' || opr[0] == 'b' || opr[0] == 'c' || opr[0] == 'd';
}

instruction
assemble(char *str) {
	instruction res;
	memset(&res, 0, sizeof(res));
	const char **tokens = split_string(str, " \n");
	/* printf("\nsrc: %s", str); */
	if (equal_string("cpy", tokens[1])) {
		/* cpy val reg -or- cpy reg reg */
		if (is_register_operand(tokens[2])) {
			res.opc = cpyr;
			res.op1 = tokens[2][0] - 'a';
		} else {
			res.opc = cpyv;
			res.op1 = strtol(tokens[2], NULL, 10);
		}
		res.op2 = tokens[3][0] - 'a';
	} else if (equal_string("inc", tokens[1])) {
		/* inc reg */
		res.opc = inc;
		res.op1 = tokens[2][0] - 'a';
	} else if (equal_string("dec", tokens[1])) {
		/* dec reg */
		res.opc = dec;
		res.op1 = tokens[2][0] - 'a';
	} else if (equal_string("jnz", tokens[1])) {
		/* jnz reg val -or- jnz lit val */
		if (is_register_operand(tokens[2])) {
			res.opc = jnzr;
			res.op1 = tokens[2][0] - 'a';
		} else {
			res.opc = jnzv;
			res.op1 = strtol(tokens[2], NULL, 10);
		}
		res.op2 = strtol(tokens[3], NULL, 10);
	} else
		res.opc = nop;
	free_split(tokens);
	/* char buffer[256]; */
	/* memset(buffer, 0, 256); */
	/* printf("dis: %s\n", disassembled(res, buffer, 255)); */
	return res;
}

cpu *
load(FILE *ifile) {
	cpu *m = malloc(sizeof(cpu));
	memset(m, 0, sizeof(cpu));

	char iline[INPUT_LINE_MAX];

	m->ip = 0;
	while (fgets(iline, INPUT_LINE_MAX - 1, ifile)) {
		m->core[m->ip] = assemble(iline);
		m->ip += 1;
	}
	m->core[m->ip].opc = inv;

	return m;
}

bool
execute(cpu *m) {
	m->ip = 0;
	int n;
	char buffer[256];
	memset(buffer, 0, 256);
	instruction *ip = &m->core[m->ip];
	while (ip->opc != inv) {
		/* printf("%s\n", trace_line(m, buffer, 255)); */
		switch (ip->opc) {
		case cpyr:
			m->registers[ip->op2] = m->registers[ip->op1];
			n = 1;
			break;
		case cpyv:
			m->registers[ip->op2] = ip->op1;
			n = 1;
			break;
		case inc:
			m->registers[ip->op1] += 1;
			n = 1;
			break;
		case dec:
			m->registers[ip->op1] -= 1;
			n = 1;
			break;
		case jnzr:
			n = m->registers[ip->op1] == 0 ? 1 : ip->op2;
			break;
		case jnzv:
			n = ip->op1 == 0 ? 1 : ip->op2;
			break;
		default:
			return false;
		}
		m->ip += n;
		ip = &m->core[m->ip];
	}
	return true;
}

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

	cpu *m = load(ifile);
	int ok = execute(m);

	if (ok)
		printf("part one: %d\n", m->registers[a]);

	else
		printf("part one: error\n");

	fclose(ifile);
	return ok ? EXIT_SUCCESS : EXIT_FAILURE;
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

	cpu *m = load(ifile);
	/* for part two, c is 1 not 0 at start */
	m->registers[c] = 1;
	int ok = execute(m);

	if (ok)
		printf("part two: %d\n", m->registers[a]);

	else
		printf("part two: error\n");


	fclose(ifile);
	return EXIT_SUCCESS;
}
