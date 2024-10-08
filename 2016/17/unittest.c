/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"
#include "solution.h"

#include "txbrs.h"

int num_args = 0;
char **the_args = NULL;

void
test_setup(void) {
}

void
test_teardown(void) {
}

/*
 * the walkthrough from the problem statement. it dead ends to
 * demonstrate some of the difficulties of the problem.
 */

MU_TEST(test_test) {
	maze_state *m = create_maze(
	                        BAD_PASSCODE,
	                        4, 4,
	                        0, 0,
	                        3, 3
	                );

	/* starting out 'doors' are open up, down, and left */
	mu_should(is_up_open(m));
	mu_should(is_down_open(m));
	mu_should(is_left_open(m));
	mu_shouldnt(is_right_open(m));

	/* being in the upper left corner, up and down are useless */
	mu_shouldnt(can_move_up(m));
	mu_should(can_move_down(m));
	mu_shouldnt(can_move_left(m));
	mu_shouldnt(can_move_right(m));

	/* so we have only one possible move */
	mu_should(number_of_possible_moves(m) == 1);

	/* take it and check the new state */
	mu_should(do_move_down(m));

	/* from here we have two possible moves, back up or to the right */
	mu_should(number_of_possible_moves(m) == 2);
	mu_should(can_move_up(m));
	mu_should(can_move_right(m));

	/* so try each */
	maze_state *copy_up = copy_maze(m);
	maze_state *copy_right = copy_maze(m);

	/* right should be a dead end */
	mu_should(do_move_right(copy_right));
	mu_should(number_of_possible_moves(copy_right) == 0);

	/* up should work a little better */
	mu_should(do_move_up(copy_up));
	mu_should(number_of_possible_moves(copy_up) == 1);
	mu_should(do_move_right(copy_up));

	/* but not for long */
	mu_should(number_of_possible_moves(copy_up) == 0);

	/* clean up */
	destroy_maze(copy_right);
	destroy_maze(copy_up);
	destroy_maze(m);
}

/*
 * given a passcode and a valid known path, test and see if it reaches
 * the end.
 */

/*
 * verification data from the problem statement.
 */

char *sample_shortest_paths[][2] = {
	{ GOOD_PASSCODE_1, GOOD_PATH_1 },
	{ GOOD_PASSCODE_2, GOOD_PATH_2 },
	{ GOOD_PASSCODE_3, GOOD_PATH_3 },
	{ BAD_PASSCODE, "DU" },
	{ NULL, NULL }
};

int sample_longest_paths[] = {
	GOOD_LONGEST_1,
	GOOD_LONGEST_2,
	GOOD_LONGEST_3,
	-1,
	0
};

MU_TEST(test_replay_moves) {
	printf("\n");
	for (int i = 0; sample_shortest_paths[i][0] != NULL; i++) {
		printf("\nreplaying path '%s' for pass code '%s':",
		       sample_shortest_paths[i][1],
		       sample_shortest_paths[i][0]);
		maze_state *m = create_maze(
		                        sample_shortest_paths[i][0],
		                        4, 4,
		                        0, 0,
		                        3, 3
		                );
		rscb *moves = rs_create_string(sample_shortest_paths[i][1]);
		int row;
		int col;
		while (rs_peekc(moves) != EOF) {
			char move = rs_getc(moves);
			get_position(m, &row, &col);
			printf("\nat %d, %d moving %c", row, col, move);
			bool valid = can_move(m, move);
			mu_should(valid);
			do_move(m, move);
		}
		printf("\nended at at %d, %d\n", m->row, m->col);
		const char *path = get_move_history(m);
		mu_should(strcmp(path, sample_shortest_paths[i][1]) == 0);
		free((void *)path);
		destroy_maze(m);
	}
	printf("\n");
}

/*
 * those valid paths were cited as shortest paths, so we can use them to
 * test our path finder logic as well.
 */

MU_TEST(test_shortest_path) {
	printf("\n");
	for (int i = 0; sample_shortest_paths[i][0] != NULL; i++) {
		printf("\nfind shortest path for pass code '%s':\n",
		       sample_shortest_paths[i][0]);
		maze_state *m = create_maze(
		                        sample_shortest_paths[i][0],
		                        4, 4,
		                        0, 0,
		                        3, 3
		                );
		maze_state *shortest = find_shortest_path(m);
		const char *path = get_move_history(shortest);
		bool found = at_end(shortest);
		bool passed = strcmp(path, sample_shortest_paths[i][1]) == 0;
		if (strcmp(shortest->passcode, BAD_PASSCODE) == 0) {
			found = true;
			passed = true;
		}
		printf("%s: result: %s wanted: '%s', got '%s'\n",
		       passed ? "green" : "red",
		       found ? "true" : "false",
		       sample_shortest_paths[i][1],
		       path);
		mu_should(found && passed);
		free((void *)path);
		destroy_maze(m);
	}
}

/*
 * check to see that we find the same length for longest paths
 */

MU_TEST(test_longest_path) {
	printf("\n");
	for (int i = 0; sample_shortest_paths[i][0] != NULL &&
	                sample_longest_paths[i] > 0; i++) {
		printf("\nfind longest path length for pass code '%s':\n",
		       sample_shortest_paths[i][0]);
		maze_state *m = create_maze(
		                        sample_shortest_paths[i][0],
		                        4, 4,
		                        0, 0,
		                        3, 3
		                );
		maze_state *longest = find_longest_path(m);
		bool found = at_end(longest);
		const char *path = get_move_history(longest);
		bool passed = longest->count_moves == sample_longest_paths[i];
		printf("%s: result: %s wanted: %d, got %d\n",
		       passed ? "green" : "red",
		       found ? "true" : "false",
		       sample_longest_paths[i],
		       longest->count_moves);
		mu_should(found && passed);
		free((void *)path);
		destroy_maze(m);
	}
}


MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	/* run your tests here */
	MU_RUN_TEST(test_test);
	MU_RUN_TEST(test_replay_moves);
	MU_RUN_TEST(test_shortest_path);
	MU_RUN_TEST(test_longest_path);
}

/*
 * master control:
 */

int
main(int argc, char *argv[]) {
	num_args = argc;
	the_args = argv;
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
