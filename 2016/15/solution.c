/* solution.c -- aoc 2016 16 -- troy brumley */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

static int discs_one[8][2] = { /* { size, starting position } */
	{ 13, 10 },
	{ 17, 15 },
	{ 19, 17 },
	{ 7, 1 },
	{ 5, 0 },
	{ 3, 1 },
	{ -1, -1 },
	{ -1, -1 }
};

static int discs_two[8][2] = { /* { size, starting position } */
	{ 13, 10 },
	{ 17, 15 },
	{ 19, 17 },
	{ 7, 1 },
	{ 5, 0 },
	{ 3, 1 },
	{ 11, 0 },      /* part two adds another disk */
	{ -1, -1 }
};

static int discs[8][2];

/*
 * this is a lazy brute force approach. i have seen a few
 * different takes on the problem, but until i dig in and
 * learn the chinese remainder algorithm for real, i'm
 * just blasting this out as is.
 */

long
spinner(int part) {

	/* i don't like pointes to doubly dimensioned arrays */
	if (part == 1)
		memcpy(discs, discs_one, sizeof(discs));

	else
		memcpy(discs, discs_two, sizeof(discs));

	long num_disks = 0;
	for (long i = 0; discs[i][0] != -1; i++)
		num_disks += 1;
	long i = 0;
	bool done = false;
	while (!done) {
		i += 1;
		done = true;
		for (long j = 0; j < num_disks; j++) {
			if ((discs[j][1] + i + j + 1) % discs[j][0] != 0) {
				done = false;
				break;
			}
		}
	}
	return i;
}


/*
 * part one:
 */

int
part_one(
	const char *fname
) {

	printf("part one: %ld\n", spinner(1));

	return EXIT_SUCCESS;
}


/*
 * part two:
 */

int
part_two(
	const char *fname
) {

	printf("part two: %ld\n", spinner(2));

	return EXIT_SUCCESS;
}
