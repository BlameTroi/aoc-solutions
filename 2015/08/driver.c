/* driver.c -- aoc 2015 any day driver -- troy brumley */

/*
 * just a wrapper to call the two parts with the name of the input data
 * sets.
 */

#include <stdio.h>
#include <stdlib.h>

#include "solution.h"  /* all we use here are the part_one and part_two functions. */

int
main(
        int argc, const char **argv
)
{

	if (argc < 2) {
		printf("usage: %s path-to-input\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (part_one(argv[1]) != EXIT_SUCCESS) {
		printf("error in part one\n");
		return EXIT_FAILURE;
	}

	if (part_two(argv[1]) != EXIT_SUCCESS) {
		printf("error in part two\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
