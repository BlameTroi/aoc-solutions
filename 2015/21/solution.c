/* solution.c -- aoc 2015 21 -- troy brumley */

/*
 * advent of code 2015 day 21 is a rpg battle simulator where we are tasked
 * with finding the cheapest survivable gearset for a player vs a boss. there
 * is no cap on player spending, but the floor is that a weapon must be used.
 * all other gear items are optional.
 *
 * this is a try all combinations problem.
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TXBMISC_IMPLEMENTATION
#include "txbmisc.h"

#define TXBSTR_IMPLEMENTATION
#include "txbstr.h"

#include "solution.h"

/*
 * the inventory of gear available to the player, extracted from the
 * problem specification. it is possible for the player to have no
 * armor or rings, and so a NULL item is added to the gear for each
 * slot.
 *
 * a weapon must always be equipped so there is no NULL weapon.
 *
 * rings must be unique, you can't have two of the same except for
 * the NULL ring.
 */

gear_t weapons[] = {
	{ "Weapon", "Dagger", 8, 4, 0 },
	{ "Weapon", "Shortsword", 10, 5, 0 },
	{ "Weapon", "Warhammer", 25, 6, 0 },
	{ "Weapon", "Longsword", 40, 7, 0 },
	{ "Weapon", "Greataxe", 74, 8, 0 },
};
const int weapons_max = sizeof(weapons) / sizeof(gear_t);

gear_t armors[] = {
	{ "Armor", "NULL", 0, 0 },
	{ "Armor", "Leather", 13, 0, 1 },
	{ "Armor", "Chainmail", 31, 0, 2 },
	{ "Armor", "Splintmail", 53, 0, 3 },
	{ "Armor", "Bandedmail", 75, 0, 4 },
	{ "Armor", "Platemail", 102, 0, 5 },
};
const int armors_max = sizeof(armors) / sizeof(gear_t);

gear_t rings[] = {
	{ "Ring", "NULL", 0, 0, 0 },
	{ "Ring", "Damage +1", 25, 1, 0 },
	{ "Ring", "Damage +2", 50, 2, 0 },
	{ "Ring", "Damage +3", 100, 3, 0 },
	{ "Ring", "Defense +1", 20, 0, 1 },
	{ "Ring", "Defense +2", 40, 0, 2 },
	{ "Ring", "Defense +3", 80, 0, 3 },
};
const int rings_max = sizeof(rings) / sizeof(gear_t);

/*
 * there are 5 weapons, 5 armor plus 1 null, and 6 rings plus 1 null (or 2
 * depending on how you look at it). all the possible sets are generated
 * at the start of a run from the gear tables above.
 */

const int sets_max = weapons_max * armors_max * rings_max * rings_max;
set_t gear_sets[sets_max];

/*
 * determine total cost, damage, and armor stats for a gear set.
 * remember that the player must always have a weapon equipped, but
 * any of the other slots may be empty.
 */

void
update_set_stats(
        set_t *p
) {
	assert(p);
	p->total_cost = p->weapon->cost;
	p->total_damage = p->weapon->damage;
	p->total_armor = 0;
	if (p->armor != NULL) {
		p->total_cost += p->armor->cost;
		p->total_armor += p->armor->armor;
	}
	if (p->ring_one != NULL) {
		p->total_cost += p->ring_one->cost;
		p->total_armor += p->ring_one->armor;
		p->total_damage += p->ring_one->damage;
	}
	if (p->ring_two != NULL) {
		p->total_cost += p->ring_two->cost;
		p->total_armor += p->ring_two->armor;
		p->total_damage += p->ring_two->damage;
	}
}

/*
 * generate all the possible gear sets.
 *
 * it's an odometerization process. all the gear sets are stored for
 * later use even though the initial problem statement could be done
 * without keeping the sets.
 */

void
generate_gear_sets(
        void
) {

	int curr_set = 0;

	int weapon_ix = 0;
	int armor_ix = 0;
	int ring_one_ix = 0;
	int ring_two_ix = 0;

	bool not_done = true;

	while (not_done) {

		/* skip duplicate rings when building gear sets */
		if (ring_one_ix != ring_two_ix ||       /* rings must not be the same */
		                ring_one_ix == 0) {               /* unless they are the null ring */
			set_t *p = &gear_sets[curr_set];
			p->weapon = &weapons[weapon_ix];
			p->armor = &armors[armor_ix];
			p->ring_one = &rings[ring_one_ix];
			p->ring_two = &rings[ring_two_ix];
			update_set_stats(p);
			curr_set += 1;
		}

		/* odometerish increment */
		ring_two_ix += 1;
		if (ring_two_ix == rings_max) {
			ring_two_ix = 0;
			ring_one_ix += 1;
			if (ring_one_ix == rings_max) {
				ring_one_ix = 0;
				armor_ix += 1;
				if (armor_ix == armors_max) {
					armor_ix = 0;
					weapon_ix += 1;
					if (weapon_ix == weapons_max)
						not_done = false;
				}
			}
		}
	}
}

