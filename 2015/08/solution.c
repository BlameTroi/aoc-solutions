/* solution.c -- aoc 2015 08 -- troy brumley */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TXBMISC_IMPLEMENTATION
#include "txbmisc.h"

#include "solution.h"


/* how long is the input line as a compiled string */

int
compiled_string_length(
        const char *c
) {
	int i = 0;

	/* skip the opening quote */
	if (*c == '"')
		c++;

	/* count each character up until ending \0, then trim that last
	   quote off the count as well. */
	while (*c) {
		i += 1;
		switch (*c) {
		case '\\':
			c += 1;
			switch (*c) {
			case '\\':
			case '\"':
				c += 1; break;
			case 'x':
				c += 3; break;
			default:
				assert(NULL);
			}
			break;
		default:
			c += 1;
		}
	}

	/* is the prior character a quote or newline? */
	if (*(c-1) == '\n') {
		i -= 1;
		c -= 1;
	}
	if (*(c-1) == '"')
		i -= 1;

	return i;
}


/* how long is the text in code */

int
source_string_length(
        const char *c
) {
	int i = 0;
	while (*c) {
		switch (*c) {
		case ' ':
		case '\n':
			break;
		default:
			i += 1;
		}
		c += 1;
	}

	return i;
}


/*
- "" encodes to "\"\"", an increase from 2 characters to 6.
- "abc" encodes to "\"abc\"", an increase from 5 characters to 9.
- "aaa\"aaa" encodes to "\"aaa\\\"aaa\"", an increase from 10 characters to 16.
- "\x27" encodes to "\"\\x27\"", an increase from 6 characters to 11.
*/

/* In addition to finding the number of characters of code, you should
   now encode each code representation as a new string and find the
   number of characters of the new encoded representation, including
   the surrounding double quotes. */

/* re-encode the input string, converting it from free text to the
   proper escaped sequence for a c-like compiler. this creates a new
   string that should be freed when no longer needed. the allocation
   may include some slack bytes at the end, but the string is properly
   ended with a trailing \0. */

char *
encoded_string(
        const char *c
) {

	/* pathological case first */

	if (strlen(c) == 0)
		return strdup("\"\"");

	/* copy and encode character by character. our buffer size guess
	   should be big enough, but if not the buffer will be reallocated
	   as needed */

	int buflen = max(strlen(c)*4 + 2 + 1, 32);
	char *buf = malloc(buflen);
	int bufidx = 0;

	/* opening quote */

	buf[bufidx] = '"';
	bufidx += 1;

	/* for each character */

	while (*c) {

		/* resize if we need to, the 8 byte pad is more than
		   enough for a trailing escaped character (2 bytes)
		   and the closing quote and \0 (another 2 bytes). */

		if (buflen - bufidx < 8) {
			buf = realloc(buf, buflen * 2);
			buflen *= 2;
		}

		/* ignore whitespace, in our input that's only newlines */

		if (*c == '\n') {
			c += 1;
			continue;
		}

		/* handle the easy stuff first */

		if (*c != '\\' && *c != '"') {
			buf[bufidx] = *c;
			bufidx += 1;
			c += 1;
			continue;
		}

		/* escape quotes and slashes */

		/* a backslash becomes two backslashes */

		if (*c == '\\') {
			buf[bufidx] = *c;
			buf[bufidx+1] = *c;
			bufidx += 2;
			c += 1;
			continue;
		}

		/* a quote becomes a backslash quote */

		if (*c == '"') {
			buf[bufidx] = '\\';
			buf[bufidx+1] = *c;
			bufidx += 2;
			c += 1;
			continue;
		}

		/* this should be impossible to reach */

		assert(NULL);
	}

	/* closing quote and trailing null byte */

	buf[bufidx] = '"';
	buf[bufidx+1] = '\0';

	return buf;
}


/* and how long is the encoded string? i allowed for the wrapper
   function but it wasn't really needed. i decided to keep it
   for consistency. */

int
encoded_string_length(
        const char *c
) {
	return strlen(c);
}


/*
 * part one:
 */

int
part_one(
        const char *fname
) {

	FILE *ifile = fopen(fname, "r");
	if (!ifile) {
		printf("could not open file: %s\n", fname);
		return EXIT_FAILURE;
	}

	char iline[INPUT_LEN_MAX];

	int compiledSize = 0;
	int sourceSize = 0;
	while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
		compiledSize += compiled_string_length(iline);
		sourceSize += source_string_length(iline);
	}

	printf("source size: %d\n", sourceSize);
	printf("compiled size: %d\n", compiledSize);
	printf("part one: %d\n", sourceSize - compiledSize);

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
	FILE *ifile;

	ifile = fopen(fname, "r");
	if (!ifile) {
		printf("could not open file: %s\n", fname);
		return EXIT_FAILURE;
	}
	char iline[INPUT_LEN_MAX];

	int originalSize = 0;
	int encodedSize = 0;
	while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
		originalSize += strlen(iline) - 1; /* drop newline */
		char *encoded = encoded_string(iline);
		encodedSize += encoded_string_length(encoded);
		free(encoded);
	}

	printf("original size: %d\n", originalSize);
	printf("encoded size: %d\n", encodedSize);
	printf("part two: %d\n", encodedSize - originalSize);

	fclose(ifile);
	return EXIT_SUCCESS;
}
