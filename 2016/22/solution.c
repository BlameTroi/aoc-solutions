/* solution.c -- aoc 2016 xx -- troy brumley */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

#define TXBMISC_IMPLEMENTATION
#include "txbmisc.h"
#define TXBRS_IMPLEMENTATION
#include "txbrs.h"
#define TXBSTR_IMPLEMENTATION
#include "txbstr.h"
#define TXBDA_IMPLEMENTATION
#include "txbda.h"

node nodes_grid[X_DIM][Y_DIM];
int num_nodes = 0;
node *nodes_list = NULL;

void
load_grid(rscb *input) {

	/* clear any priors grid */
	memset(nodes_grid, 0, sizeof(nodes_grid));
	if (nodes_list != NULL) {
		free(nodes_list);
		nodes_list = NULL;
	}
	num_nodes = 0;

	/* create a string of all non-numeric characters for
	 * split_string(). input is positional and none of
	 * the verbiage matters for this problem.
	 *
	 * lines we care about start with /dev, and we only
	 * need the six numeric fields:
	 *
	 * Filesystem              Size  Used  Avail  Use%
	 * /dev/grid/node-x0-y0     90T   69T    21T   76%
	 * /dev/grid/node-x0-y1     88T   71T    17T   80%
	 *                 1  2      3     4      5     6   */

	char non_numerics[129];
	int i = 0;
	for (int c = ' '; c <= 127; c++) {
		if (!is_digit(c)) {
			non_numerics[i] = c;
			non_numerics[i+1] = '\0';
			i += 1;
		}
	}

	/* read, parse, store */

	char buffer[256];
	int buflen = 256;
	rs_rewind(input);
	while (!rs_at_end(input)) {
		if (rs_gets(input, buffer, buflen) && buffer[0] == '/') {
			const char **tokens = split_string(buffer, non_numerics);
			assert(tokens[1] && tokens[2] && tokens[3] && tokens[4] && tokens[5] &&
			       tokens[6]);
			int x = strtol(tokens[1], NULL, 10);
			int y = strtol(tokens[2], NULL, 10);
			nodes_grid[x][y].loc = (loc) {
				x, y
			};
			nodes_grid[x][y].size = strtol(tokens[3], NULL, 10);
			nodes_grid[x][y].used = strtol(tokens[4], NULL, 10);
			nodes_grid[x][y].avail = strtol(tokens[5], NULL, 10);
			nodes_grid[x][y].pct = strtol(tokens[6], NULL, 10);
			printf("%s\n", buffer);
			node *n = &nodes_grid[x][y];
			printf("x%d-y%d size=%d used=%d avail=%d pct=%d\n", n->loc.x, n->loc.y, n->size,
			       n->used, n->avail, n->pct);
			num_nodes += 1;
			free_split(tokens);
		}
	}

	/* get in a list to try something out */
	nodes_list = malloc(num_nodes *sizeof(node));
	i = 0;
	for (int x = 0; x < X_DIM; x++) {
		for (int y = 0; y < Y_DIM; y++) {
			nodes_list[i] = nodes_grid[x][y];
			i = i + 1;
		}
	}
}

/* To do this, you'd like to count the number of viable pairs of
   nodes. A viable pair is any two nodes (A,B), regardless of whether
   they are directly connected, such that:

    Node A is not empty (its Used is not zero).
    Nodes A and B are not the same node.
    The data on node A (its Used) would fit on node B (its Avail).

 * How many viable pairs of nodes are there?
 */

bool
viable_pairing(const node *a, const node *b) {
	if (a == b)
		return false;
	if (a->used == 0)
		return false;
	if (a->used > b->avail)
		return false;
	return true;
}

int
check_viable_with(const node *a) {
	if (a->used == 0)
		return 0;
	int viable = 0;
	for (int x = 0; x < X_DIM; x++) {
		for (int y = 0; y < Y_DIM; y++) {
			if (viable_pairing(a, &nodes_grid[x][y]))
				viable += 1;
		}
	}
	return viable;
}

int
viable_node_pairs(void) {
	int viable = 0;
	for (int x = 0; x < X_DIM; x++) {
		for (int y = 0; y < Y_DIM; y++)
			viable += check_viable_with(&nodes_grid[x][y]);
	}
	return viable;
}

int
viable_node_pairs2(void) {
	int viable = 0;

	for (int i = 0; i < num_nodes; i++) {
		for (int j = i+1; j < num_nodes; j++) {
			if (nodes_list[i].used != 0 && nodes_list[i].used <= nodes_list[j].avail)
				viable += 1;
			if (nodes_list[j].used != 0 && nodes_list[j].used <= nodes_list[i].avail)
				viable += 1;
		}
	}

	return viable;
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

	rscb *rs = rs_create_string_from_file(ifile);
	load_grid(rs);

	printf("part one: %d\n", viable_node_pairs());
	printf("part one: %d\n", viable_node_pairs2());

	free(rs);
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

	while (fgets(iline, INPUT_LINE_MAX - 1, ifile)) {
	}

	printf("part two: %d\n", 0);

	fclose(ifile);
	return EXIT_SUCCESS;
}
