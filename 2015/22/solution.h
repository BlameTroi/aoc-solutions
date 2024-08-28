/* solution.h -- aoc 2015 22 -- troy brumley */
#ifndef SOLUTION_H
#define SOLUTION_H

#include <stdbool.h>


/* how long may a line in the test dataset be? */
#define INPUT_LEN_MAX 255

/*
 * the player has five spells.
 */

#define MAGIC_MISSLE 0
#define DRAIN 1
#define SHIELD 2
#define POISON 3
#define RECHARGE 4
#define SPELL_MAX 5

/*
 * battle state includes pertinent stats for the opponents and timers
 * for 'x' over time effects.
 */

typedef struct battle_state battle_state;

struct battle_state {
	int turn;
	int casting;
	int player_hp;
	int player_mana;
	int player_temp_armor;
	int mana_used;
	int boss_hp;
	int boss_damage;
	bool hard_mode;
	int spell_timers[SPELL_MAX];
};

/*
 * a spell has a name and methods to determine if it can be cast,
 * casting, and hanlding 'x' over time effects.
 */

typedef bool (*fncan)(battle_state *);
typedef void (*fncast)(battle_state *);
typedef void (*fntick)(battle_state *);

typedef struct spell spell;

struct spell {
	char *name;
	fncan can_cast;
	fncast do_cast;
	fntick do_tick;
};

/*
 * methods for each spell are exposed here for unit testing.
 */

bool
can_magic_missle(battle_state *);
void
cast_magic_missle(battle_state *);
void
null_tick(battle_state *);
bool
can_drain(battle_state *);
void
cast_drain(battle_state *);
bool
can_shield(battle_state *);
void
cast_shield(battle_state *);
void
tick_shield(battle_state *);
bool
can_poison(battle_state *);
void
cast_poison(battle_state *);
void
tick_poison(battle_state *);
bool
can_recharge(battle_state *);
void
cast_recharge(battle_state *);
void
tick_recharge(battle_state *);

/*
 * apply any 'x' over time effects during a turn.
 */

void
tick_spells(battle_state *);

/*
 * utilities for battle state.
 */

void
print_battle_state(battle_state *);

battle_state
create_battle_state(
        int player_hp,
        int player_mana,
        int boss_hp,
        int boss_damage,
        bool hard_mode
);

/*
 * a round in the battle, first the player acts and then
 * the boss. this is recursive to try all possible spell
 * combinations. the boss has only one attack.
 */
int
battle_round(
        battle_state *,
        int best_result_yet
);

/*
 * run all the possible battles and return the least mana spent in a
 * player victory. for hard mode, the player takes one point of damage
 * at the start of their turn.
 */
int
battle(
        int player_hp,
        int player_mana,
        int boss_hp,
        int boss_damage,
        bool hard_mode
);

/*
 * reset global state for next run:
 */
void
reset_all(
        void
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
