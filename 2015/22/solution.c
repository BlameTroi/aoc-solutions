/* solution.c -- aoc 2015 22 -- troy brumley */

/*
 * advent of code 2015 day 22 is a rpg battle simulator where we are
 * tasked with finding the least amount of mana that a player must
 * expend to survive a battle with a particular boss. spells are
 * limited, effects can not be stacked. the player starts with a fixed
 * amount of mana but has a regeneneration spell available.
 */

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TXBMISC_IMPLEMENTATION
#include "txbmisc.h"

#include "solution.h"

/*
 * changing up from day 21 from melee to caster. the player has no
 * gear and no starting armor, but does have a list of spells that may
 * be cast with varying effects. spells can not stack, but may be cast
 * again once their duration expires.
 *
 * the spells:
 *
 * spell  cost   duration    damage     hp    armor    mana
 * mm     53                 4          0      0        0
 * dr     73                 2          2      0        0
 * sh     113    6           0          0      7        0
 * po     173    6           3          0      0        0
 * re     229    5           0          0      0        101
 *
 * the recharge mana effect and drain effect are additive for the
 * player for each tick, while the shield armor effect is constant
 * over time.
 *
 * a spell may be cast if it's not currently on countdown and the
 * player has sufficient mana.
 *
 * as the problem wants us to find the least amount of mana spent to a
 * victory, all possible paths will be checked. there's no need to
 * pick the best spell.
 */

/*
 * going a little smalltalkish objectish here.
 *
 * each spell has a "can i cast" and "cast" function. the cast
 * function applies any immediate changes to the battle state (reduce
 * mana, damage the opponent) and will place the spell on timer if it
 * has duration.
 *
 * at the start of each turn (player, boss), all spells are checked to
 * see if they are on timer, and if so their ongoing effects are
 * applied by the tick function.
 */

spell spells[] = {
	{
		"magic missle",
		can_magic_missle,
		cast_magic_missle,
		NULL
	},
	{
		"drain",
		can_drain,
		cast_drain,
		NULL
	},
	{
		"shield",
		can_shield,
		cast_shield,
		tick_shield
	},
	{
		"poison",
		can_poison,
		cast_poison,
		tick_poison
	},
	{
		"recharge",
		can_recharge,
		cast_recharge,
		tick_recharge
	},
};

/*
 * for spells on timer that have over time effects,
 * do the effects.
 */

void
tick_spells(battle_state *s) {
	for (int i = 0; i < SPELL_MAX; i++) {
		if (spells[i].do_tick)
			spells[i].do_tick(s);
	}
}

/*
 * magic missle and drain are instant effect spells, no ticks.
 */

/* magic missle costs 53 mana and instantly does 4 damage */
bool
can_magic_missle(battle_state *s) {
	return s->player_hp > 0 &&
	       s->player_mana >= 53;
}

void
cast_magic_missle(battle_state *s) {
	s->player_mana -= 53;
	s->mana_used += 53;
	s->boss_hp -= 4;
}

/* drain costs 73 mana, instantly does 2 points of damage and
 * heals you 2 points. */

bool
can_drain(battle_state *s) {
	return s->player_hp > 0 &&
	       s->player_mana >= 72;
}

void
cast_drain(battle_state *s) {
	s->player_mana -= 73;
	s->mana_used += 73;
	s->boss_hp -= 2;
	s->player_hp += 2;
}

/*
 * shield applies a fixed temporary armor buff for its duration.
 */

/* shield costs 113 mana and applies a limited duration temp armor
 * bonus of 7. */

bool
can_shield(battle_state *s) {
	return s->player_hp > 0 &&
	       s->player_mana >= 113 &&
	       s->spell_timers[SHIELD] == 0;
}

void
cast_shield(battle_state *s) {
	s->spell_timers[SHIELD] = 6;
	s->player_mana -= 113;
	s->mana_used += 113;
}

void
tick_shield(battle_state *s) {
	s->player_temp_armor = 0;
	if (s->spell_timers[SHIELD] == 0)
		return;
	s->spell_timers[SHIELD] -= 1;
	s->player_temp_armor = 7;
}

/*
 * poison applies a damage over time effect to the opponent.
 */

/* poison costs 173 mana and applies a damage over time effect
 * of 3 per turn lasting 6 turns. */

bool
can_poison(battle_state *s) {
	return s->player_hp > 0 &&
	       s->player_mana >= 173 &&
	       s->spell_timers[POISON] == 0;
}

void
cast_poison(battle_state *s) {
	s->spell_timers[POISON] = 6;
	s->player_mana -= 173;
	s->mana_used += 173;
}

