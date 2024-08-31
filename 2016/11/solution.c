/* solution.c -- aoc 2016 11 -- troy brumley */

/*
 * the problem is a different spin on towers of hanoi i think. move
 * things around between four floors to get them all to the top floor
 * while honoring restrictions (a match rule instead of size
 * ordering).
 *
 * actually the class of problems is 'river crossing'.
 *
 * what is the fewest number of moves required to get all parts to
 * the top floor of a four floor fabrication lab?
 *
 * the input dataset is several somewhat regular sentences but rather
 * than write a parser, i sketched the layout and codified it in
 * init_and_load() which sets the initial state for both the sample
 * from the problem description and the live data.
 *
 * live run: (??? steps from this)
 *
 *  1 elv prg prm
 *  2     cog cug rug plg
 *  3     com cum rum plm
 *  4 nil
 *
 * sample run: (11 steps from this)
 *
 *  1 elv hym lim
 *  2     hyg
 *  3     lig
 *  4 nil
 *
 * the rules of transport:
 *
 *  elevator will only move with one or two items.
 *
 *  elevator stops at each floor. this stop is the move counter.
 *
 * if a chip ends a move on a floor with any generators, that chip's
 * matching generator must be on the floor as well.
 *
 * so: hyg hym lig is legal,
 *
 * but: hym lig is not.
 *
 * there are only a few items and i decided to use bit masks. i don't
 * know if it will make any runtime difference, but fiddling bits is
 * something that needs to be done from time to time, so practice is
 * worthwhile.
 *
 * conclusions on part one:
 *
 * i and most everyone i've seen mention it on reddit or the web
 * regard this as a bitch of a problem to compute. there are so many
 * paths that if you don't figure out how to prune the search it just
 * takes too long.
 *
 * like hours or days depending on your system and language. many found
 * it quicker to write a 'game' interface and solve the problem manually.
 *
 * i tripped myself up a few times with byte ordering when i was working
 * through this, which slowed me down considerably, but even if i'd done
 * that right from the start i don't know that i would have figured out
 * the trick--which is that you don't need to compare every item to
 * prune duplicate paths from the search. the 'elements' of the two
 * kinds of only matters so long as the chip-generator constraint is
 * satisfied.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

#define TXBMISC_IMPLEMENTATION
#include "txbmisc.h"

#define TXBQU_IMPLEMENTATION
#include "txbqu.h"

/*
 * the number of distinct elements in the problem is the key
 * information. there are two in the illustrative sample and five in
 * the live data (see solution.h defines). each element will have
 * corresponding microchips and generators.
 *
 * num_elements will restrict the number of elements we check so we
 * don't waste cycles on impossible configurations.
 */

static int num_elements = 0;
static uint8_t all_elements = 0;

/*
 * avoid chasing duplicate paths. if we have already seen a particular
 * configuration, skip it. as long as a move state is legal, we only
 * care about the number of items on each floor for checking. at least
 * that's the reddit hive mind wisdom.
 *
 * the volumes are getting high enough that a hash table is something
 * i am going to have to consider implementing for my libraries. i've
 * spent enough time on this problem so i'm not going to worry about
 * doing that or a binary search here.
 *
 * on the part one data (5 elements, 20 parts) we had 3,724 duplicates
 * generated, and skipped them 32,231 times. on part two data (7
 * elements, 28 parts) we had 12,445 duplicates generated and skipped
 * them 158,256 times.
 */

static dupcheck *duplicates = NULL;
static int num_duplicates = 0;
static int skipped_duplicates = 0;
#define DUPLICATES_MAX 25000

void
init_duplicates(int size) {
	if (duplicates) {
		free(duplicates);
		duplicates = NULL;
		skipped_duplicates = 0;
		num_duplicates = 0;
	}
	duplicates = malloc((1 + size) * sizeof(dupcheck));
	memset(duplicates, 0, (1 + size) * sizeof(dupcheck));
	num_duplicates = 0;
	skipped_duplicates = 0;
}

dupcheck
state_as_dupcheck(
        state *m
) {
	dupcheck dm;
	memset(&dm, 0, sizeof(dupcheck));
	dm.elevator = m->elevator;
	dm.counts[f4][0] = one_bits_in(m->microchips[f4]);
	dm.counts[f4][1] = one_bits_in(m->generators[f4]);
	dm.counts[f3][0] = one_bits_in(m->microchips[f3]);
	dm.counts[f3][1] = one_bits_in(m->generators[f3]);
	dm.counts[f2][0] = one_bits_in(m->microchips[f2]);
	dm.counts[f2][1] = one_bits_in(m->generators[f2]);
	dm.counts[f1][0] = one_bits_in(m->microchips[f1]);
	dm.counts[f1][1] = one_bits_in(m->generators[f1]);
	return dm;
}

void
add_duplicate(state *m) {
	duplicates[num_duplicates] = state_as_dupcheck(m);
	num_duplicates += 1;
}

bool
is_duplicate(state *m) {
	dupcheck dm = state_as_dupcheck(m);
	for (int i = 0; i < num_duplicates; i++) {
		if (memcmp(&duplicates[i], &dm, sizeof(dupcheck)) == 0)
			return true;
	}
	return false;
}

