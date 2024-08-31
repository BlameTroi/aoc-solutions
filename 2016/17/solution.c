/* solution.c -- aoc 2016 17 -- troy brumley */

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TXBMD5_IMPLEMENTATION
#include "txbmd5.h"

#define TXBSB_IMPLEMENTATION
#include "txbsb.h"

#define TXBRS_IMPLEMENTATION
#include "txbrs.h"

#define TXBQU_IMPLEMENTATION
#include "txbqu.h"

#define TXBMISC_IMPLEMENTATION
#include "txbmisc.h"

#include "solution.h"

/*
 * a four by four 'maze' must be traversed, but as always there's a
 * catch. doors open and close in the maze based on the md5 hash of
 * your passcode (provided) and your moves (as a string of characters
 * UDLR).
 *
 * for part one, what is the shortest path (as moves) from the start
 * to the end. for part two, what is the longest path length (integer)
 * to reach the end.
 *
 * as i frequently do, i over engineered this during part one, but
 * that pays off in part two. and i had fun along the way, which is
 * the real goal anyway.
 */

/*
 * our problem state and its api.
 *
 * since the maze 'map' changes as you move through it, a history of
 * moves is needed instead of a real map. your current available moves
 * depends upon a passcode and move history.
 *
 * integers for most things, booleans where appropriate, and the move
 * history is stored in a string builder.
 *
 * since the maze literally changes underfoot, the ability to do a
 * deep copy of the maze state is provided. one could just extract the
 * move history and replay it, but that just trades the mallocs and
 * copies for more complexity elsehwere and having to recalculate
 * md5 hashes with each replayed move.
 *
 * that's a bad trade.
 */

maze_state *
create_maze(
        const char *passcode,
        int rows, int cols,
        int start_row, int start_col,
        int end_row, int end_col
) {
	maze_state *m = malloc(sizeof(*m));
	m->passcode = passcode;
	m->rows = rows;
	m->cols = cols;
	m->row = start_row;
	m->col = start_col;
	m->start_row = start_row;
	m->start_col = start_col;
	m->end_row = end_row;
	m->end_col = end_col;
	m->moves = sb_create_blksize(rows *cols * 4);
	m->count_moves = 0;
	m->last_move = '\0';
	m->are_current = false;
	return m;
}

void
destroy_maze(
        maze_state *m
) {
	if (m->moves)
		sb_destroy(m->moves);
	free(m);
}

/*
 * copy the maze. the move history is cloned so this is a deep copy.
 */

maze_state *
copy_maze(
        const maze_state *m
) {
	maze_state *copy = malloc(sizeof(*copy));
	memcpy(copy, m, sizeof(*copy));
	copy->moves = sb_create_blksize(m->rows *m->cols * 4);
	char *str = sb_to_string(m->moves);
	sb_puts(copy->moves, str);
	copy->are_current = false;
	free(str);
	return copy;
}

/*
 * general accessors.
 */

void
get_position(
        maze_state *m,
        int *row,
        int *col
) {
	*row = m->row;
	*col = m->col;
}

const char *
get_move_history(
        const maze_state *m
) {
	return sb_to_string(m->moves);
}

bool
at_end(
        const maze_state *m
) {
	return m->row == m->end_row && m->col == m->end_col;
}

/*
 * movement in the four directions up down left and right:
 *
 * the doors in the maze open and close based on your moves to date
 * combined with your passcode. concatenate the passcode and your move
 * history, take the md5 hash, and the first four digits of the
 * indicate whether a door is open or closed and locked. the first
 * four digits (two bytes) correspond to the directions up down left
 * and right. a value of 0->a means closed, and b->f means opened.
 *
 * so i guess you never pass the same way twice :)
 *
 * while the performance for this problem isn't critical, i make an
 * attempt to minimize taking the md5. this is more about practicing
 * techniques. predicates and actions come groups of four or five
 * functions, one each for the direction, and a cap that accepts a
 * direction as an argument.
 */

void
update_directions(maze_state *m) {
	uint8_t digest[16];
	char *source = malloc(strlen(m->passcode) + sb_length(m->moves) + 1);
	source[0] = '\0';
	strcpy(source, m->passcode);
	char *moves = sb_to_string(m->moves);
	strcat(source, moves);
	md5_string(source, digest);
	m->up = (digest[0] >> 4) > 0x0a;
	m->down = (digest[0] & 0x0f) > 0x0a;
	m->left = (digest[1] >> 4) > 0x0a;
	m->right = (digest[1] & 0x0f) > 0x0a;
	m->are_current = true;
	free(source);
	free(moves);
}