/*
 * calculate starting hp, damage, and armor class based on equipped
 * gear slots. all gear slots can be empty (NULL) except for the
 * weapon slot.
 */

void
update_mob_stats(
        mob_t *mob
) {
	mob->curr_hp = mob->hp;
	mob->damage = mob->equipped->weapon->damage +
	              (mob->equipped->ring_one ? mob->equipped->ring_one->damage : 0) +
	              (mob->equipped->ring_two ? mob->equipped->ring_two->damage : 0);
	mob->armor = (mob->equipped->armor ? mob->equipped->armor->armor : 0) +
	             (mob->equipped->ring_one ? mob->equipped->ring_one->armor : 0) +
	             (mob->equipped->ring_two ? mob->equipped->ring_two->armor : 0);
}

/*
 * remove the gear and reflect the changes in the mob's current stats.
 * support for the boss mob isn't needed. when being stripped, the
 * player mob should have had a weapon equipped.
 */

void
strip_mob(
        mob_t *mob
) {
	assert(mob);
	assert(mob->equipped->weapon &&
	       strcmp(mob->tag, "Boss") != 0);
	if (mob->equipped->weapon) {
		mob->damage -= mob->equipped->weapon->damage;
		mob->equipped->weapon = NULL;
	}
	if (mob->armor) {
		mob->armor -= mob->equipped->armor->armor;
		mob->equipped->armor = NULL;
	}
	if (mob->equipped->ring_one) {
		mob->armor -= mob->equipped->ring_one->armor;
		mob->damage -= mob->equipped->ring_one->damage;
		mob->equipped->ring_one = NULL;
	}
	if (mob->equipped->ring_two) {
		mob->armor -= mob->equipped->ring_two->armor;
		mob->damage -= mob->equipped->ring_two->damage;
		mob->equipped->ring_two = NULL;
	}
	assert(mob->armor == 0);
	assert(mob->damage == 0);
}

/*
 * read a mob definition from a configuration file. the format is
 * loosely "keyword: value". each mob has a tag name preceeding its
 * section.
 *
 * gearing was not part of the original mob definition, just the basic
 * stats for the boss, while player hit points were given as a
 * constant. i expanded the definition for testing to include the
 * player since the test battle provided did not have any gear
 * information, just hit points, damage, and armor stats.
 *
 * the data is read from a stream. the stream is rewound at the
 * start of this function and its position is unreliable when
 * this function completes.
 */

mob_t *
read_mob(
        char *tag,
        FILE *from
) {
	/* per `man 3 rewind`, while rewind does not return an error,
	 * we can check to see if one occured this way. */
	errno = 0;
	rewind(from);
	assert(errno == 0);

	/* position to requested mob tag. */
	char iline[INPUT_LEN_MAX];
	const char **tokens = NULL;
	bool found = false;

	while (!found && fgets(iline, INPUT_LEN_MAX - 1, from)) {
		tokens = split_string(iline, ": ");
		if (tokens[1] && strcmp(tokens[1], tag) == 0)
			found = true;
	}

	assert(found);

	/* read in values until eof or unknown tag found */
	mob_t *mob = malloc(sizeof(mob_t));
	memset(mob, 0, sizeof(mob_t));
	mob->tag = tag;

	while (fgets(iline, INPUT_LEN_MAX - 1, from)) {
		tokens = split_string(iline, ": ");
		if (tokens[1] &&
		                strcmp(tokens[1], "Hit") == 0 &&
		                strcmp(tokens[2], "Points") == 0) {
			mob->hp = atol(tokens[3]);
			mob->curr_hp = mob->hp;
			continue;
		}
		if (tokens[1] &&
		                strcmp(tokens[1], "Damage") == 0) {
			mob->damage = atol(tokens[2]);
			continue;
		}
		if (tokens[1] &&
		                strcmp(tokens[1], "Armor") == 0) {
			mob->armor = atol(tokens[2]);
			continue;
		}
		break;
	}

	return mob;

}

