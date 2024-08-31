/* solution.c -- aoc 2016 xx -- troy brumley */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

#define TXBSTR_IMPLEMENTATION
#include "txbstr.h"

typedef struct triangle triangle;

struct triangle {
	bool valid;
	int a;
	int b;
	int c;
};

/* original part one, switched to use the same array method as
   part two. */

triangle
get_triangle(char *str) {
	triangle result = { false, 0, 0, 0 };
	const char **tokens = split_string(str, " \n");
	result.a = strtol(tokens[1], NULL, 10);
	result.b = strtol(tokens[2], NULL, 10);
	result.c = strtol(tokens[3], NULL, 10);
	assert(!tokens[4]);
	free((void *)tokens[0]);
	free(tokens);
	return result;
}

bool
validate_triangle(triangle *t) {
	t->valid = (t->a + t->b > t->c) &&
	           (t->a + t->c > t->b) &&
	           (t->b + t->c > t->a);
	return t->valid;
}

#define TRIANGLE_MAX 2000
int data[TRIANGLE_MAX][3];
int num_data = 0;

int
load_data(FILE *ifile) {
	assert(ifile);
	rewind(ifile);
	char buffer[INPUT_LINE_MAX];
	num_data = 0;
	memset(data, 0, sizeof(data));
	while (fgets(buffer, INPUT_LINE_MAX - 1, ifile)) {
		if (strlen(buffer) < 7)
			continue;
		const char **tokens = split_string(buffer, " \n");
		data[num_data][0] = strtol(tokens[1], NULL, 10);
		data[num_data][1] = strtol(tokens[2], NULL, 10);
		data[num_data][2] = strtol(tokens[3], NULL, 10);
		assert(!tokens[4]);
		num_data += 1;
		free((void *)tokens[0]);
		free(tokens);
	}
	return num_data;
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

	if (load_data(ifile) < 1)
		fprintf(stderr, "error: could not load from file: %s\n", fname);

	int valid = 0;
	for (int i = 0; i < num_data; i++) {
		triangle t = { false, data[i][0], data[i][1], data[i][2] };
		if (validate_triangle(&t))
			valid += 1;
	}

	printf("part one: %d\n", valid);

	fclose(ifile);
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

	if (load_data(ifile) < 1)
		fprintf(stderr, "error: could not load from file: %s\n", fname);

	int valid = 0;
	int i = 0;
	while (i < num_data) {
		triangle t0 = { false, data[i+0][0], data[i+1][0], data[i+2][0] };
		triangle t1 = { false, data[i+0][1], data[i+1][1], data[i+2][1] };
		triangle t2 = { false, data[i+0][2], data[i+1][2], data[i+2][2] };
		if (validate_triangle(&t0))
			valid += 1;
		if (validate_triangle(&t1))
			valid += 1;
		if (validate_triangle(&t2))
			valid += 1;
		i += 3;
	}
	assert(i == num_data);

	printf("part two: %d\n", valid);

	fclose(ifile);
	return EXIT_SUCCESS;
}
