/* solution.c -- aoc 2016 20 -- troy brumley */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

#define TXBSTR_IMPLEMENTATION
#include "txbstr.h"

#define TXBMISC_IMPLEMENTATION
#include "txbmisc.h"

#define BITS ((uint64_t)8)
#define BITMASK(b) (1 << (((uint64_t)(b) % BITS)))
#define BITSLOT(b) (((uint64_t)(b)) / BITS)
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) (((uint64_t)(nb) + BITS - 1) / BITS)

#define IP_MIN 0
#define IP_MAX 4294967295

long ip_min = 0;
long ip_max = 4294967295;

typedef struct ip_blacklist_range ip_blacklist_range;
struct ip_blacklist_range {
	long ip_min;
	long ip_max;
};

#define RANGES_MAX 2000
long num_ranges = 0;
ip_blacklist_range *ranges = NULL;

/*
 * part one:
 *
 */

int
fn_cmp_range_asc(const void *a, const void *b) {
	ip_blacklist_range *ra = (ip_blacklist_range *)a;
	ip_blacklist_range *rb = (ip_blacklist_range *)b;
	long min_cmp = ra->ip_min - rb->ip_min;
	if (min_cmp != 0)
		return min_cmp;
	return ra->ip_max - rb->ip_max;
}

int
part_one(
	const char *fname
) {

	FILE *ifile = fopen(fname, "r");
	if (!ifile) {
		fprintf(stderr, "error: could not open file: %s\n", fname);
		return EXIT_FAILURE;
	}

	long c = 4294967295;
	long n = BITNSLOTS(c);
	uint8_t *bits = malloc(n);
	long max_seen = 0;
	long min_seen = 0xFFFFFFFF;
	memset(bits, 0xff, n);
	assert(bits);

	ranges = malloc(RANGES_MAX *sizeof(*ranges));
	memset(ranges, 0, RANGES_MAX *sizeof(*ranges));
	char iline[INPUT_LINE_MAX];
	memset(iline, 0, INPUT_LINE_MAX);
	num_ranges = 0;
	long min_ip = -1;
	long max_ip = 0;
	while (fgets(iline, INPUT_LINE_MAX - 1, ifile)) {
		const char **tokens = split_string(iline, "-\n");
		ranges[num_ranges].ip_min = strtol(tokens[1], NULL, 10);
		ranges[num_ranges].ip_max = strtol(tokens[2], NULL, 10);
		for (long i = ranges[num_ranges].ip_min; i <= ranges[num_ranges].ip_max; i++)
			BITCLEAR(bits, i);
		min_ip = min(ranges[num_ranges].ip_min, min_ip);
		max_ip = max(ranges[num_ranges].ip_max, max_ip);
		num_ranges += 1;
		free_split(tokens);
	}
	/* i thought about sorting and doing things by checking for gaps and
	 * overlaps, but this is fast enough as is. */
	qsort(ranges, num_ranges, sizeof(ip_blacklist_range), fn_cmp_range_asc);

	long res_one = -1;
	long res_two = 0;
	for (long i = 0; i < max_ip; i++) {
		if (BITTEST(bits, i)) {
			res_two += 1;
			if (res_one < 0)
				res_one = i;
		}
	}
	/* printf("\n"); */
	/* printf("min %ld\n", min_ip); */
	/* printf("max %ld\n", max_ip); */
	/* printf("num %ld\n", num_ranges); */
	printf("part one: %ld\n", res_one);
	printf("part two: %ld\n", res_two);
	free(ranges);
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
	return EXIT_SUCCESS;
}
