/* solution.c -- aoc 2015 03 -- troy brumley */


#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define TXBMISC_IMPLEMENTATION
#include "txbmisc.h"


/*
 * santa gets delivery instructions as moves on an infinite
 * rectangular grid. ^>v< for north, east, south, and west. moves are
 * always exactly one house. after each move, he delivers another
 * present to the house at his new location. for the first move, he is
 * assumed to have dropped off a package at his starting location.
 *
 * examples:
 *
 * > delivers presents to 2 houses: one at the starting location,
 *   and one to the east.
 *
 * ^>v< delivers presents to 4 houses in a square, including twice
 *      to the house at his starting/ending location.
 *
 * ^v^v^v^v^v delivers a bunch of presents to some very lucky
 *            children at only 2 houses.
 *
 * for part two, the instructions alternate between santa and a robot
 * helper. flip back and forth between them for a different answer.
 *
 * to get the part one answer, comment out the second inc(0,0) before
 * the main loop, and the flip of who between santa and robot at the
 * end of the loop.
 */


/*
 * direction glyphs:
 */

#define N ('^')
#define E ('>')
#define S ('v')
#define W ('<')
#define NESW ("^>v<")


/*
 * we know the input is 8k in length, so at worst we could
 * only move 8192 positions along a straight line. swag a
 * dimension:
 */

#define X_ADJ (1000)
#define Y_ADJ (1000)

#define X_MIN (-X_ADJ)
#define X_MAX (X_ADJ)
#define Y_MIN (-Y_ADJ)
#define Y_MAX (Y_ADJ)


/*
 * for part 2, santa has a robot helper so roll the position into a
 *  helper structure.
 */

typedef struct coord coord;

struct coord {
	int x;                     /* current coordinates */
	int y;
};


/*
 * grid is a block of storage to hold gift counters.
 */

typedef struct grid_t {
	size_t at_least_once;     /* how many cells visited */
	int minx;                 /* range of cells visited */
	int maxx;
	int miny;
	int maxy;
	uint16_t cell[2*X_ADJ+1][2*Y_ADJ+1];
} grid_t;

grid_t *grid;


/*
 * create the grid, initialized appropriately.
 */

void
make_grid(void) {
	grid = calloc(sizeof(grid_t), 1);
	assert(grid);
	grid->at_least_once = 0;
	grid->minx = X_MAX+1;
	grid->maxx = -X_MAX-1;
	grid->miny = Y_MAX+1;
	grid->maxy = -Y_MAX-1;
}


/*
 * release the grid.
 */

void
free_grid(void) {
	free(grid);
	grid = NULL;
}


/*
 * fake out negative array indices.
 */

void
adjust_coords(
        int *x,
        int *y
) {
	assert(X_MIN < *x && *x < X_MAX);
	assert(Y_MIN < *y && *y < Y_MAX);
	*x = *x + X_ADJ;
	*y = *y + Y_ADJ;
}


/*
 * get the value from a grid cell.
 */

uint16_t
get_grid(
        int x,
        int y
) {
	adjust_coords(&x, &y);
	return grid->cell[x][y];
}


/*
 * increment a visited grid cell and count each cell when it is
 * visited for the first time.
 */

void
inc_grid(
        int x,
        int y
) {
	/* min and max are pre-adjustment */
	grid->minx = min(grid->minx, x);
	grid->maxx = max(grid->maxx, x);
	grid->miny = min(grid->miny, y);
	grid->maxy = max(grid->maxy, y);
	/* map coordinates to 0 based */
	adjust_coords(&x, &y);
	/* count first visits */
	if (grid->cell[x][y] == 0)
		grid->at_least_once += 1;
	/* count gifts left */
	grid->cell[x][y] += 1;
}


/*
 * let's do this:
 */

int
main(
        int argc,
        const char **argv
) {
	FILE *ifile;
	coord santa = {0, 0};
	coord robot = {0, 0};
	size_t moves = 0;     /* how many? */
	int ch;               /* an input command */

	make_grid();

	if (argc < 2) {
		fprintf(stderr, "usage: %s path-to-input\n", argv[0]);
		return EXIT_FAILURE;
	}

	ifile = fopen(argv[1], "r");
	if (!ifile) {
		fprintf(stderr, "could not open file: %s\n", argv[1]);
		return EXIT_FAILURE;
	}

	/*
	 * prime the pump. we start at 0,0 and have left a gift there.
	 */

	inc_grid(santa.x, santa.y);
	inc_grid(robot.x, robot.y);
	coord *who = &santa;
	while (EOF != (ch = fgetc(ifile))) {
		switch (ch) {
		case N:
			who->y += 1;
			break;
		case E:
			who->x += 1;
			break;
		case S:
			who->y -= 1;
			break;
		case W:
			who->x -= 1;
			break;
		default:
			fprintf(stderr, "illegal input %c %x, ignored!\n", ch, ch);
			continue;
		}
		moves += 1;
		inc_grid(who->x, who->y);
		/*
		 * comment out the following update of who for part
		 * one result.
		 */
		/* who = who == &santa ? &robot : &santa; */
	}

	fprintf(stdout, "moves: %zu\n", moves);
	fprintf(stdout, "x range: %d, %d\n", grid->minx, grid->maxx);
	fprintf(stdout, "y range: %d, %d\n", grid->miny, grid->maxy);
	fprintf(stdout, "cells visited at least once: %zu\n", grid->at_least_once);

	fclose(ifile);
	free_grid();

	return EXIT_SUCCESS;
}
