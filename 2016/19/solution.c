/* solution.c -- aoc 2016 19 -- troy brumley */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

/*
 * the elves sit in a circle, and the first elf (1) steals the
 * presents from the elf to his left (2). that elf (2) leaves and the
 * next elf (3) steals the presents from the elf to his left (4).
 * repeat until there is only one elf left with all the presents.
 * which elf is it?
 *
 * this is the josephus problem, and a naive approach using an
 * array like a sieve works well enough even when there are more
 * than three million elves in the circle.
 *
 * for the second part of the problem, instead of stealing from the
 * elf to the left, the gifts are stolen from the elf across from
 * thief. if there are two elves there (odd numbers and all) the
 * elf to the left of the midpoint is the victim (integer math).
 *
 * this does not lend itself to the same naive loop as in part
 * one.
 *
 * the first obvious optimization is that we can immediately kill
 * off the last half of the elves, but that only works once, or
 * does it?
 *
 * big target is to solve for 3018458 elves.
 */

/*
 * so far all approaches use an array as a sieve. we keep wrapping
 * around so a bit of modulus in our lives is inevitable. yes, i
 * modul(us)ate.
 *
 * switched to a macro for the hopeful speedup from removing a
 * possible function call.
 */

/* int */
/* wrap(int i, int n) { */
/*    return i % n; */
/* } */

#define wrap(i, n) ((i) >= (n) ? (i) % (n) : (i))
/* #define wrap(i, n) ((i) % (n)) */

/*
 * when we're working on remaining adjacent elves, simply repeated
 * striking out around the circle works quickly enough.
 */

int
josephus_adjacent(int num) {
	uint8_t *sieve  = malloc(num);
	memset(sieve, 1, num);
	int k = num;          /* remaining */
	int i = 0;            /* current elf */
	int j = 1;            /* victim */
	while (k > 1) {
		j = wrap(i + 1, num);
		while (!sieve[j])
			j = wrap(j + 1, num);
		sieve[j] = 0;
		k -= 1;
		i = wrap(j + 1, num);
		while (!sieve[i])
			i = wrap(i + 1, num);
	}
	free(sieve);
	return i + 1;
}

/*
 * for part two, the elf opposite (left of center if there are two
 * adjacent to the center point) is the victim. the thief is always
 * the next live elf to the left as in the adjacent version.
 *
 * i spent way too long experimenting with shrinking arrays and then
 * gave in and used a documented shortcut for this variation of the
 * josephus problem.
 *
 * the longer solutions using a deque look intereting but i'm under
 * some time pressure (pending trip) so i'll just cheese out with
 * this function.
 */

int
josephus_opposite(int num) {
	int s = 2;
	while (s < num)
		s = s * 3 - 2;
	s = (s + 2) / 3;
	if (s >= num)
		s = s * 2 - 9;
	return num - (s) + 1;
}

/*
 * part one:
 */

int
part_one(
        const char *fname
) {
	printf("part one: %d\n", josephus_adjacent(LIVE));
	return EXIT_SUCCESS;
}


/*
 * part two:
 */

int
part_two(
        const char *fname
) {
	printf("part two: %d\n", josephus_opposite(LIVE));
	return EXIT_SUCCESS;
}
