/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"

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

#define MESSAGE_MAX 16
int frequency[MESSAGE_MAX][255];

void
clear_frequency(void) {
	memset(frequency, 0, sizeof(frequency));
}

void
update_frequency(const char *str) {
	assert(strlen(str) <= MESSAGE_MAX);
	for (int i = 0; i < strlen(str); i++)
		frequency[i][(unsigned int)str[i]] += 1;
}

const char *
most_frequent(void) {
	char *res = malloc(MESSAGE_MAX+1);
	memset(res, 0, MESSAGE_MAX+1);
	for (int i = 0; i < MESSAGE_MAX; i++) {
		int mx = 0;
		for (int j = 1; j < 256; j++) {
			if (frequency[i][j] > frequency[i][mx])
				mx = j;
			res[i] = mx;
		}
	}
	return res;
}

MU_TEST(test_contrived) {
	clear_frequency();
	update_frequency("asdf");
	update_frequency("abcd");
	update_frequency("bseg");
	update_frequency("xqwf");
	const char *res = most_frequent();
	printf("\n%s\n", res);
	mu_should(strcmp(res, "ascf") == 0);
	free((void *)res);
}

MU_TEST(test_test) {
	mu_should(datafile);
	rewind(datafile);
	char buffer[1024];
	memset(buffer, 0, 1024);
	clear_frequency();
	while (fgets(buffer, 1023, datafile))
		update_frequency(buffer);
	const char *res = most_frequent();
	printf("\n%s\n", res);
	mu_should(strncmp(res, "easter", 6) == 0);
	free((void *)res);
}

/*
 * here we defini the whole test suite.
 */
MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	/* run your tests here */
	MU_RUN_TEST(test_test);

	MU_RUN_TEST(test_contrived);
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