/*
 * prepare a mob for battle. they should already be geared,
 * so just update their running hit points.
 */

void
ready_mob(
        mob_t *mob
) {
	mob->curr_hp = mob->hp;
}

/*
 * battle damage is straight forward. the aggressor's damage total
 * less the defender's armor total, but the aggressor will always
 * do at least one point of damage.
 */

int
hit(
        mob_t *aggressor,
        mob_t *defender
) {
	return max(1, aggressor->damage - defender->armor);
}

/*
 * a battle between two mobs. the mob designated player always attacks
 * first in a turn. the battle ends when either mob has current hit
 * points less than 1. returns the victor.
 */

mob_t *
battle(
        mob_t *player,
        mob_t *boss,
        bool print
) {
	assert(player &&
	       player->equipped->weapon &&
	       player->hp > 0);
	ready_mob(player);

	assert(boss &&
	       boss->hp > 0);
	ready_mob(boss);

	if (print)
		printf("\nare you not entertained?\n");

	/* continue until someone dies */

	player->equipped->total_hits = 0;

	while (true) {

		/* player hits boss */
		int player_hit = hit(player, boss);
		player->equipped->total_hits += 1;
		boss->curr_hp -= player_hit;
		if (print)
			printf("player hits boss for %d leaving boss with %d hp\n", player_hit,
			       boss->curr_hp);
		if (boss->curr_hp < 1) {
			if (print)
				printf("boss is dead\n");
			return player;
		}

		/* boss hits player */
		int boss_hit = hit(boss, player);
		player->curr_hp -= boss_hit;
		player->equipped->final_hp = player->curr_hp;
		if (print)
			printf("boss hits player for %d leaving player with %d hp\n", boss_hit,
			       player->curr_hp);
		if (player->curr_hp < 1) {
			if (print)
				printf("player is dead\n");
			return boss;
		}

		/* and the beat goes on ... */
	}
}

/*
 * reset any global state. so far, that's just clearing the
 * gear sets.
 */

void
reset_all(
        void
) {
	memset(gear_sets, 0, sizeof(gear_sets));
	generate_gear_sets();
}

/*
 * part one:
 *
 */

int
part_one(
        const char *fname
) {

	reset_all();

	FILE *ifile = fopen(fname, "r");
	if (!ifile) {
		printf("could not open file: %s\n", fname);
		return EXIT_FAILURE;
	}
	mob_t *player = read_mob("Player", ifile);
	mob_t *boss = read_mob("Boss", ifile);

	int player_wins = 0;
	int player_min_cost = 99999;
	int player_max_cost = -1;
	int player_min_hp = 99999;
	int player_max_hp = -1;
	int boss_wins = 0;
	int boss_min_hp = 99999;
	int boss_max_hp = -1;
	int boss_win_player_min_cost = 99999;
	int boss_win_player_max_cost = -1;
	for (int i = 0; i < sets_max; i++) {
		if (gear_sets[i].weapon) { /* unused slots will have no weapon */
			player->equipped = &gear_sets[i];
			update_mob_stats(player);
			ready_mob(player);
			ready_mob(boss);
			/* printf("*** battle %d ***\n", i); */
			mob_t *victor = battle(player, boss, false);
			if (victor == player) {
				player_wins += 1;
				player_max_cost = max(player_max_cost, player->equipped->total_cost);
				player_min_cost = min(player_min_cost, player->equipped->total_cost);
				player_min_hp = min(player_min_hp, player->curr_hp);
				player_max_hp = max(player_max_hp, player->curr_hp);
			} else {
				boss_wins += 1;
				boss_min_hp = min(boss_min_hp, boss->curr_hp);
				boss_max_hp = max(boss_max_hp, boss->curr_hp);
				boss_win_player_min_cost = min(boss_win_player_min_cost,
				                               player->equipped->total_cost);
				boss_win_player_max_cost = max(boss_win_player_max_cost,
				                               player->equipped->total_cost);
			}
			strip_mob(player);
		}
	}

	printf("part one: %d\n", player_min_cost);
	printf("part two: %d\n", boss_win_player_max_cost);

	fclose(ifile);
	return EXIT_SUCCESS;
}


/*
 * part two:
 *
 * combined with part one.
 */

int
part_two(
        const char *fname
) {

	return EXIT_SUCCESS;
}
