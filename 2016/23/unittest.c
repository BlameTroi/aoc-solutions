/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"
#include "solution.h"

/*
 * minunit setup and teardown of infrastructure.
 */

bool got_args = false;
int num_args = 0;
char **the_args = NULL;
char *filename = NULL;
FILE *datafile = NULL;
char *test_string = NULL;

/*
 * unittest (filename | -f filename) | -e string ...
 */

bool
get_args(void) {
	if (num_args < 2)
		got_args = true;
	if (got_args)
		return true;

	if (num_args == 2) {
		filename = the_args[1];
		got_args = true;
		return got_args;
	}

	if (num_args > 2 && strcmp(the_args[1], "-f") == 0) {
		filename = the_args[2];
		got_args = true;
		return got_args;
	}

	if (strcmp(the_args[1], "-i") != 0) {
		fprintf(stderr, "error: illegal arguments to unittest\n");
		exit(EXIT_FAILURE);
	}

	int len = num_args;
	for (int i = 2; i < num_args; i++)
		len += strlen(the_args[i]);
	test_string = malloc(len+1);
	memset(test_string, 0, len+1);
	for (int i = 2; i < num_args; i++) {
		if (i != 2)
			strcat(test_string, " ");
		strcat(test_string, the_args[i]);
	}

	got_args = true;
	return got_args;
}

void
test_setup(void) {
	get_args();
	if (filename) {
		datafile = fopen(filename, "r");
		if (!datafile) {
			fprintf(stderr, "error: could not open test data file!\n");
			exit(EXIT_FAILURE);
		}
	}
}

void
test_teardown(void) {
	if (datafile)
		fclose(datafile);
}

/*
 * sample test shell.
 */

/*
 day 23 additional instruction

tgl x toggles the instruction x away (pointing at instructions like
jnz does: positive means forward; negative means backward):

* For one-argument instructions, inc becomes dec, and all other
  one-argument instructions become inc.

* For two-argument instructions, jnz becomes cpy, and all other
  two-instructions become jnz.

* The arguments of a toggled instruction are not affected.

* If an attempt is made to toggle an instruction outside the program,
  nothing happens.

* If toggling produces an invalid instruction (like cpy 1 2) and an
  attempt is later made to execute that instruction, skip it instead.

* If tgl toggles itself (for example, if a is 0, tgl a would target
  itself and become inc a), the resulting instruction is not executed
  until the next time it is reached.

For example, given this program:

cpy 2 a
tgl a
tgl a
tgl a
cpy 1 a
dec a
dec a

* cpy 2 a initializes register a to 2.

* The first tgl a toggles an instruction a (2) away from it, which
  changes the third tgl a into inc a.

* The second tgl a also modifies an instruction 2 away from it, which
  changes the cpy 1 a into jnz 1 a.

* The fourth line, which is now inc a, increments a to 3.

* Finally, the fifth line, which is now jnz 1 a, jumps a (3)
  instructions ahead, skipping the dec a instructions.

In this example, the final value in register a is 3.


 old day 12 prog
cpy 1 a
cpy 1 b
cpy 26 d
jnz c 2
jnz 1 5
cpy 7 c
inc d
dec c
jnz c -2
cpy a c
inc a
dec b
jnz b -2
cpy c b
dec d
jnz d -6
cpy 14 c
cpy 14 d
inc a
dec d
jnz d -2
dec c
jnz c -5
*/
/*
  typedef enum opcode opcode;
cpy 41 a
inc a
inc a
dec a
jnz a 2
dec a
  */

cpu *
load_test(void) {
	cpu *m = malloc(sizeof(cpu));
	memset(m, 0, sizeof(cpu));
	char *prog[] = {
		"cpy 2 a",
		"tgl a",
		"tgl a",
		"tgl a",
		"cpy 1 a",
		"dec a",
		"dec a",
		NULL
	};
	printf("\n");
	m->ip = 0;
	int i = 0;
	while (prog[i]) {
		m->core[m->ip] = assemble(prog[i]);
		m->ip += 1;
		i += 1;
	}
	m->core[m->ip].opc = inv;

	return m;
}

MU_TEST(test_load) {
	cpu *bunny = load_test();
	int ok = execute(bunny);
	printf(
	        "a %8d  b %8d  c %8d  d %8d\n",
	        bunny->registers[a], bunny->registers[b], bunny->registers[c],
	        bunny->registers[d]
	);
	mu_should(ok);
	mu_should(bunny->registers[a] == 3);
}

MU_TEST(test_inc) {
	cpu *m = malloc(sizeof(cpu));
	memset(m, 0, sizeof(cpu));
	m->registers[a] = 1;
	m->registers[b] = 2;
	m->registers[c] = 3;
	m->registers[d] = -1;
	m->core[0] = assemble("inc a");
	mu_should(m->core[0].opc == inc);
	mu_should(m->core[0].op1.reg);
	mu_should(m->core[0].op1.opr == a);
	m->core[1] = assemble("inc b");
	m->core[2] = assemble("inc c");
	m->core[3] = assemble("inc d");
	m->core[4] = assemble("inc 5"); /* illegal operand */
	mu_should(m->core[4].opc == inc);
	mu_shouldnt(m->core[4].op1.reg);
	mu_should(m->core[4].op1.opr == 5);
	bool ok = execute(m);
	mu_should(m->registers[a] == 2);
	mu_should(m->registers[b] == 3);
	mu_should(m->registers[c] == 4);
	mu_should(m->registers[d] == 0);
	mu_should(ok);
	free(m);
}

