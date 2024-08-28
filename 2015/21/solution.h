/* solution.h -- aoc 2015 13 -- troy brumley */
#ifndef SOLUTION_H
#define SOLUTION_H

#include <stdbool.h>
#include <stdio.h>


/* how long may a line in the test dataset be? */
#define INPUT_LEN_MAX 255

/*
 * a gear item description.
 */

typedef struct gear_t {
	const char *slot;
	const char *name;
	int cost;
	int damage;
	int armor;
} gear_t;

/*
 * an equipable gear set and its battle stats.
 */

typedef struct set_t {
	int total_hits;
	int total_cost;
	int total_armor;
	int total_damage;
	int final_hp;
	gear_t *weapon;
	gear_t *armor;
	gear_t *ring_one;
	gear_t *ring_two;
} set_t;

/*
 * a mob is a combatant, either the player or the boss in this
 * simulation.
 */

typedef struct mob_t {
	const char *tag;
	int hp;
	int damage;
	int armor;
	int curr_hp;
	set_t *equipped;
} mob_t;

/*
 * reset global state for next run:
 */

void
reset_all(
        void
);

void
strip_mob(
        mob_t *
);

mob_t *
read_mob(
        char *,
        FILE *
);

int
equip_mob(
        mob_t *
);

void
update_mob_stats(
        mob_t *
);

mob_t *
battle(
        mob_t *player,
        mob_t *boss,
        bool print
);

/*
 * the code for each of the day's parts.
 *
 * each expects the name of an input dataset or some other string to
 * drive the part's code.
 *
 * should print the result to stdout.
 *
 * returns EXIT_SUCCESS or EXIT_FAILURE.
 */

int
part_one(
        const char *                       /* a file name */
);

int
part_two(
        const char *                       /* a file name */
);


/* end solution.h */
#endif /* SOLUTION_H */
