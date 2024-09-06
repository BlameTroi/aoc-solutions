/* solution.c -- aoc 2016 21 -- troy brumley */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

#define TXBSTR_IMPLEMENTATION
#include "txbstr.h"

/*
 * swap position X with position Y means that the letters at indexes X
 * and Y (counting from 0) should be swapped.
 */

void
swap_position(char *str, int n, int x, int y) {
	char swap = str[x];
	str[x] = str[y];
	str[y] = swap;
}

/*
 * swap letter X with letter Y means that the letters X and Y should
 * be swapped (regardless of where they appear in the string).
 *
 * possible bug: i'm not checking for duplicates.
 */

void
swap_letter(char *str, int n, char x, char y) {
	int px = pos_char(str, 0, x);
	int py = pos_char(str, 0, y);
	if (px >= 0 && py >= 0 && px != py)
		swap_position(str, n, px, py);
}

/*
 * rotate left/right X steps means that the whole string should be
 * rotated; for example, one right rotation would turn abcd into dabc.
 */

void
rotate_left(char *str, int n, int x) {
	if (x == 0)
		return;
	if (x < 0) {
		rotate_right(str, n, -x);
		return;
	}
	while (x > 0) {
		char swap = str[0];
		memmove(str, str+1, n-1);
		str[n-1] = swap;
		x -= 1;
	}
}

void
rotate_right(char *str, int n, int x) {
	if (x == 0)
		return;
	if (x < 0) {
		rotate_left(str, n, -x);
		return;
	}
	while (x > 0) {
		char swap = str[n - 1];
		for (int i = n-1; i > 0; i--)
			str[i] = str[i-1];
		str[0] = swap;
		x -= 1;
	}
}

/*
 * rotate based on position of letter X means that the whole string
 * should be rotated to the right based on the index of letter X
 * (counting from 0) as determined before this instruction does any
 * rotations. Once the index is determined, rotate the string to the
 * right one time, plus a number of times equal to that index, plus
 * one additional time if the index was at least 4.
 */

void
rotate_position(char *str, int n, char x) {
	int p = pos_char(str, 0, x);
	if (p < 0)
		return;
	int rot = 1 + p;
	if (p >= 4)
		rot += 1;
	rotate_right(str, n, rot);
}


/*
 * move position X to position Y means that the letter which is at
 * index X should be removed from the string, then inserted such that
 * it ends up at index Y.
 */

void
move_position(char *str, int n, int x, int y) {
	char *temp = malloc(n+1);
	memset(temp, 0, n+1);
	char hold = str[x];
	int ip = 0;
	int op = 0;
	while (true) {
		if (ip == x)
			ip += 1;
		temp[op] = str[ip];
		if (str[ip] == '\0')
			break;
		ip += 1;
		op += 1;
	}
	ip = 0;
	op = 0;
	while (true) {
		if (op == y) {
			str[op] = hold;
			op += 1;
		}
		str[op] = temp[ip];
		if (temp[ip] == '\0')
			break;
		ip += 1;
		op += 1;
	}
	free(temp);
}


/*
 * reverse positions X through Y means that the span of letters at
 * indexes X through Y (including the letters at X and Y) should be
 * reversed in order.
 */

void
reverse_positions(char *str, int n, int x, int y) {
	if (x == y)
		return;
	char swap;
	char *px;
	char *py;
	if (x < y) {
		px = str + x;
		py = str + y;
	} else {
		px = str + y;
		py = str + x;
	}
	while (px < py) {
		swap = *px;
		*px = *py;
		*py = swap;
		px += 1;
		py -= 1;
	}
}

/*
 * given a legal scrambler command, run it.
 */

bool
processor(char *password, int n, char *cmd) {
	int x, y;
	const char **tokens = split_string(cmd, " \n");
	bool ok = true;
	if (equal_string(tokens[1], "swap")) {
		/* swap position 2 with position 7
		 * swap letter h with letter e */
		if (equal_string(tokens[2], "position")) {
			x = strtol(tokens[3], NULL, 10);
			y = strtol(tokens[6], NULL, 10);
			swap_position(password, n, x, y);
		} else if (equal_string(tokens[2], "letter")) {
			x = tokens[3][0];
			y = tokens[6][0];
			swap_letter(password, n, x, y);
		} else
			ok = false;
	} else if (equal_string(tokens[1], "rotate")) {
		/* rotate right 7 steps
		 * rotate left 0 steps
		 * rotate based on position of letter f */
		if (equal_string(tokens[2], "left")) {
			x = strtol(tokens[3], NULL, 10);
			rotate_left(password, n, x);
		} else if (equal_string(tokens[2], "right")) {
			x = strtol(tokens[3], NULL, 10);
			rotate_right(password, n, x);
		} else if (equal_string(tokens[2], "based")) {
			x = tokens[7][0];
			rotate_position(password, n, x);
		} else
			ok = false;
	} else if (equal_string(tokens[1], "reverse")) {
		/* reverse positions 0 through 1
		 * reverse positions 4 through 7 */
		x = strtol(tokens[3], NULL, 10);
		y = strtol(tokens[5], NULL, 10);
		reverse_positions(password, n, x, y);
	} else if (equal_string(tokens[1], "move")) {
		/* move position 6 to position 4 */
		x = strtol(tokens[3], NULL, 10);
		y = strtol(tokens[6], NULL, 10);
		move_position(password, n, x, y);
	} else
		ok = false;
	/* printf("\n%s  <--  %s%s", password, ok ? "":"ERROR:", cmd); */
	free_split(tokens);
	return ok;
}

