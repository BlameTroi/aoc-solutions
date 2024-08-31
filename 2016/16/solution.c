/* solution.c -- aoc 2016 16 -- troy brumley */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

/*
 * the 'dragon' expand concatenates a bit string a with a 0
 * and then (NOT a) reversed.
 */

char *
dragon_expand(const char *a) {
	long input_len = strlen(a);
	long output_len = input_len + input_len + 1;
	char *res = malloc(output_len + 1);
	memset(res, 0, output_len + 1);
	memcpy(res, a, input_len);
	char *pi = res + input_len;
	char *po = pi + 1;
	*pi = '0';
	pi -= 1;
	while (input_len) {
		*po = (*pi) ^ 0x01;
		po += 1;
		pi -= 1;
		input_len -= 1;
	}
	return res;
}

/*
 * the 'dragon' checksum. every distinct pair of bits gets a bit that
 * gives it an odd parity. 11 -> 1, 00 -> 1, 10 -> 0, 01 -> 0.
 * checksums must be of odd length, so if an even length is produced,
 * take the checksum of the checksum.
 */

char *
dragon_checksum(const char *a) {
	long input_len = strlen(a);
	long work_len = input_len + 1;
	char *work = malloc(work_len);
	strcpy(work, a);
	while (true) {
		char *ip = work;
		char *op = work;
		while (*ip) {
			*op = (*ip == *(ip+1)) ? '1' : '0';
			ip += 2;
			op += 1;
		}
		*op = '\0';
		if (strlen(work) & 1)
			break;
	}
	char *res = malloc(strlen(work) + 1);
	strcpy(res, work);
	free(work);
	return res;
}

/*
 * dragonize an input string of bits to completely fill a specific
 * length and report its checksum.
 */

char *
dragonize(const char *input, long fill_length) {
	char *expanded = malloc(strlen(input) + 1);
	strcpy(expanded, input);
	while (strlen(expanded) < fill_length) {
		char *next = dragon_expand(expanded);
		free(expanded);
		expanded = next;
	}
	expanded[fill_length] = '\0';
	char *checksum = dragon_checksum(expanded);
	free(expanded);
	return checksum;
}

/*
 * part one:
 *
 * rather than a filename, the argument is a string of bits followed by a comma
 * followed by a length. use the expand function to expand the bits to fill the
 * length, then calculate the checksum.
 *
 */

int
part_one(
        const char *fname
) {

	char *input = malloc(strlen(fname) + 1);
	strcpy(input, fname);
	char *p = input;
	while (*p && *p != ',')
		p += 1;
	*p = '\0';
	p += 1;
	int fill_length = strtol(p, NULL, 10);

	char *checksum = dragonize(input, fill_length);
	printf("part one: %s\n", checksum);

	free(input);
	free(checksum);

	return EXIT_SUCCESS;
}


/*
 * part two: bigger output, same problem.
 */

int
part_two(
        const char *fname
) {

	char *input = malloc(strlen(fname) + 1);
	strcpy(input, fname);
	char *p = input;
	while (*p && *p != ',')
		p += 1;
	*p = '\0';
	p += 1;
	/* int fill_length = strtol(p, NULL, 10); */
	long fill_length = 35651584;

	char *checksum = dragonize(input, fill_length);
	printf("part two: %s\n", checksum);

	free(input);
	free(checksum);

	return EXIT_SUCCESS;
}
