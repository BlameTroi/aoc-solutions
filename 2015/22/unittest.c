/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "minunit.h"

#include "solution.h"

#define TXBPQ_IMPLEMENTATION
#include "txbpq.h"

/*
 * notes:
 *
 * a hit is opp hp - max(1, agg dmg - opp armor)
 * player is first aggressor
 *
 */


FILE *testdata = NULL;

/*
 * minunit setup and teardown of infrastructure.
 */

void
test_setup(void)
{
	reset_all();
	testdata = fopen("unitdata.txt", "r");
	if (!testdata)
		mu_fail("could not open test data file!");
}

void
test_teardown(void)
{
	fclose(testdata);
}


MU_TEST(test_battle_state)
{
	battle_state s = create_battle_state(10, 250, 13, 8, false);
	mu_should(s.player_hp == 10);
	mu_should(s.boss_damage == 8);
}

MU_TEST(test_sample_battle)
{
	/* for example, suppose the player has 10 hit points and 250 mana, and that the boss has 13 hit points and 8 damage: */

	battle_state init = create_battle_state(10, 250, 13, 8, false);

	/*
	-- Player turn --
	- Player has 10 hit points, 0 armor, 250 mana
	- Boss has 13 hit points
	Player casts Poison.  */
	tick_spells(&init);
	mu_should(can_poison(&init));
	cast_poison(&init);
	mu_should(init.player_mana == 77);
	mu_should(init.mana_used == 173);
	mu_should(init.boss_hp == 13);

	/*
	-- Boss turn --
	- Player has 10 hit points, 0 armor, 77 mana
	- Boss has 13 hit points
	Poison deals 3 damage; its timer is now 5.
	Boss attacks for 8 damage. */
	tick_spells(&init);
	mu_shouldnt(can_poison(&init));
	mu_should(init.boss_hp == 10);
	init.player_hp -= 8;
	mu_should(init.player_hp = 2);

	/*
	-- Player turn --
	- Player has 2 hit points, 0 armor, 77 mana
	- Boss has 10 hit points
	Poison deals 3 damage; its timer is now 4.
	Player casts Magic Missile, dealing 4 damage. */

	mu_should(init.player_hp == 2 && init.player_mana == 77 && init.player_temp_armor == 0);
	tick_spells(&init);
	mu_should(init.spell_timers[POISON] == 4);
	mu_should(init.boss_hp == 7);
	mu_should(can_magic_missle(&init));
	cast_magic_missle(&init);
	mu_should(init.boss_hp == 3);
	mu_should(init.player_mana == 24);

	/*
	-- Boss turn --
	- Player has 2 hit points, 0 armor, 24 mana
	- Boss has 3 hit points
	Poison deals 3 damage. This kills the boss, and the player wins. */
	mu_should(init.player_hp == 2 && init.player_mana == 24);
	tick_spells(&init);
	mu_should(init.boss_hp < 1);
	mu_should(init.mana_used == 226);
	mu_should(init.player_hp > 0);
}

MU_TEST(test_sample_battle_2)
{
	/* for example, suppose the player has 10 hit points and 250 mana, and that the boss has 14 hit points and 8 damage: */

	battle_state init = create_battle_state(10, 250, 14, 8, false);

	/*
	  -- Player turn --
	- Player has 10 hit points, 0 armor, 250 mana
	- Boss has 14 hit points
	Player casts Recharge. */
	tick_spells(&init);
	mu_should(can_recharge(&init));
	cast_recharge(&init);
	mu_should(init.player_mana == 21);
	mu_should(init.mana_used == 229);
	mu_should(init.boss_hp == 14);

	/*
	-- Boss turn --
	- Player has 10 hit points, 0 armor, 21 mana
	- Boss has 14 hit points
	Recharge provides 101 mana; its timer is now 4.
	Boss attacks for 8 damage! */
	tick_spells(&init);
	mu_shouldnt(can_recharge(&init));
	mu_should(init.boss_hp == 14);
	mu_should(init.player_mana == 122);
	mu_should(init.spell_timers[RECHARGE] == 4);
	init.player_hp -= 8;
	mu_should(init.player_hp == 2);

	/*
	-- Player turn --
	- Player has 2 hit points, 0 armor, 122 mana
	- Boss has 14 hit points
	Recharge provides 101 mana; its timer is now 3.
	Player casts Shield, increasing armor by 7. */
	tick_spells(&init);
	mu_shouldnt(can_recharge(&init));
	mu_should(can_shield(&init));
	mu_should(init.player_mana == 223);
	mu_should(init.spell_timers[RECHARGE] == 3);
	cast_shield(&init);

	/*
	-- Boss turn --
	- Player has 2 hit points, 7 armor, 110 mana
	- Boss has 14 hit points
	Shield's timer is now 5.
	Recharge provides 101 mana; its timer is now 2.
	Boss attacks for 8 - 7 = 1 damage! */
	mu_should(init.player_mana == 110);
	tick_spells(&init);
	mu_should(init.boss_hp == 14);
	mu_should(init.player_hp == 2);
	mu_should(init.player_temp_armor == 7);
	mu_should(init.player_mana == 211);
	mu_should(init.spell_timers[RECHARGE] == 2);
	mu_should(init.spell_timers[SHIELD] == 5);
	init.player_hp -= (8 - init.player_temp_armor);
	mu_should(init.player_hp == 1);

}

MU_TEST(test_battle_data_1)
{

	int ans = battle(10, 250, 13, 8, false);
	printf("%d\n", ans);
	mu_should(ans == 226);
}

MU_TEST(test_battle_data_live)
{
	int ans = battle(50, 500, 71, 10, false);
	printf("%d\n", ans);
	mu_should(ans > 1500 && ans < 3000);
}

MU_TEST(test_battle_data_live_hard)
{
	int ans = battle(50, 500, 71, 10, true);
	printf("%d\n", ans);
	mu_should(ans > 1500 && ans < 3000);
}

/*
 * here we define the whole test suite. sadly there's no runtime
 * introspection. there is probably an opportunity for an elisp helper
 * to create the suite in the editor, but for now it's just a matter
 * of doing it manually.
 */
MU_TEST_SUITE(test_suite)
{

	/* always have a setup and teardown, even if they */
	/* do nothing. */

	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	/* run your tests here */
	MU_RUN_TEST(test_battle_state);
	MU_RUN_TEST(test_sample_battle);
	MU_RUN_TEST(test_sample_battle_2);
	MU_RUN_TEST(test_battle_data_1);
	MU_RUN_TEST(test_battle_data_live);
	MU_RUN_TEST(test_battle_data_live_hard);
}


/*
 * master control:
 */

int
main(int argc, char *argv[])
{
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