/*
 * given a legal scrambler command, undo it. some can be undone by
 * reversing parameters or direction, others by just doing the same
 * command again. the only complexity was "rotate based", which
 * shouldn't have been as difficult as it was.
 *
 * unrotate based on position of letter X means that the whole string
 * should be rotated to the right based on the index of letter X
 * (counting from 0). eyeballed the differences and that leads to
 * a simple switch.
 *
 * case 1, character was last character before rotate and is now
 * the first character, rotate left by p + 1 and we got it.
 *
 * case 2, character was first character and is now second character,
 * rotate left by exactly 1.
 *
 * case 3, character was fifth character and is now third character,
 * rotate right by 2.
 *
 * case 4, character was second character and is now fourth character,
 * rotate left by 2.
 *
 * case 5, character was sixth character and is now fifth character,
 * rotate right by 1.
 *
 * case 6, character was third character, now is sixth character.
 * rotate left by 3.
 *
 * case 7. character was seventh character, now is eight character.
 * no change.
 *
 * case 8. character was fourth character, now is eighth character.
 * rotate left by 4.
 */

bool
unprocessor(char *password, int n, char *cmd) {
	int x, y;
	const char **tokens = split_string(cmd, " \n");
	bool ok = true;
	if (equal_string(tokens[1], "swap")) {
		/* swap position 2 with position 7
		 * swap letter h with letter e */
		if (equal_string(tokens[2], "position")) {
			x = strtol(tokens[3], NULL, 10);
			y = strtol(tokens[6], NULL, 10);
			swap_position(password, n, x, y);
		} else if (equal_string(tokens[2], "letter")) {
			x = tokens[3][0];
			y = tokens[6][0];
			swap_letter(password, n, x, y);
		} else
			ok = false;
	} else if (equal_string(tokens[1], "rotate")) {
		/* rotate right 7 steps
		 * rotate left 0 steps
		 * rotate based on position of letter f */
		if (equal_string(tokens[2], "left")) {
			x = strtol(tokens[3], NULL, 10);
			rotate_right(password, n, x);
		} else if (equal_string(tokens[2], "right")) {
			x = strtol(tokens[3], NULL, 10);
			rotate_left(password, n, x);
		} else if (equal_string(tokens[2], "based")) {
			x = tokens[7][0];
			switch (pos_char(password, 0, x)) {
			case 0: rotate_left(password, n, 1); break;
			case 1: rotate_left(password, n, 1); break;
			case 2: rotate_right(password, n, 2); break;
			case 3: rotate_left(password, n, 2); break;
			case 4: rotate_right(password, n, 1); break;
			case 5: rotate_left(password, n, 3); break;
			case 6: /* nothing to do */ break;
			case 7: rotate_left(password, n, 4); break;
			default: /* this is an error, ignored */ break;
			}
		} else
			ok = false;
	} else if (equal_string(tokens[1], "reverse")) {
		/* reverse positions 0 through 1
		 * reverse positions 4 through 7 */
		x = strtol(tokens[3], NULL, 10);
		y = strtol(tokens[5], NULL, 10);
		reverse_positions(password, n, x, y);
	} else if (equal_string(tokens[1], "move")) {
		/* move position 6 to position 4
		 * undone by switching from and to */
		x = strtol(tokens[6], NULL, 10);
		y = strtol(tokens[3], NULL, 10);
		move_position(password, n, x, y);
	} else
		ok = false;
	return ok;
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

	char iline[INPUT_LINE_MAX];

	bool failed = false;
	char *password = dup_string(UNHASHED);
	int n = strlen(password);
	printf("%s\n", password);
	char **commands = malloc(200 * sizeof(char *));
	memset(commands, 0, 200 * sizeof(char *));
	int num_commands = 0; /* abcn */
	while (fgets(iline, INPUT_LINE_MAX - 1, ifile)) {
		commands[num_commands] = dup_string(iline);
		commands[num_commands][strlen(iline)-1] = '\0';
		num_commands += 1;
	}
	for (int i = 0; i < num_commands; i++) {
		/* printf("%3d  %s->", i+1, password); */
		if (!processor(password, n, commands[i])) {
			failed = true;
			fprintf(stderr, "not understood: %s\n", iline);
			break;
		}
		/* printf("%s  %s\n", password, commands[i]); */
	}
	printf("part one: %s\n", password);

	free(password);
	fclose(ifile);
	return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}


/*
 * part two: decrypt a different password.
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

	bool failed = false;
	char *password = dup_string(HASHED);
	int n = strlen(password);
	printf("%s\n", password);
	char **commands = malloc(200 * sizeof(char *));
	memset(commands, 0, 200 * sizeof(char *));
	int num_commands = 0;
	while (fgets(iline, INPUT_LINE_MAX - 1, ifile)) {
		commands[num_commands] = dup_string(iline);
		commands[num_commands][strlen(iline)-1] = '\0';
		num_commands += 1;
	}

	/* apply the reverse of the command in reverse order */
	for (int i = num_commands - 1; i >= 0; i--) {
		/* printf("%3d  %s->", i+1, password); */
		if (!unprocessor(password, n, commands[i])) {
			failed = true;
			fprintf(stderr, "not understood: %s\n", iline);
			break;
		}
		/* printf("%s  %s\n", password, commands[i]); */
	}

	printf("part two: %s\n", password);

	free(password);
	for (int i = 0; i < num_commands; i++)
		free(commands[i]);
	free(commands);
	fclose(ifile);
	return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
