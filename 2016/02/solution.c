/* solution.c -- aoc 2016 xx -- troy brumley */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

/* additional data */


/*
 * keypad one is a phone number keypad without
 * special keys or 0.
 */

char keypad_one[3][3] = {
	{'1', '2', '3'},
	{'4', '5', '6'},
	{'7', '8', '9'}
};
char
fn_read_one(int row, int col) {
	return keypad_one[row][col];
}
keypad_def keypad_one_def = {
	3, 3,
	(keypad_pos) {
		1, 1
	},
	fn_read_one
};

/*
 * keypad two is a diamond laid out on a 5x5
 * grid. empty key positions are blanks.
 */

char keypad_two[5][5] = {
	{ ' ', ' ', '1', ' ', ' ' },
	{ ' ', '2', '3', '4', ' ' },
	{ '5', '6', '7', '8', '9' },
	{ ' ', 'A', 'B', 'C', ' ' },
	{ ' ', ' ', 'D', ' ', ' ' }
};
char
fn_read_two(int row, int col) {
	return keypad_two[row][col];
}
keypad_def keypad_two_def = {
	5, 5,
	(keypad_pos) {
		2, 0
	},
	fn_read_two
};

/*
 * give a move (URDL), the current position, and a keypad
 * definition, make the move and update the current position
 * if the move is legal, or don't move and leave the
 * position unchanged if the move is illegal.
 */

void
move_key(keypad_pos *pos, keypad_def def, char move) {
	assert(pos->row >= 0 && pos->row < def.rows &&
	       pos->col >= 0 && pos->col < def.cols);
	if (move == UP) {
		if (pos->row != 0 && def.fnreader(pos->row - 1, pos->col) != EMPTY)
			pos->row -= 1;
		return;
	}
	if (move == RIGHT) {
		if (pos->col < def.cols - 1 && def.fnreader(pos->row, pos->col + 1) != EMPTY)
			pos->col += 1;
		return;
	}
	if (move == DOWN) {
		if (pos->row < def.rows - 1 && def.fnreader(pos->row + 1, pos->col) != EMPTY)
			pos->row += 1;
		return;
	}
	if (move == LEFT) {
		if (pos->col != 0 && def.fnreader(pos->row, pos->col - 1) != EMPTY)
			pos->col -= 1;
		return;
	}
	assert(NULL);
}


/* the rules of the instructions:
 *
 * - start at the previous button (5 for the very first)
 *
 * - make the moves URDL from your current position, if move would
 *   take you off the keypad, ignore it
 *
 * - when you get the end of a line, that's the digit for that line,
 *   and it's the starting digit for the next line
 *
 * so for the sample isntructions:
 *
 * ULL should move from 5 to 2 to 1 to ignore so you stay at 1
 * RRDDD move from 1 to 2 to 3 then 6 then 9 then ignore so you stay at 9
 * LURDL should move from 9 to 8 to 5 to 6 to 9 to 8
 * UUUUD should move from 8 to 5 to 2 to ignore to ignore to 5
 *
 * answer is 1985
 *
 */

/*
 * part one:
 *
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

	keypad_pos pos = { 1, 1 };

	char *result = malloc(RESULT_MAX + 1);
	memset(result, 0, RESULT_MAX + 1);
	int p = 0;

	char buffer[INPUT_LINE_MAX];
	memset(buffer, 0, INPUT_LINE_MAX);

	while (fgets(buffer, INPUT_LINE_MAX - 1, ifile)) {
		char *move = buffer;
		while (*move) {
			if (*move == '\n') {
				move += 1;
				continue;
			}
			move_key(&pos, keypad_one_def, *move);
			move += 1;
		}
		result[p] = keypad_one_def.fnreader(pos.row, pos.col);
		p += 1;
		assert(p < RESULT_MAX);
	}

	printf("part one: %s\n", result);

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
		fprintf(stderr, "error: could not open file: %s\n", fname);
		return EXIT_FAILURE;
	}

	keypad_pos pos = { 2, 0 };

	char *result = malloc(RESULT_MAX + 1);
	memset(result, 0, RESULT_MAX + 1);
	int p = 0;

	char buffer[INPUT_LINE_MAX];
	memset(buffer, 0, INPUT_LINE_MAX);

	while (fgets(buffer, INPUT_LINE_MAX - 1, ifile)) {
		char *move = buffer;
		while (*move) {
			if (*move == '\n') {
				move += 1;
				continue;
			}
			move_key(&pos, keypad_two_def, *move);
			move += 1;
		}
		result[p] = keypad_two_def.fnreader(pos.row, pos.col);
		p += 1;
		assert(p < RESULT_MAX);
	}

	printf("part two: %s\n", result);

	fclose(ifile);
	return EXIT_SUCCESS;
}