/*
 * trace output support.
 */

char *
trace_line(
        state *m,
        char *buffer,
        int maxlen
) {
	snprintf(
	        buffer, maxlen,
	        "%c%c  %5d  %c[%02X %02X]  %c[%02X %02X]  %c[%02X %02X]  %c[%02X %02X]",
	        is_goal(m) ? 'G' : ' ',
	        is_valid(m) ? ' ' : 'E',
	        m->no,
	        m->elevator == f1 ? '>' : ' ', m->microchips[f1], m->generators[f1],
	        m->elevator == f2 ? '>' : ' ', m->microchips[f2], m->generators[f2],
	        m->elevator == f3 ? '>' : ' ', m->microchips[f3], m->generators[f3],
	        m->elevator == f4 ? '>' : ' ', m->microchips[f4], m->generators[f4]
	);
	return buffer;
}

/*
 * is this move a valid combination?
 *
 * if a chip ends a move on a floor with any generators, that chip's
 * matching generator must be on the floor as well.
 *
 * so: hyg hym lig is legal,
 *
 * but: hym lig is not.
 */

bool
is_valid(state *m) {
	for (int f = f1; f <= f4; f++) {
		if (m->generators[f] != 0 &&
		                (m->microchips[f] & m->generators[f]) != m->microchips[f])
			return false;
	}
	return true;
}

/*
 * the desired end is everything on the fourth floor.
 */

bool
is_goal(
        state *m
) {
	return m->elevator == f4 &&
	       m->microchips[f4] != 0 && m->generators[f4] != 0 &&
	       m->microchips[f3] == 0 && m->generators[f3] == 0 &&
	       m->microchips[f2] == 0 && m->generators[f2] == 0 &&
	       m->microchips[f1] == 0 && m->generators[f1] == 0;
}

/*
 * helpers to create new candidate moves from an existing move. the
 * sequence of create move, queue if valid, clean up, was repeated a
 * dozen times.
 *
 * clone the existing move, increment the move number, adjust the
 * elevator floor, and flip the part bits to move the parts to the new
 * floor (bitwise OR on the new floor, bitwise XOR on the old floor),
 * check validity, enqueue if valid, free if not.
 */

void
move_up(qucb *q, state *m, uint8_t microchips, uint8_t generators) {
	state *nm = malloc(sizeof(*nm));
	memcpy(nm, m, sizeof(*nm));
	nm->no += 1;
	nm->elevator += 1;
	nm->microchips[nm->elevator] |= microchips;
	nm->generators[nm->elevator] |= generators;
	nm->microchips[m->elevator] ^= microchips;
	nm->generators[m->elevator] ^= generators;
	if (is_valid(nm))
		qu_enqueue(q, nm);

	else
		free(nm);
}

void
move_down(qucb *q, state *m, uint8_t microchips, uint8_t generators) {
	state *nm = malloc(sizeof(*nm));
	memcpy(nm, m, sizeof(*nm));
	nm->no += 1;
	nm->elevator -= 1;
	nm->microchips[nm->elevator] |= microchips;
	nm->generators[nm->elevator] |= generators;
	nm->microchips[m->elevator] ^= microchips;
	nm->generators[m->elevator] ^= generators;
	if (is_valid(nm))
		qu_enqueue(q, nm);

	else
		free(nm);
}

/*
 * from a given valid start state, provide a queue of possible valid
 * next states. caller is responsible for deallocating the returned
 * queue via qu_destroy().
 *
 * iterate through each possible element
 * if there's a generator for that element
 *   try a move of just that generator
 *   for all the other elements, try to move the generator and any
 *       one generator or microchip for the other elements.
 * and the same for microchips
 *
 * i don't worry about duplicates here. they are dealt with in
 * seak_goal();
 */

