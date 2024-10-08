/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"

#include "txbmd5.h"

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

void
print_md5_digest(uint8_t *digest) {
	printf("\ndigest: ");
	for (int i = 0; i < 4; i++)
		printf("%02X", digest[i]);
	printf(" ");
	for (int i = 4; i < 8; i++)
		printf("%02X", digest[i]);
	printf("  ");
	for (int i = 8; i < 12; i++)
		printf("%02X", digest[i]);
	printf(" ");
	for (int i = 12; i < 16; i++)
		printf("%02X", digest[i]);
	printf("\n");
}

MU_TEST(test_test) {
	printf("\n");
	char *str1 = "abc3231929";
	uint8_t result[16];
	md5_string(str1, result);
	print_md5_digest(result);
	mu_should((result[2] & 0x0f) == 1);
	char *str2 = "abc5017308";
	md5_string(str2, result);
	print_md5_digest(result);
	mu_should((result[2] & 0x0f) == 8);
	char *str3 = "abc5278568";
	md5_string(str3, result);
	print_md5_digest(result);
	mu_should((result[2] & 0x0f) == 0x0f);
}

/*
 * here we defini the whole test suite.
 */
MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	/* run your tests here */
	MU_RUN_TEST(test_test);
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
