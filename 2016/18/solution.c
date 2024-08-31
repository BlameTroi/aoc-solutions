/* solution.c -- aoc 2016 xx -- troy brumley */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

/*
 * work from the top (row 1 and down) and determine if the grid cell
 * is a trap or safe by looking at the row above and applying the
 * following rules:
 *
 * a tile is a trap only if:
 *
 * its left and center tiles are traps, but its right tile is not.
 * its center and right tiles are traps, but its left tile is not.
 * only its left tile is a trap.
 * only its right tile is a trap.
 *
 * in glyphs:
 *
 *  ^^.  --  trap
 *  .^^  --  trap
 *  ^..  --  trap
 *  ..^  --  trap
 *
 * positions that are off the grid are considered safe and not traps.
 */

#define ROW_WRAP 100
int map_rows = 0;
int map_cols = 0;
int map_safes = 0;
int map_traps = 0;
char floor_map[ROW_WRAP][100];
bool map_filled = false;

int
wrapped_row(int row) {
	return row % ROW_WRAP;
}

void
initialize_floor_map(int rows, int cols, const char *data) {
	assert(rows > 0 && cols > 0 && cols <= 100);
	memset(floor_map, 0, sizeof(floor_map));
	map_rows = rows;
	map_cols = cols;
	map_traps = 0;
	map_safes = 0;
	map_filled = false;
	int col = 0;
	while (*data == SAFE || *data == TRAP) {
		floor_map[0][col] = *data;
		if (*data == SAFE)
			map_safes += 1;

		else
			map_traps += 1;
		col += 1;
		data += 1;
		cols -= 1;
	}
	assert(!cols);
}

char trap_signatures[][3] = {
	{ TRAP, TRAP, SAFE },
	{ SAFE, TRAP, TRAP },
	{ TRAP, SAFE, SAFE },
	{ SAFE, SAFE, TRAP },
	{ '\0', '\0', '\0' }
};

char
upper_left(int row, int col) {
	if (col > 0)
		return floor_map[wrapped_row(row-1)][col-1];
	return SAFE;
}

char
upper_right(int row, int col) {
	if (col < map_cols - 1)
		return floor_map[wrapped_row(row-1)][col+1];
	return SAFE;
}

char
upper_middle(int row, int col) {
	return floor_map[wrapped_row(row-1)][col];
}

bool
is_trap(int row, int col) {
	char ul = upper_left(row, col);
	char um = upper_middle(row, col);
	char ur = upper_right(row, col);
	for (int i = 0; trap_signatures[i][0] != '\0'; i++) {
		if (ul == trap_signatures[i][0] &&
		                um == trap_signatures[i][1] &&
		                ur == trap_signatures[i][2])
			return true;
	}
	return false;
}

void
fill_floor_map(void) {
	assert(!map_filled);
	for (int row = 1; row < map_rows; row++) {
		for (int col = 0; col < map_cols; col++) {
			if (is_trap(row, col)) {
				floor_map[wrapped_row(row)][col] = TRAP;
				map_traps += 1;
			} else {
				floor_map[wrapped_row(row)][col] = SAFE;
				map_safes += 1;
			}
		}
	}
	map_filled = true;
}

void
count_map(int *trap, int *safe) {
	if (!map_filled)
		fill_floor_map();
	*trap = map_traps;
	*safe = map_safes;
}

void
print_map(void) {
	if (map_rows > ROW_WRAP) {
		printf("\ncan not print map of this size\n");
		return;
	}
	if (!map_filled)
		fill_floor_map();
	printf("\n");
	for (int row = 0; row < map_rows; row++) {
		for (int col = 0; col < map_cols; col++)
			printf("%c", floor_map[row][col]);
		printf("\n");
	}
}

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

	char iline[INPUT_LINE_MAX];

	fgets(iline, INPUT_LINE_MAX - 1, ifile);
	initialize_floor_map(40, 100, iline);
	fill_floor_map();
	int traps = 0;
	int safes = 0;
	count_map(&traps, &safes);

	printf("part one: %d\n", safes);

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
	char iline[INPUT_LINE_MAX];

	fgets(iline, INPUT_LINE_MAX - 1, ifile);
	initialize_floor_map(400000, 100, iline);
	fill_floor_map();
	int traps = 0;
	int safes = 0;
	count_map(&traps, &safes);

	printf("part two: %d\n", safes);

	fclose(ifile);
	return EXIT_SUCCESS;
}