bool
is_up_open(maze_state *m) {
	if (m->are_current)
		return m->up;
	update_directions(m);
	return m->up;
}

bool
is_down_open(maze_state *m) {
	if (m->are_current)
		return m->down;
	update_directions(m);
	return m->down;
}

bool
is_left_open(maze_state *m) {
	if (m->are_current)
		return m->left;
	update_directions(m);
	return m->left;
}

bool
is_right_open(maze_state *m) {
	if (m->are_current)
		return m->right;
	update_directions(m);
	return m->right;
}

bool
can_move_up(maze_state *m) {
	return is_up_open(m) && m->row > 0;
}

bool
can_move_down(maze_state *m) {
	return is_down_open(m) && m->row < m->rows - 1;
}

bool
can_move_left(maze_state *m) {
	return is_left_open(m) && m->col > 0;
}

bool
can_move_right(maze_state *m) {
	return is_right_open(m) && m->col < m->cols - 1;
}

bool
can_move(maze_state *m, char direction) {
	switch (direction) {
	case UP:
		return can_move_up(m);
	case DOWN:
		return can_move_down(m);
	case LEFT:
		return can_move_left(m);
	case RIGHT:
		return can_move_right(m);
	default:
		assert(NULL);
	}
}

int
number_of_possible_moves(maze_state *m) {
	int res = 0;
	if (can_move_up(m))
		res += 1;
	if (can_move_down(m))
		res += 1;
	if (can_move_left(m))
		res += 1;
	if (can_move_right(m))
		res += 1;
	return res;
}

bool
do_move(
        maze_state *m,
        char direction
) {
	bool ok = false;
	int row_delta = 0;
	int col_delta = 0;
	switch (direction) {
	case UP:
		ok = can_move_up(m);
		row_delta = -1;
		break;
	case DOWN:
		ok = can_move_down(m);
		row_delta = 1;
		break;
	case LEFT:
		ok = can_move_left(m);
		col_delta = -1;
		break;
	case RIGHT:
		ok = can_move_right(m);
		col_delta = 1;
		break;
	default:
		assert(NULL);
	}
	if (ok) {
		sb_putc(m->moves, direction);
		m->last_move = direction;
		m->count_moves += 1;
		m->row += row_delta;
		m->col += col_delta;
		m->are_current = false;
	}
	return ok;
}

bool
do_move_up(maze_state *m) {
	return do_move(m, UP);
}

bool
do_move_down(maze_state *m) {
	return do_move(m, DOWN);
}

bool
do_move_left(maze_state *m) {
	return do_move(m, LEFT);
}

bool
do_move_right(maze_state *m) {
	return do_move(m, RIGHT);
}

/*
 * search for the shortest path through the maze. the difficulty lies
 * in the maze changing with each move, but we are guaranteed that the
 * passcode we were given has a solution.
 *
 * returns an updated copy of the maze. if at_end() is true, the
 * shortest path is available with get_move_history(). the original
 * maze is not altered.
 */

maze_state *
find_shortest_path(
        const maze_state *original
) {
	/* the final result will be in res */
	maze_state *res = NULL;
	maze_state *m = copy_maze(original);

	int queue_depth = -1;
	int shortest_length = INT_MAX;
	qucb *results = qu_create();
	qucb *work = qu_create();
	qu_enqueue(work, m);
	m = NULL;
	while (!qu_empty(work)) {
		queue_depth = max(queue_depth, qu_count(work));
		m = qu_dequeue(work);
		/* solution? */
		if (at_end(m)) {
			shortest_length = min(m->count_moves, shortest_length);
			qu_enqueue(results, m);
			m = NULL;
			continue;
		}
		/* dead end? */
		if (number_of_possible_moves(m) == 0) {
			destroy_maze(m);
			m = NULL;
			continue;
		}
		char checks[] = { UP, DOWN, LEFT, RIGHT, '\0' };
		for (int i = 0; checks[i] != '\0'; i++) {
			if (can_move(m, checks[i])) {
				maze_state *c = copy_maze(m);
				do_move(c, checks[i]);
				qu_enqueue(work, c);
				c = NULL;
			}
		}
		destroy_maze(m);
		m = NULL;
	}

	/* all possible paths have been checked. */
	qu_destroy(work);
	work = NULL;

	/* it is possible to have a failure, no solution found. if so, just
	 * return a copy of the original. */

	if (qu_empty(results)) {
		qu_destroy(results);
		results = NULL;
		return copy_maze(original);
	}

	/* it is also possible that multiple results were found, return the
	 * first of the shortest found. if there is only one result, just
	 * return it directly. */

	if (qu_count(results) == 1) {
		res = qu_dequeue(results);
		qu_destroy(results);
		results = NULL;
		return res;
	}

	/* flush out the results queue and report the first of the results
	 * that have the shortest lengh, but check for and report number of
	 * duplicates. */

	int duplicates = 0;
	res = NULL;
	while (!qu_empty(results)) {
		m = qu_dequeue(results);
		if (m->count_moves == shortest_length) {
			if (res == NULL)
				res = m;

			else {
				duplicates += 1;
				destroy_maze(m);
			}
		} else
			destroy_maze(m);
		m = NULL;
	}
	qu_destroy(results);
	results = NULL;
	if (duplicates)
		printf("an additional %d results also had the shortest length\n", duplicates);
	return res;
}