MU_TEST(test_dec) {
	cpu *m = malloc(sizeof(cpu));
	memset(m, 0, sizeof(cpu));
	m->registers[a] = 3;
	m->registers[b] = 2;
	m->registers[c] = 1;
	m->registers[d] = -1;
	m->core[0] = assemble("dec a");
	mu_should(m->core[0].opc == dec);
	mu_should(m->core[0].op1.reg);
	mu_should(m->core[0].op1.opr == a);
	m->core[1] = assemble("dec b");
	m->core[2] = assemble("dec c");
	m->core[3] = assemble("dec d");
	m->core[4] = assemble("dec 15"); /* illegal operand */
	mu_should(m->core[4].opc == dec);
	mu_shouldnt(m->core[4].op1.reg);
	mu_should(m->core[4].op1.opr == 15);
	bool ok = execute(m);
	mu_should(m->registers[a] == 2);
	mu_should(m->registers[b] == 1);
	mu_should(m->registers[c] == 0);
	mu_should(m->registers[d] == -2);
	mu_should(ok);
	free(m);
}
MU_TEST(test_cpy) {
	cpu *m = malloc(sizeof(cpu));
	memset(m, 0, sizeof(cpu));
	m->registers[a] = 10;
	m->registers[b] = 11;
	m->registers[c] = 12;
	m->registers[d] = 13;
	m->core[0] = assemble("cpy a b");
	mu_should(m->core[0].opc == cpy);
	mu_should(m->core[0].op1.reg);
	mu_should(m->core[0].op1.opr == a);
	mu_should(m->core[0].op2.reg);
	mu_should(m->core[0].op2.opr == b);
	m->core[1] = assemble("cpy 3 c");
	mu_shouldnt(m->core[1].op1.reg);
	mu_should(m->core[1].op1.opr == 3);
	m->core[2] = assemble("cpy d 9");   /* illegal operand */
	mu_should(m->core[2].op1.reg);
	mu_should(m->core[2].op1.opr == d);
	mu_shouldnt(m->core[2].op2.reg);
	mu_should(m->core[2].op2.opr == 9);
	m->core[3] = assemble("cpy 9 d");   /* legal */
	bool ok = execute(m);
	mu_should(m->registers[a] == 10);
	mu_should(m->registers[b] == 10);
	mu_should(m->registers[c] == 3);
	mu_should(m->registers[d] == 9);
	mu_should(ok);
	free(m);
}
MU_TEST(test_jnz) {
	cpu *m = malloc(sizeof(cpu));
	memset(m, 0, sizeof(cpu));
	m->registers[a] = 15;
	m->registers[b] = 2;
	m->registers[c] = 1;
	m->registers[d] = 0;
	m->core[0] = assemble("dec a");
	m->core[1] = assemble("inc d");
	m->core[2] = assemble("jnz a -2");
	m->core[3] = assemble("inc a");
	bool ok = execute(m);
	mu_should(m->registers[a] == 1);
	mu_should(m->registers[d] == 15);
	mu_should(ok);
	free(m);
}
MU_TEST(test_tgl) {
	cpu *m = malloc(sizeof(cpu));
	memset(m, 0, sizeof(cpu));
	m->core[0] = assemble("cpy 3 a");
	m->core[1] = assemble("cpy 2 b");
	m->core[2] = assemble("cpy -3 c");
	m->core[3] = assemble("cpy 4 d");
	m->core[4] = assemble("tgl c");
	m->core[5] = assemble("dec a");
	m->core[6] = assemble("jnz a -5");
	bool ok = execute(m);
	mu_should(m->registers[a] == 0);
	mu_should(m->registers[b] == 6);
	mu_should(ok);
	free(m);
}

MU_TEST(test_test) {
	instruction c = assemble("cpy 41 a\n");
	mu_should(c.opc == cpy);
	mu_should(c.op1.opr == 41);
	mu_should(c.op2.opr == a);
	mu_should(true);
	mu_shouldnt(false);
}

/*
 * here we defini the whole test suite.
 */
MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	/* run your tests here */
	MU_RUN_TEST(test_inc);
	MU_RUN_TEST(test_dec);
	MU_RUN_TEST(test_cpy);
	MU_RUN_TEST(test_jnz);
	MU_RUN_TEST(test_tgl);
	MU_RUN_TEST(test_test);
	MU_RUN_TEST(test_load);
}

/*
 * master control:
 */

int
main(int argc, char *argv[]) {
	num_args = argc;
	the_args = argv;
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
