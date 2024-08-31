/* solution.c -- aoc 2016 10 -- troy brumley */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

#define TXBSTR_IMPLEMENTATION
#include "txbstr.h"

#define TXBMISC_IMPLEMENTATION
#include "txbmisc.h"

#define TXBFS_IMPLEMENTATION
#include "txbfs.h"

/*
 * a bot has two hands, when they are full of values
 * those values can be given to other bots or sent to
 * output bins. track if two particular values were
 * seen.
 */

typedef struct bot_node bot_node;
struct bot_node {
	bool low_bot;
	int low_dest;
	bool high_bot;
	int high_dest;
	int low_value;
	int high_value;
	bool full;
	bool seen_17;
	bool seen_61;
};

/*
 * a bin holds values that are no longer being
 * passed from bot to bot. using a stack for
 * multiple values was not needed but i've left
 * it in.
 */

typedef struct output_bin output_bin;
struct output_bin {
	fscb *fs;
};

/*
 * part of the scrambled input data are directions that
 * a bot is pre-loaded with a value.
 */

typedef struct load_command load_command;
struct load_command {
	int value;
	int bot;
};

/*
 * globals. input was actually about 20 bins, 210 bots, and about 20
   loads.
 */

int last_bin = -1;
output_bin bins[50];

int last_bot = -1;
bot_node bots[250];

int last_load = -1;
load_command loads[50];

void
initialize(void) {

	memset(bins, 0, sizeof(bins));
	for (int i = 0; i < 50; i++)
		bins[i].fs = fs_create(10);

	memset(bots, 0, sizeof(bots));
	for (int i = 0; i < 250; i++) {
		bots[i].low_value = -1;
		bots[i].high_value = -1;
		bots[i].full = false;
		bots[i].seen_17 = false;
		bots[i].seen_61 = false;
	}

	memset(loads, 0, sizeof(loads));
}

/*
 * give a value from somewhere (bot or load) to another bot.
 * bots can hold two values, and they are kept in order.
 * when a bot is full, it is eligible to run its programmed
 * instructions and send the values to other bots or a bin.
 *
 * the problem wants to know which bot saw two particular
 * values. they are flagged here.
 */

void
give_to_bot(bot_node *bot, int value) {
	assert(bot->low_value == -1 || bot->high_value == -1);
	if (value == 17)
		bot->seen_17 = true;
	if (value == 61)
		bot->seen_61 = true;
	if (bot->low_value == -1)
		bot->low_value = value;

	else
		bot->high_value = value;
	if (bot->low_value > bot->high_value) {
		int swap = bot->low_value;
		bot->low_value = bot->high_value;
		bot->high_value = swap;
	}
	if (bot->low_value != -1 && bot->high_value != -1)
		bot->full = true;
}

/*
 * put a value in an output bin. initially i thought
 * there might be more than one item going to a bin but
 * there isn't. i wrote a stack helper and used it
 * before discovering it wasn't needed.
 */

void
give_to_bin(output_bin *bin, int value) {
	fs_push(bin->fs, (void *)(long)value);
}

/*
 * part one and part two combined today.
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

	/*
	 * input is mixed format. parse and collect. keep track of the last
	 * used load command, bot, and output bin. since the input is also
	 * not in any meaningful order, max is our friend.
	 */

	char buffer[256];
	memset(buffer, 0, 256);

	initialize();

	while (fgets(buffer, 255, ifile)) {
		const char **tokens = split_string(buffer, " \n");

		if (equal_string(tokens[1], "value")) {
			last_load += 1;
			loads[last_load].value = strtol(tokens[2], NULL, 10);
			loads[last_load].bot = strtol(tokens[6], NULL, 10);
			free_split(tokens);
			continue;
		}

		if (equal_string(tokens[1], "bot")) {
			int bot = strtol(tokens[2], NULL, 10);
			bots[bot].low_bot = equal_string(tokens[6], "bot");
			bots[bot].low_dest = strtol(tokens[7], NULL, 10);
			bots[bot].high_bot = equal_string(tokens[11], "bot");
			bots[bot].high_dest = strtol(tokens[12], NULL, 10);
			if (!bots[bot].low_bot)
				last_bin = max(last_bin, bots[bot].low_dest);
			if (!bots[bot].high_bot)
				last_bin = max(last_bin, bots[bot].high_dest);
			last_bot = max(last_bot, bot);
			free_split(tokens);
			continue;
		}

		assert(NULL);
	}

	/*
	 * process the load commands, giving a value to a bot.
	 */

	for (int i = 0; i <= last_load; i++)
		give_to_bot(&bots[loads[i].bot], loads[i].value);

	/*
	 * at least one bot will have two values after the load commands
	 * complete. repeatedly scan through the bots for one with two
	 * values loaded, follow its programming for what to do with
	 * the values, and the restart the loop. do this until there
	 * are no more bots with two values.
	 */

	bool done = false;
	while (!done) {
		done = true;
		int i = 0;
		while (i <= last_bot) {
			if (bots[i].full) {
				done = false;
				if (bots[i].low_bot)
					give_to_bot(&bots[bots[i].low_dest], bots[i].low_value);

				else
					give_to_bin(&bins[bots[i].low_dest], bots[i].low_value);
				bots[i].low_value = -1;
				if (bots[i].high_bot)
					give_to_bot(&bots[bots[i].high_dest], bots[i].high_value);

				else
					give_to_bin(&bins[bots[i].high_dest], bots[i].high_value);
				bots[i].high_value = -1;
				bots[i].full = false;
				break;
			}
			i += 1;
		}
	}

	/*
	 * for part one, we want to know which bot handled values 17 and 61. this is
	 * tracked in give_to_bot().
	 *
	 * for part two, we want the product of the first three output bins, 0-2.
	 */

	int the_bot = -1;
	for (int i = 0; i <= last_bot; i++) {
		if (bots[i].seen_17 && bots[i].seen_61) {
			the_bot = i;
			break;
		}
	}

	printf("part one: %d\n", the_bot);

	int the_product = (long)fs_pop(bins[0].fs) *
	                  (long)fs_pop(bins[1].fs) *
	                  (long)fs_pop(bins[2].fs);
	printf("part two: %d\n", the_product);

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
	return EXIT_SUCCESS;
}
