/* solution.c -- aoc 2016 09 -- troy brumley */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

#define TXBSTR_IMPLEMENTATION
#include "txbstr.h"
#define TXBMISC_IMPLEMENTATION
#include "txbmisc.h"
#define TXBSB_IMPLEMENTATION
#include "txbsb.h"
#define TXBRS_IMPLEMENTATION
#include "txbrs.h"
#define TXBPAT_IMPLEMENTATION
#include "txbpat.h"

/*
 * part one:
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

	/* i know the file is around 20k in size */
	char *data_buf = malloc(32767);
	assert(data_buf);
	memset(data_buf, 0, 32767);
	fgets(data_buf, 32766, ifile);

	rscb *compressed = rs_create_string(data_buf);

	/* not needed now that data is in read stream. */
	memset(data_buf, 253, 32767);
	free(data_buf);
	fclose(ifile);

	/* expand into a string buffer, it should be at least as large as
	 * the input. */
	sbcb *uncompressed = sb_create_blksize(rs_length(compressed));

	/* expand compressed runs into a smaller buffer. */
	sbcb *run = sb_create_blksize(8192);

	/* read, process, output ... same as it always was */
	int c;
	while ((c = rs_getc(compressed)) != EOF) {

		/* if not in a run, just send straight to output */
		if (c != '(') {
			if (c > ' ')
				sb_putc(uncompressed, c);
			continue;
		}

		/* read run length ... (lengthxtimes) */
		int run_length = 0;
		int repeat_count = 0;
		while (is_digit(c = rs_getc(compressed)))
			run_length = run_length * 10 + (c - '0');
		assert(c == 'x');
		while (is_digit(c = rs_getc(compressed)))
			repeat_count = repeat_count * 10 + (c - '0');
		assert(c == ')');

		/* copy run */
		sb_reset(run);
		while (run_length) {
			assert(!rs_at_end(compressed));
			sb_putc(run, rs_getc(compressed));
			run_length -= 1;
		}

		/* write out repeat times */
		char *temp = sb_to_string(run);
		while (repeat_count) {
			sb_puts(uncompressed, temp);
			repeat_count -= 1;
		}
		memset(temp, 253, strlen(temp));
		free(temp);
	}

	printf("input length: %lu\n", rs_length(compressed));

	printf("part one: %lu\n", sb_length(uncompressed));

	free(uncompressed);
	free(compressed);
	fclose(ifile);

	return EXIT_SUCCESS;
}


/*
 * part two:
 */

int
part_two(
	const char *fname
) {

	FILE *ifile = fopen(fname, "r");
	if (!ifile) {
		fprintf(stderr, "error: could not open file: %s\n", fname);
		return EXIT_FAILURE;
	}

	/* i know the file is around 20k in size. buffer and load
	 * into a read stream, then release the buffer and close
	 * the file. */
	char *data_buf = malloc(32767);
	assert(data_buf);
	memset(data_buf, 0, 32767);
	fgets(data_buf, 32766, ifile);
	fclose(ifile);

	size_t length = expand((const char *)data_buf);

	printf("part two: %lu\n", length);

	memset(data_buf, 253, 32767);
	free(data_buf);

	return EXIT_SUCCESS;
}

/*
 * a compression marker is formatted '(~run_length~x~repeat_count~)'
 */

size_t
interpret_marker(rscb *rs, size_t *run_length, size_t *repeat_count) {
	*run_length = 0;
	size_t marker_length = 1; /* opening ( consumed by caller */
	int c;
	while (is_digit(c = rs_getc(rs))) {
		marker_length += 1;
		*run_length = *run_length * 10 + (c - '0');
	}
	marker_length += 1;
	assert(c == 'x');
	*repeat_count = 0;
	while (is_digit(c = rs_getc(rs))) {
		marker_length += 1;
		*repeat_count = *repeat_count * 10 + (c - '0');
	}
	marker_length += 1;
	assert(c == ')');
	return marker_length;
}

/*
 * calculate the expanded length, handling compression markers
 * '(charsxrepeat)' recursively. to keep the code managable, input is
 * assumed to be syntactically and semantically correct. a few asserts
 * are used to detect errors, but they would be caused by a bug here
 * and not in the data.
 */

size_t
expand_r(rscb *rs, size_t until) {
	size_t length = 0;
	int c;
	while (!rs_at_end(rs) && (rs_peekc(rs) != EOF) && rs_position(rs) < until) {
		c = rs_getc(rs);
		if (c <= ' ')
			continue;
		if (c != '(') {
			length += 1;
			continue;
		}
		size_t run_length = 0;
		size_t repeat_count = 0;
		interpret_marker(rs, &run_length, &repeat_count);
		size_t pos = rs_position(rs);
		length += expand_r(rs, pos + run_length) * repeat_count;
	}
	return length;
}

size_t
expand(const char *str) {
	rscb *rs = rs_create_string(str);
	size_t len = expand_r(rs, strlen(str));
	return len;
}
