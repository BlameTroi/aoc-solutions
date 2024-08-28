/* solution.c -- aoc 2015 24 -- troy brumley */

/*

to balance the load in santa's sleigh, group the gift packages into
three groups of equal weight, one for each available compartment in
the sleigh. find the "quantum entanglement" (product of weights of
packages) of the smallest sized group. this is the group that goes up
front with santa.

for part two, santa forgot the sleigh has a trunk. rerun for four
groups instead of three.

 * p packages given by weights
 * split into n groups of equal weight
 * group 1 must contain the fewest packages and have the smallest
   quantum entanglement score
 * groups 2 through n can have any number of packages so long as
   the weight of every group is total_weight / n

larger data:
1 2 3 5 7 13 17 19 23 29 31 37 41 43 53 59 61 67 71 73 79 83 89 97 101
103 107 109 113

smaller data:
1 2 3 4 5 7 8 9 10 11

*/

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

#define TXBABORT_IMPLEMENTATION
#include "txbabort.h"
#define TXBPMUTE_IMPLEMENTATION
#include "txbpmute.h"
#define TXBMISC_IMPLEMENTATION
#include "txbmisc.h"

/*
 * part one:
 *
 */

int
part_one(
        const char *fname
)
{

	printf("part one: %lu\n", try_combination(fname, 3));

	return EXIT_SUCCESS;
}


/*
 * part two:
 *
 */

int
part_two(
        const char *fname
)
{
	printf("part two: %lu\n", try_combination(fname, 4));

	return EXIT_SUCCESS;
}

/*
 * the dataset in memory and various stats and controls.
 */

int *packages = NULL;           /* package weights */
int *groupings = NULL;          /* count number of groups that fit */
int num_packages = 0;           /* how many are there */
int total_weight = 0;           /* total weight */
int target_groupings = 0;       /* how many groups of packages */
int target_weight = 0;          /* target weight of a group, total / groups */
int min_packages = 0;           /* smallest possible grouping */
int max_packages = 0;           /* largest allowed grouping */

/*
 * free dangling arrays from prior iterations and zero out the data.
 * load_data will rebuild.
 */

void
zero_data(void)
{
	if (packages) {
		free(packages);
		packages = NULL;
	}
	if (groupings) {
		free(groupings);
		groupings = NULL;
	}
	num_packages = 0;
	total_weight = 0;
	target_groupings = 0;
	target_weight = 0;
	min_packages = 0;
	max_packages = 0;
}

/*
 * load the list of package weights, sort it, and gather some basic
 * metrics.
 */

bool
load_data(
        const char *fname,
        bool ascending,
        int groups
)
{

	zero_data();

	/* make a pass through the dataset to find array sizes and verify
	 * that the data is usable. */

	FILE *ifile = fopen(fname, "r");
	if (!ifile) {
		fprintf(stderr, "error: could not open file: %s\n", fname);
		return false;
	}
	char iline[INPUT_LEN_MAX];
	while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
		total_weight += strtol(iline, NULL, 10);
		num_packages += 1;
	}

	if (total_weight % groups != 0) {
		fprintf(stderr, "error: weight is not evenly divisible by %d\n", groups);
		fclose(ifile);
		return false;
	}

	/* allocate arrays and zero them out */

	packages = malloc(num_packages *sizeof(int));
	memset(packages, 0, num_packages *sizeof(int));
	groupings = malloc(num_packages *sizeof(int));
	memset(groupings, 0, num_packages *sizeof(int));

	/* load the package weights */

	rewind(ifile);
	int i = 0;
	while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
		packages[i] = strtol(iline, NULL, 10);
		i += 1;
	}
	fclose(ifile);

	/* sort into requested order, the data was presented in ascending
	 * order but we'll resort regardless. */

	qsort(packages,
	      num_packages,
	      sizeof(int),
	      ascending ? fn_cmp_int_asc : fn_cmp_int_dsc);

	/* set objectives and report */

	target_weight = total_weight / groups;
	target_groupings = groups;
	max_packages = num_packages / groups;
	if (num_packages % groups == 0)
		max_packages -= 1;

	/* there is a realistic minimum of packages ... that being the
	 * number of the largest packages that equal or exceed the target
	 * weight. */

	int start_from = 0;
	int incr_decr = 1;
	int accum = 0;
	if (ascending) {
		start_from = num_packages - 1;
		incr_decr = -1;
	}
	i = start_from;
	min_packages = 0;
	while (accum < target_weight) {
		accum += packages[i];
		i += incr_decr;
		min_packages += 1;
	}
	return true;
}

/*
 * "quantum entanglement" is a discriminator among package groups of
 * the same size. a lower qe is better. for the problem we report the
 * lowest entanglement of the smallest possible group.
 */

unsigned long
quantum_entanglement(
        int n,
        int *p
)
{
	unsigned long qe = 1;
	for (int i = 0; i < n; i ++)
		qe *= packages[p[i]];
	return qe;
}

/*
 * since i already have a permutation generator in my tool box, my
 * first approach was to do permutations even though ordering is not a
 * requirement. it works but it's too slow.
 */

