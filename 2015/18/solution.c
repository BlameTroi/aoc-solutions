/* solution.c -- aoc 2015 18 -- troy brumley */

/*
 * play a grid of lights following rules similar to the game of life.
 *
 * a light which is on stays on when 2 or 3 neighbors are on, and
 * turns off otherwise.
 *
 * a light which is off turns on if exactly 3 neighbors are on, and
 * stays off otherwise.
 *
 * these decisions are made simultaneously and the state does not
 * change until all states have been determined.
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"


/*
 * reset to initial state
 */

void
reset_state(int rel)
{
	if (rel) {
		/* not used this day */
	}
	dim_rows = 0;
	dim_cols = 0;
	iter = 0;
	stuck_rules = 0;
	for (int i = 0; i < DIMENSION_MAX; i++) {
		memset(page_one.p[i], G_OFF, sizeof(page_one.p[i]));
		memset(page_two.p[i], G_OFF, sizeof(page_one.p[i]));
	}
	reading = &page_one;
	writing = &page_two;
}


/*
 * flip foreground and background display pages.
 */

void
flip_pages(void)
{
	page_t *swap = reading;
	reading = writing;
	writing = swap;
}


/*
 * for part two, some lightss are stuck on. conveniently, these are the corner
 * lights.
 */

bool
stuck_p(
        int row,
        int col
)
{
	if (stuck_rules) {
		if (row == 0 && (col == 0 || col == dim_cols-1))
			return true;
		if (row == dim_rows-1 && (col == 0 || col == dim_cols-1))
			return true;
	}
	return false;
}


/*
 * is a light on or off? clips coordinates to the valid range, anything
 * outside is off. this keeps hides the whole edge issue from higher
 * level code.
 */

bool
on_p(
        int row,
        int col
)
{
	if (row < 0 || col < 0 || row > dim_rows - 1 || col > dim_cols - 1)
		return false;
	if (stuck_p(row, col))
		return true;
	else
		return reading->p[row][col] == G_ON;
}


/*
 * turn the light on in the writing page.
 */

void
turn_on(
        int row,
        int col
)
{
	writing->p[row][col] = G_ON;
}


/*
 * turn the light off in the writing page. honors the stuck_rule.
 */

void
turn_off(
        int row,
        int col
)
{
	if (stuck_p(row, col))
		return;
	writing->p[row][col] = G_OFF;
}


/*
 * return the number of neighboring lights that are on.
 */

int
neighbors_on(
        int row,
        int col
)
{
	int r = 0;
	r += on_p(row-1, col-1) + on_p(row, col-1) + on_p(row+1, col-1);
	r += on_p(row-1, col) + /*     this cell          */ on_p(row+1, col);
	r += on_p(row-1, col+1) + on_p(row, col+1) + on_p(row+1, col+1);
	return r;
}


/*
 * should this light be on or off? this does not update the light on
 * the write page.
 *
 * a light which is on stays on when 2 or 3 neighbors are on, and
 * turns off otherwise.
 *
 * a light which is off turns on if exactly 3 neighbors are on, and
 * stays off otherwise.
 */

bool
decide(
        int row,
        int col
)
{
	if (stuck_p(row, col))
		return true;
	int hood = neighbors_on(row, col);
	if (on_p(row, col))
		return hood == 2 || hood == 3;
	else
		return hood == 3;
}


/*
 * make a pass through the lights and decide what's up. returns number
 * of lights that are on in the write page at the end of the cycle.
 */

int
cycle_lights(void)
{
	int lit = 0;
	for (int r = 0; r < dim_rows; r++) {
		for (int c = 0; c < dim_cols; c++) {
			if (decide(r, c)) {
				lit += 1;
				turn_on(r, c);
			} else
				turn_off(r, c);
		}
	}
	return lit;
}


/*
 * load a page row from input. each row is expected to be dim_cols
 * glyph characters followed by a newline.
 */

void
load_row(
        int row,
        const char *s
)
{
	assert(strlen(s) == dim_cols + 1);
	for (int c = 0; c < dim_cols; c++) {
		switch (s[c]) {
		case G_ON:
			turn_on(row, c);
			break;
		case G_OFF:
			turn_off(row, c);
			break;
		case '\n':
			break;
		default:
			assert(NULL);
		}
	}
}


/*
 * part one:
 *
 * run the input for 100 iterations, how many lights are on?
 *
 */

int
part_one(
        const char *fname
)
{

	FILE *ifile = fopen(fname, "r");
	if (!ifile) {
		printf("could not open file: %s\n", fname);
		return EXIT_FAILURE;
	}

	char iline[INPUT_LEN_MAX];

	reset_state(0);
	int first = 1;
	int row = 0;
	while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
		if (first) {
			/* the first input line contains number of rows, columns, and
			   iterations to run */
			char *next = iline;
			dim_rows = strtol(iline, &next, 10);
			dim_cols = strtol(next, &next, 10);
			iter = strtol(next, &next, 10);
			first = 0;
			continue;
		}
		load_row(row, iline);
		row += 1;
	}

	int lit;
	for (int i = 0; i < iter; i++) {
		flip_pages();
		lit = cycle_lights();
	}

	printf("part one: %d\n", lit);

	reset_state(1);
	fclose(ifile);
	return EXIT_SUCCESS;
}


/*
 * part two:
 *
 * run the input for 100 iterations, but this time the lights
 * in the four corners are stuck always on.
 *
 */

int
part_two(
        const char *fname
)
{

	FILE *ifile = fopen(fname, "r");
	if (!ifile) {
		printf("could not open file: %s\n", fname);
		return EXIT_FAILURE;
	}

	char iline[INPUT_LEN_MAX];

	reset_state(0);
	int first = 1;
	int row = 0;
	while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
		if (first) {
			/* the first input line contains number of rows, columns, and
			   iterations to run */
			char *next = iline;
			dim_rows = strtol(iline, &next, 10);
			dim_cols = strtol(next, &next, 10);
			iter = strtol(next, &next, 10);
			first = 0;
			continue;
		}
		load_row(row, iline);
		row += 1;
	}

	stuck_rules = 1;

	int lit;
	for (int i = 0; i < iter; i++) {
		flip_pages();
		lit = cycle_lights();
	}

	printf("part two: %d\n", lit);

	reset_state(1);
	fclose(ifile);
	return EXIT_SUCCESS;
}
