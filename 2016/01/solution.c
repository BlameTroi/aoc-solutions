/* solution.c -- aoc 2016 xx -- troy brumley */

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"


/*
 * a command is (L|R)digits ... which gets parsed into a
 * move structure.
 */

#define INVALID '?'
#define LEFT    'L'
#define RIGHT   'R'
#define END     'E'

typedef struct move move;

struct move {
	char turn;       /* LEFT, RIGHT, INVALID, END */
	int distance;    /* number of blocks */
};

/*
 * directions faced/moved on the grid.
 */

#define NORTH   'N'
#define EAST    'E'
#define SOUTH   'S'
#define WEST    'W'

/*
 * coordinates on the grid.
 */

typedef struct coord coord;

struct coord {
	int x;
	int y;
};

/* not quite the null object pattern, but it will do in a pinch. */

static coord null_coord = { INT_MAX, INT_MAX };

bool
is_null_coord(coord p) {
	return p.x == INT_MAX && p.y == INT_MAX;
}

/*
 * distance on a grid.
 */

int
rectilinear_distance(
        coord p,
        coord q
) {
	return labs(p.x - q.x) + labs(p.y - q.y);
}

int
rectilinear_distance_from_origin(
        coord p
) {
	return labs(p.x) + labs(p.y);
}

/*
 * for part two we need to track each location that is
 * visited. a -500 to 500, -500 to 500 grid will be
 * more than large enough for the problem data.
 */

#define GRID_OFS (500)
#define GRID_DIM (2*GRID_OFS)
static bool grid[GRID_DIM][GRID_DIM];

void
clear_grid(void) {
	memset(grid, 0, sizeof(grid));
}

/*
 * since we don't have fortran or pascal subscripts, map
 * onto our c grid.
 */

coord
map_to_grid(coord p) {
	return (coord) {
		p.x + GRID_OFS, p.y + GRID_OFS
	};
}

/*
 * note that a point has been visited. returns true if this
 * is not the first visit of the point.
 */

bool
visit(coord p) {
	coord m = map_to_grid(p);
	if (grid[m.x][m.y])
		return true;
	grid[m.x][m.y] = true;
	return false;
}

/*
 * work through the input command string and return a parsed move
 * structure. the format of a command is turn direction L(eft) or
 * R(ight) followed immediately by an integer. commands are separated
 * by ", ".
 *
 * on end of file move.turn will be 'E', on error move.turn will be
 * '?'.
 */

move
get_next_move(
        const char *str,   /* input string L9, R17, L3, etc */
        int *pos           /* current position in string */
) {

	move g = {INVALID, 0};

	if (!str || *pos < 0 || *pos > strlen(str))
		return g;

	/* find L or R */
	while (str[*pos] &&
	                str[*pos] != LEFT &&
	                str[*pos] != RIGHT)
		*pos += 1;

	/* end of string? */
	if (!str[*pos]) {
		g.turn = END;
		*pos = -1;
		return g;
	}

	/* get the distance to move */
	g.turn = str[*pos];
	*pos += 1;
	char *endtol = NULL;
	g.distance = strtol(str + *pos, &endtol, 10);

	/* update position in string */
	*pos = endtol - str;
	return g;
}

/*
 * given the current map direction and a 90 degree turn from a move (left
 * or right), return the new direction.
 */

char
direction_after_turn(
        char facing,
        char turn
) {
	char n = '?';
	if (facing == NORTH)
		n = turn == LEFT ? WEST : EAST;

	else if (facing == EAST)
		n = turn == LEFT ? NORTH : SOUTH;

	else if (facing == SOUTH)
		n = turn == LEFT ? EAST : WEST;

	else if (facing == WEST)
		n = turn == LEFT ? SOUTH : NORTH;

	else
		assert(NULL);
	return n;
}

/*
 * actually traverse from-to step by step and if we step somewhere
 * we've been before, return that location, otherwise the null_coord.
 * the walk stops early if a visited point is reached again.
 */

coord
walk_from_to(coord from, coord to) {
	coord res = null_coord;
	bool seen = false;

	char dir = '?';
	if (from.x < to.x)
		dir = EAST;

	else if (from.x > to.x)
		dir = WEST;

	else if (from.y < to.y)
		dir = NORTH;

	else if (from.y > to.y)
		dir = SOUTH;

	else
		assert(NULL);

	while (from.x != to.x || from.y != to.y) {
		if (dir == NORTH)
			from.y += 1;

		else if (dir == EAST)
			from.x += 1;

		else if (dir == SOUTH)
			from.y -= 1;

		else if (dir == WEST)
			from.x -= 1;

		if (visit(from) && !seen) {
			res = from;
			seen = true;
		}
	}

	return res;
}

/*
 * make a move. the caller provides the currently faced direction
 * and location along with the move. location and direction are
 * updated.
 */

void
do_move(char *dir, coord *at, move m) {
	assert(m.turn == LEFT || m.turn == RIGHT);
	assert(*dir == NORTH || *dir == EAST || *dir == SOUTH || *dir == WEST);
	char new_dir = direction_after_turn(*dir, m.turn);
	if (new_dir == NORTH)
		at->y += m.distance;

	else if (new_dir == EAST)
		at->x += m.distance;

	else if (new_dir == SOUTH)
		at->y -= m.distance;

	else if (new_dir == WEST)
		at->x -= m.distance;

	else
		assert(NULL);
	*dir = new_dir;
}

/*
 * for part one, just move from point to point based on the
 * directions, assuming a starting point of origin and facing
 * north. follow the moves and report the final location.
 */

coord
walk_moves(const char *str) {
	int pos = 0;
	char dir = NORTH;
	coord at = { 0, 0 };
	move m;
	while (true) {
		m = get_next_move(str, &pos);
		if (m.turn == END)
			break;
		do_move(&dir, &at, m);
	}
	return at;
}

/*
 * for part two, follow the same instructions as in part one,
 * but check each step along the path to see if that point
 * has been visited before. stop and report the coordinates
 * of the first point reached twice in the moves.
 */

coord
walk_until_repeat(const char *str) {
	int pos = 0;
	char dir = NORTH;
	coord last = { 0, 0 };
	coord next = { 0, 0 };
	coord res = null_coord;
	move m;
	while (is_null_coord(res)) {
		m = get_next_move(str, &pos);
		if (m.turn == END)
			break;
		do_move(&dir, &next, m);
		res = walk_from_to(last, next);
		last = next;
	}
	return res;
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
		fprintf(stderr, "error: could not open file: %s\n", fname);
		return EXIT_FAILURE;
	}

	char iline[INPUT_LINE_MAX];

	if (!fgets(iline, INPUT_LINE_MAX - 1, ifile))
		fprintf(stderr, "error: could not read file: %s\n", fname);

	coord fini = walk_moves(iline);
	long distance = rectilinear_distance_from_origin(fini);

	printf("part one: %ld\n", distance);

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
	FILE *ifile;

	ifile = fopen(fname, "r");
	if (!ifile) {
		fprintf(stderr, "error: could not open file: %s\n", fname);
		return EXIT_FAILURE;
	}
	char iline[INPUT_LINE_MAX];

	if (!fgets(iline, INPUT_LINE_MAX - 1, ifile))
		fprintf(stderr, "error: could not read file: %s\n", fname);

	coord fini = walk_until_repeat(iline);
	int distance = rectilinear_distance_from_origin(fini);

	printf("part two: %d\n", distance);

	fclose(ifile);
	return EXIT_SUCCESS;
}