unsigned long
try_permutation(
        const char *data_file_name,
        int groups
)
{
	load_data(data_file_name, true, groups);
	if (num_packages > 15) {
		fprintf(stderr, "error: too many packages for permutation %d", num_packages);
		return -1;
	}
	int target_size_max = (num_packages / groups);
	int ordering[num_packages];
	unsigned long group_qe[num_packages][2];
	for (int i = 0; i < num_packages; i++) {
		ordering[i] = i;
		group_qe[i][0] = 0;
		group_qe[i][1] = INT_MAX;
	}
	int possibilities = 0;
	while (permute_next(num_packages, ordering)) {
		int current_weight = 0;
		for (int i = 0; i < target_size_max; i++) {
			current_weight += packages[ordering[i]];
			if (current_weight < target_weight)
				continue;
			else if (current_weight > target_weight)
				break;

			/* exact match on weight, let's trust that the remaining
			 * packages can be split evenly by weight, and let's go
			 * for a qe calculation. */

			possibilities += 1;
			group_qe[i][0] += 1;
			group_qe[i][1] = min(group_qe[i][1], quantum_entanglement(i+1, ordering));
		}
	}
	int j = -1;
	int k = INT_MAX;
	for (int i = 0; i < num_packages; i++) {
		if (group_qe[i][0] > 0) {
			if (group_qe[i][0] < k) {
				j = i;
				k = group_qe[i][0];
			}
		}
	}
	return group_qe[j][1];
}

/*
 * i next tried my combinations generator which creates all possible
 * combinations of n items, from 1 to n in each combination. it's also
 * a bit slow but hits everything without duplicates. i had the basic
 * code for this already working in a prior day's problem, but it's
 * actually straight forward. if the number of items will fit in a
 * standard type (unsigned long in this case), then just count through
 * all the possible values 0 -> (2^n)-1. if the bit in the current
 * value is set, use that package.
 *
 * this presented two obvious optimizations:
 *
 * first, since we have a computable maximum number of packages in the
 * result group, if more bits than that are set, don't check this
 * group.
 *
 * second, by sorting the data it is possible to exclude any result where
 * the bits indicating the heaviest m items (where sum of m package weights
 * is greather than the target weight). the first optimization will catch
 * these as well, but it's just a bit faster to skip based on this first.
 * bitwise and is faster than the loop to count bits.
 *
 * checking this reduced runtime from 75 down to 10 seconds, not too
 * shabby. it caused us to skip 520381366 of the 536870911 possible
 * combinations of 29 items when checking for too many bits in the
 * pattern.
 *
 * the timing above was with address sanitize turned on. turning it
 * off and enabling optimization -O2 runs 13 seconds without the
 * clipping, and 1 second with.
 */

unsigned long
try_combination(
        const char *data_file_name,
        int groups
)
{

	load_data(data_file_name, false, groups);

	/* either or combinations like this are counting in up binary,
	 * bit on means use this bucket. */

	unsigned long combinator = (1 << num_packages) - 1;

	unsigned long combinations = 0;

	unsigned long group_qe[num_packages][2];
	for (int i = 0; i < num_packages; i++) {
		group_qe[i][0] = 0;
		group_qe[i][1] = LONG_MAX;
	}

	/* determine mask for obviously impossible groups. we'l check this
	 * as a filter before counting bits. */

	unsigned long
	end_mask =
	        (combinator >> (num_packages - min_packages)) << (num_packages - min_packages);

	/* counting down through the possible combinations until we hit
	 * zero */

	int skipped_bits = 0;
	int skipped_mask = 0;

	while (combinator != 0) {

		/* skip if all high order bits set. */

		if ((combinator & end_mask) == end_mask) {
			skipped_mask += 1;
			combinator -= 1;
			continue;
		}

		/* skip if too many or too few bits set. */

		int b = one_bits_in(combinator);
		if (b > max_packages || b < min_packages) {
			skipped_bits += 1;
			combinator -= 1;
			continue;
		}

		/* shift right to left through the current combination
		 * identifying containers to use (bit 1). */

		unsigned long shifter = 1;

		/* as a package is used, its bit is turned off in this tracker.
		 * if we don't hit the weight limit exactly, this combination is
		 * invalid. */

		unsigned long tracker = combinator;

		int i = 0;
		int j = 0;

		int dx[num_packages];
		for (j = 0; j < num_packages; j++)
			dx[j] = -1;
		j = 0;

		int left = target_weight;
		while (shifter < combinator && left > 0) {

			/* if this container is available, use it and track it */
			if (shifter & combinator) {
				tracker ^= shifter;
				left -= packages[i];
				dx[j] = i;
				j += 1;
			}
			i += 1;
			shifter <<= 1;
		}

		/* if we used the exact amount, it's a potential fit */

		if (left == 0 && tracker == 0) {
			combinations += 1;
			/* j is post incremented so -1 here */
			group_qe[j-1][0] += 1;
			unsigned long qe = 1;
			for (int k = 0; k < j; k++)
				qe *= packages[dx[k]];
			group_qe[j-1][1] = lmin(group_qe[j-1][1], qe);
		}

		/* next trial combination */
		combinator -= 1;
	}

	int j = -1;
	for (int i = 0; i < num_packages; i++) {
		if (group_qe[i][0] > 0) {
			if (j == -1)
				j = i;
		}
	}
	return group_qe[j][1];
}