/*
 * search for the longest path through the maze. this is a minor
 * surgery on find_shortest_path(), see that function for more
 * commentary.
 */

maze_state *
find_longest_path(
        const maze_state *original
) {
	/* the final result will be in res */
	maze_state *res = NULL;
	maze_state *m = copy_maze(original);

	int queue_depth = -1;
	int longest_length = -1;
	qucb *results = qu_create();
	qucb *work = qu_create();
	qu_enqueue(work, m);
	m = NULL;
	while (!qu_empty(work)) {
		queue_depth = max(queue_depth, qu_count(work));
		m = qu_dequeue(work);
		/* solution? */
		if (at_end(m)) {
			longest_length = max(m->count_moves, longest_length);
			qu_enqueue(results, m);
			m = NULL;
			continue;
		}
		/* dead end? */
		if (number_of_possible_moves(m) == 0) {
			destroy_maze(m);
			m = NULL;
			continue;
		}
		char checks[] = { UP, DOWN, LEFT, RIGHT, '\0' };
		for (int i = 0; checks[i] != '\0'; i++) {
			if (can_move(m, checks[i])) {
				maze_state *c = copy_maze(m);
				do_move(c, checks[i]);
				qu_enqueue(work, c);
				c = NULL;
			}
		}
		destroy_maze(m);
		m = NULL;
	}

	/* all possible paths have been checked. */
	qu_destroy(work);
	work = NULL;

	/* it is possible to have a failure, no solution found. if so, just
	 * return a copy of the original. */

	if (qu_empty(results)) {
		qu_destroy(results);
		results = NULL;
		return copy_maze(original);
	}

	/* it is also possible that multiple results were found, return the
	 * first of the shortest found. if there is only one result, just
	 * return it directly. */

	if (qu_count(results) == 1) {
		res = qu_dequeue(results);
		qu_destroy(results);
		results = NULL;
		return res;
	}

	/* flush out the results queue and report the first of the results
	 * that have the longest lengh, but check for and report number of
	 * duplicates. */

	int duplicates = 0;
	res = NULL;
	while (!qu_empty(results)) {
		m = qu_dequeue(results);
		if (m->count_moves == longest_length) {
			if (res == NULL)
				res = m;

			else {
				duplicates += 1;
				destroy_maze(m);
			}
		} else
			destroy_maze(m);
		m = NULL;
	}
	qu_destroy(results);
	results = NULL;
	if (duplicates)
		printf("an additional %d results also had the longest length\n", duplicates);
	return res;
}

/*
 * part one:
 *
 */

int
part_one(
        const char *fname
) {

	maze_state *m = create_maze(LIVE_PASSCODE, 4, 4, 0, 0, 3, 3);
	maze_state *res = find_shortest_path(m);

	printf("part one: %d '%s'\n", res->count_moves, get_move_history(res));

	destroy_maze(m);
	destroy_maze(res);

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
	maze_state *m = create_maze(LIVE_PASSCODE, 4, 4, 0, 0, 3, 3);
	maze_state *res = find_longest_path(m);

	printf("part two: %d '%s'\n", res->count_moves, get_move_history(res));

	destroy_maze(m);
	destroy_maze(res);

	return EXIT_SUCCESS;
}
