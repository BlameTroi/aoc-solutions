/*  solution.c -- aoc 2015 04 -- troy brumley */

/*
 * santa needs some crypto currency!
 *
 * find MD5 hashes which (as hex digits) start with at least five
 * zeroes. we are given a secret key. find the lowest positive number
 * that produces such a hash when appended to the key.
 *
 * example: if key is abcdef the smallest number that gets the desired
 * hash pattern is 609043 (000001dbbfa...).
 *
 * accept the key from the command line. there is nothing cute to do
 * here, the solution is brute force.
 */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TXBMD5_IMPLEMENTATION
#include "txbmd5.h"

/*
 * maximum length of the text to digest. it actually can't be more
 * than 21 + length of key, but memory is rather cheap.
 */

#define HASH_MAX (1024)

int
main(
        int argc,
        const char **argv
) {
	if (argc < 2) {
		fprintf(stderr, "usage: %s secret-key\n", argv[0]);
		return EXIT_FAILURE;
	}

	const char *base_key = argv[1];
	int len_key = strlen(base_key);
	char *workarea = malloc(HASH_MAX);
	assert(workarea);
	memset(workarea, 0, HASH_MAX);
	strcpy(workarea, base_key);
	uint64_t odometer = 0;
	unsigned char digest[16];
	bool done5 = false;
	bool done6 = false;

	/*
	 * there is no check for a runaway here. the expectation is
	 * that we will get an answer within the limits of the
	 * problem.
	 */

	while (!done5 || !done6) {
		odometer += 1;
		memset(workarea+len_key, 0, HASH_MAX-1-len_key);
		sprintf(workarea+len_key, "%llu", odometer);
		md5_string(workarea, digest);

		if (digest[0]) continue;
		if (digest[1]) continue;

		if (digest[2] < 16) {
			if (!done5) {
				fprintf(stdout, "(5)odometer: %llu\n", odometer);
				done5 = true;
			}
		}

		if (digest[2]) continue;
		if (!done6) {
			fprintf(stdout, "(6)odometer: %llu\n", odometer);
			done6 = true;
		}
	}

	return EXIT_SUCCESS;
}

void
test(
        char *blarg,
        int bleh
) {
	printf("this is a test");
}

int
incrementer(
	int i
	) {
	return i + 1;
}
