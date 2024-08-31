/* solution.c -- aoc 2015 01 -- troy brumley */

/*
 * starting from floor 0 of an infinitely tall building with an
 * infinitely deep basement, santa needs to find the right floor to
 * deliver a package. his instructions are in the form of a list of
 * parenthesis. an open paren means go up one floor. a close paren
 * means go down one floor.
 *
 * the live dataset contains 7,000 parens on one line.
 */


#include <stdlib.h>
#include <stdio.h>

int

main(
	int argc, char **argv
) {
	FILE *ifile;

	if (argc < 2) {
		fprintf(stderr, "usage: %s path-to-input\n", argv[0]);
		return EXIT_FAILURE;
	}

	ifile = fopen(argv[1], "r");
	if (!ifile) {
		fprintf(stderr, "could not open file: %s\n", argv[1]);
		return EXIT_FAILURE;
	}

	int atBasement = 0;       /* which position of input sends santa to
				     the basement for the first time */
	int floor = 0;            /* what is the current floor */
	int parens = 0;           /* parens read/current paren position */
	int ch = 0;               /* input character, could include newlines */

	while (EOF != (ch = fgetc(ifile))) {
		/* part one, move from floor to floor as instructed */
		if (ch == '(') {
			parens += 1;
			floor += 1;
		} else if (ch == ')') {
			parens += 1;
			floor -= 1;
			/* part two, did that move us into the first
			   basement level for the first time? */
			if (atBasement == 0 && floor == -1)
				atBasement = parens;
		} else if (ch != '\n') {
			fprintf(stderr, "error: illegal character %d, aborting\n", ch);
			fclose(ifile);
			return EXIT_FAILURE;
		}
	}

	fprintf(stdout, "after %d parens, santa finds himeself on floor %d.\n", parens, floor);
	fprintf(stdout, "and he first entered the basement on paren %d.\n", atBasement);

	fclose(ifile);

	return EXIT_SUCCESS;
}