void
tick_poison(battle_state *s) {
	if (s->spell_timers[POISON] == 0)
		return;
	s->spell_timers[POISON] -= 1;
	s->boss_hp -= 3;
}

/*
 * recharge applies a 'heal' over time effect to the player's
 * available mana pool.
 */

/* recharge costs 229 mana and regenerates 101 mana per turn for
 * 5 turns. */

bool
can_recharge(battle_state *s) {
	return s->player_hp > 0 &&
	       s->player_mana >= 229 &&
	       s->spell_timers[RECHARGE] == 0;
}

void
cast_recharge(battle_state *s) {
	s->spell_timers[RECHARGE] = 5;
	s->player_mana -= 229;
	s->mana_used += 229;
}

void
tick_recharge(battle_state *s) {
	if (s->spell_timers[RECHARGE] == 0)
		return;
	s->spell_timers[RECHARGE] -= 1;
	s->player_mana += 101;
}

/*
 * utilities for battle state:
 *
 * battle state records relevant stats for the participants and
 * a set of timers for available spells.
 */

void
print_state(battle_state *s) {
	printf("turn: %2d spell: %1d  player: %2d %3d %1d %4d  boss hp: %3d  spells: ",
	       s->turn, s->casting, s->player_hp, s->player_mana, s->player_temp_armor,
	       s->mana_used, s->boss_hp);
	for (int i = 0; i < SPELL_MAX; i++) {
		if (s->spell_timers[i] < 1)
			continue;
		printf("[%s %d]", spells[i].name, s->spell_timers[i]);
	}
	printf("\n");
}

/*
 * build the state structure.
 */

battle_state
create_battle_state(
        int player_hp,
        int player_mana,
        int boss_hp,
        int boss_damage,
        bool hard_mode
) {
	battle_state s;
	memset(&s, 0, sizeof(battle_state));
	s.player_hp = player_hp;
	s.player_mana = player_mana;
	s.boss_hp = boss_hp;
	s.boss_damage = boss_damage;
	s.hard_mode = hard_mode;
	for (int i = 0; i < SPELL_MAX; i++)
		s.spell_timers[i] = 0;
	return s;
}

int
battle_round(
        battle_state *s,
        int best_so_far
) {

	/* the player actually has the initiative, but the code wants
	 * the boss to come first in structure. */

	if (s->turn > 0) {
		tick_spells(s);
		if (s->boss_hp < 1)
			return min(s->mana_used, best_so_far);
		s->player_hp -= max(1, s->boss_damage - s->player_temp_armor);
		if (s->player_hp < 1)
			return best_so_far;
	}

	/* now the player goes ... */

	if (s->hard_mode)
		s->player_hp -= 1;
	if (s->player_hp < 1)
		return best_so_far;
	tick_spells(s);
	if (s->boss_hp < 1)
		return min(s->mana_used, best_so_far);

	/* prune this path if it's using too much mana */
	if (s->mana_used > best_so_far)
		return best_so_far;

	/* chase down each possible spell path recursively */

	for (int i = 0; i < SPELL_MAX; i++) {
		if (!spells[i].can_cast(s))
			continue;
		battle_state trial = *s;
		trial.turn += 1;
		trial.casting = i;
		spells[i].do_cast(&trial);

		/* boss dead, yay */
		if (trial.boss_hp < 1) {
			best_so_far = min(trial.mana_used, best_so_far);
			continue;
		}

		best_so_far = min(battle_round(&trial, best_so_far), best_so_far);
	}

	return best_so_far;
}

/*
 * battles are fought in turns and rounds. in a round, first the
 * player acts and then the boss. once either is dead, the battle
 * ends.
 */

int
battle(
        int player_hp,
        int player_mana,
        int boss_hp,
        int boss_damage,
        bool hard_mode
) {
	battle_state s = create_battle_state(player_hp, player_mana, boss_hp,
	                                     boss_damage, hard_mode);
	s.turn = 0;
	int result = battle_round(&s, INT_MAX);
	return result;
}

/*
 * if any globals need clearing between before a problem section
 * is run, do it here. there is nothing for day 22 of 2015.
 */
void
reset_all(void) {
	return;
}

/*
 * part one and part two: the input is so small that i've hard coded
 * it. the only difference between the parts required one boolean and
 * a conditional subtraction, so everythign is collapsed.:
 */

int
part_one(
        const char *fname
) {

	int r;
	r = battle(50, 500, 71, 10, false);
	printf("part one: %d\n", r);

	r = battle(50, 500, 71, 10, true);
	printf("part two: %d\n", r);

	return EXIT_SUCCESS;
}

int
part_two(
        const char *fname
) {
	return EXIT_SUCCESS;
}