qucb *
next_moves(state *m) {
	qucb *q = qu_create();

	/* moves are restricted to one floor in any legal direction. */
	int v = m->elevator;
	bool can_go_up = v < f4;
	bool can_go_down = v > f1;

	/* loop control variables and bit masks for the elements that
	 * distinquish generator from generator, microchip from microchip,
	 * and let us match a microchip and generator.
	 *
	 * this_* is the outer loop
	 * that_* is the inner loop
	 *
	 * so if we have a floor with 1g 2g 3g 1m 2m, for element this_ of
	 * 1 we woul dhave candidate moves up and down for:
	 *
	 * 1g
	 * 1g 2g
	 * 1g 3g
	 * 1g 1m
	 * 1g 2m          */
	uint8_t this_element = 0;
	uint8_t that_element = 0;
	uint8_t this_mask = 0;
	uint8_t that_mask = 0;

	while (this_element < num_elements) {
		this_mask = 1 << this_element;

		/* is there a generator for this element? */
		if (m->generators[v] & this_mask) {

			/* yes, first move only it. */
			if (can_go_up)
				move_up(q, m, 0, this_mask);
			if (can_go_down)
				move_down(q, m, 0, this_mask);

			/* now move it and one of everything else on the floor */
			that_element = 0;

			while (that_element < num_elements) {
				that_mask = 1 << that_element;

				/* if there's a generator for the other element element, move it and
				 * this one. there will be an extra move created but it will be
				 * a duplicate and ignored. */

				if (m->generators[v] & that_mask) {
					if (can_go_up)
						move_up(q, m, 0, this_mask | that_mask);
					if (can_go_down)
						move_down(q, m, 0, this_mask | that_mask);
				}

				/* and now move the generator and any microchip. */

				if (m->microchips[v] & that_mask) {
					if (can_go_up)
						move_up(q, m, that_mask, this_mask);
					if (can_go_down)
						move_down(q, m, that_mask, this_mask);
				}
				that_element += 1;
			}
		}

		/* and now do all that stuff above from a microchip perspective.
		 * so, is there a microchip for this element? */

		if (m->microchips[v] & this_mask) {
			/* yes, first move only it. */
			if (can_go_up)
				move_up(q, m, this_mask, 0);
			if (can_go_down)
				move_down(q, m, this_mask, 0);

			/* now move it and one of everything else on the floor */
			that_element = 0;

			while (that_element < num_elements) {
				that_mask = 1 << that_element;

				/* if there's a generator for the other element element, move it
				 * and this microchip. */

				if (m->generators[v] & that_mask) {
					if (can_go_up)
						move_up(q, m, this_mask, that_mask);
					if (can_go_down)
						move_down(q, m, this_mask, that_mask);
				}

				if (m->microchips[v] & that_mask) {
					if (can_go_up)
						move_up(q, m, that_mask | this_mask, 0);
					if (can_go_down)
						move_down(q, m, that_mask | this_mask, 0);
				}
				that_element += 1;
			}
		}

		this_element += 1;
	}

	return q;
}

/*
 * seek a path to the goal state of everything on the fourth floor.
 *
 * seeking along a path is pruned if we come to an already tried
 * configuration,
 */

int
seek_goal(
        state *start
) {

	qucb *trials = qu_create();
	init_duplicates(DUPLICATES_MAX); /* wastefully large */

	state *copy = malloc(sizeof(state));
	memcpy(copy, start, sizeof(state));

	qu_enqueue(trials, copy);
	add_duplicate(copy);

	char buffer[256];
	memset(buffer, 0, 256);

	state *m = NULL;
	while (!qu_empty(trials)) {
		m = qu_dequeue(trials);
		/* printf("%s\n", trace_line(m, buffer, 255)); */
		if (is_goal(m)) {
			/* printf("\nGOAL\n"); */
			/* printf("num_duplicates %d\n", num_duplicates); */
			/* printf("duplicates_skipped %d\n", duplicates_skipped); */
			break;
		}
		qucb *possibilities = next_moves(m);
		while (!qu_empty(possibilities)) {
			state *next = qu_dequeue(possibilities);
			if (is_duplicate(next)) {
				skipped_duplicates += 1;
				free(next);
				next = NULL;
			} else {
				add_duplicate(next);
				qu_enqueue(trials, next);
				next = NULL;
			}
		}
		qu_destroy(possibilities);
	}

	int moves = m->no;
	free(trials);
	free(m);

	return moves;
}

/*
 * use either the live problem data or the sample.
 */

void
init_and_load(
        state *m,
        bool use_live
) {

	memset(m, 0, sizeof(*m));
	m->no = 0;
	m->elevator = f1;

	/* for the live run, five elements */

	if (use_live) {
		/*  1 elv prg prm
		    2     cog cug rug plg
		    3     com cum rum plm
		    4 nil */
		m->generators[f1] = promethium;
		m->microchips[f1] = promethium;
		m->generators[f2] = cobalt | curium | ruthenium | plutonium;
		m->microchips[f3] = cobalt | curium | ruthenium | plutonium;
		all_elements = promethium | cobalt | curium | ruthenium | plutonium;
		num_elements = LIVE_ELEMENTS;
	}

	/* for the exmaple run from the site, two elements */

	if (!use_live) {
		/*  1 elv hym lim
		    2     hyg
		    3     lig
		    4 nil */
		m->microchips[f1] = hydrogen | lithium;
		m->generators[f2] = hydrogen;
		m->generators[f3] = lithium;
		all_elements = hydrogen | lithium;
		num_elements = SAMPLE_ELEMENTS;
	}
}

/*
 * part one:
 */

int
part_one(
        const char *fname
) {

	state start;
	init_and_load(&start, true);
	int res = seek_goal(&start);

	/* printf("%d %d\n", num_duplicates, skipped_duplicates); */

	printf("part one: %d\n", res);

	return EXIT_SUCCESS;
}

/*
 * part two:
 */

int
part_two(
        const char *fname
) {

	state start;
	init_and_load(&start, true);

	/* for part two, some extra stuff was found on the first floor */

	start.microchips[f1] |= elerium | dilithium;
	start.generators[f1] |= elerium | dilithium;

	num_elements = PART_TWO_ELEMENTS;
	all_elements |= elerium | dilithium;

	int res = seek_goal(&start);

	/* printf("%d %d\n", num_duplicates, skipped_duplicates); */

	printf("part two: %d\n", res);

	return EXIT_SUCCESS;
}
