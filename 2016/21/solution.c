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
	printf("\n%s  <--  %s%s", password, ok ? "":"ERROR:", cmd);
	free_split(tokens);
	return ok;
}

/*
 * reverse or undo a legal scrambler command.
 */

bool
undoer(char *password, int n, char *cmd) {

	/* the command to reverse *cmd, or undo, is overallocated so i can
	 * skip checking later. it is then preset with an error message for
	 * debugging tracking.
	 *
	 * all successfully created undo commands will overwrite this error
	 * message. 'hole in the bucket' paths will leave it as is and an
	 * error will be reported. */

	char *undo = malloc(strlen(cmd) * 3);
	memset(undo, 0, strlen(cmd) * 3);
	strcpy(undo, "YOU DIDN\'T REVERSE ");
	strcat(undo, cmd);

	const char **tokens = split_string(cmd, " \n");

	if (equal_string(tokens[1], "swap")) {
		/* swap position 2 with position 7
		 * swap letter h with letter e
		 * both are undone simply by reapplying */
		strcpy(undo, cmd);
	} else if (equal_string(tokens[1], "reverse")) {
		/* reverse positions 0 through 1
		 * reverse positions 4 through 7
		 * undone by simply reapplying */
		strcpy(undo, cmd);
	} else if (equal_string(tokens[1], "move")) {
		/* move position 6 to position 4
		 * undone by switching from and to */
		snprintf(undo, strlen(cmd) * 3 - 1, "%s %s %s %s %s %s\n",
		         tokens[1], tokens[2], tokens[6], tokens[4], tokens[5], tokens[3]);
	} else if (equal_string(tokens[1], "rotate")) {
		/* rotate right 7 steps
		 * rotate left 0 steps
		 * right and left should just be switching direction */
		if (equal_string(tokens[2], "right")) {
			snprintf(undo, strlen(cmd) * 3 - 1, "%s %s %s %s\n",
			         tokens[1], "left", tokens[3], tokens[4]);
		} else if (equal_string(tokens[2], "left")) {
			snprintf(undo, strlen(cmd) * 3 - 1, "%s %s %s %s\n",
			         tokens[1], "right", tokens[3], tokens[4]);
		} else if (equal_string(tokens[2], "based")) {
			/* TODO none of these work reliably even for people with
			 * the same passwords to hash or unhash. so they are all
			 * somewhat input dependent. brute force may be the answer
			 * and i still have to try that. ... in the queue.
			 * this one is frustrating ... */
			/* rotate based on position of letter f
			 * more thought needed for based on position */
			/* it appears that the resulting position of the letter
			 * can be used to find a proper left shift to undo the
			 * right shift of the rotate based on position. this
			 * is hard coded for the input of an eight character
			 * (unique) string.
			 * [7, 7, 2, 6, 1, 5, 0, 4]
			 *  1  1  5  2  7  3  0  4
			 * {0: 7, 1: 0, 2: 4, 3: 1, 4: 5, 5: 2, 6: 6, 7: 3}
			 0 -> 1
			1 -> 2
			2 -> 3
			3 -> 4
			4 -> 6
			5 -> 7
			6 -> 0
			7 -> 1
			 */
			int xlate_rot[] = { 1, 2, 3, 4, 6, 7, 0, 1 };
			/*    { 0, 1 } */
			/*    { 1, 1 }, */
			/*    { 2, 5 }, */
			/*    { 3, 2 }, */
			/*    { 4, 7 }, */
			/*    { 5, 3 }, */
			/*    { 6, 0 }, */
			/*    { 7, 4 }, */
			/* }; */
			int pos = pos_char(password, 0, tokens[7][0]);
			int left = xlate_rot[pos];
			/* for (int i = 0; i < 8; i++) { */
			/*    if (pos == xlate_rot[i][0]) { */
			/*       left = xlate_rot[i][1]; */
			/*       break; */
			/*    } */
			/* } */
			snprintf(undo, strlen(cmd) * 3 - 1, "rotate left %d steps", left);
		} else {
			/* input in error, but we'll let it fall through. */
		}
	} else {
		/* input in error but we'll let it fall through */
	}

	free_split(tokens);
	printf("reversing --> original: %s                   new: %s", cmd, undo);
	bool ok = processor(password, n, undo);
	free(undo);
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
	int num_commands = 0;
	while (fgets(iline, INPUT_LINE_MAX - 1, ifile)) {
		commands[num_commands] = dup_string(iline);
		num_commands += 1;
	}
	for (int i = 0; i < num_commands; i++) {
		if (!processor(password, n, commands[i])) {
			failed = true;
			fprintf(stderr, "not understood: %s\n", iline);
			break;
		}
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
		num_commands += 1;
	}
	for (int i = num_commands - 1; i >= 0; i--) {
		if (!undoer(password, n, commands[i])) {
			failed = true;
			fprintf(stderr, "not understood: %s\n", iline);
			break;
		}
	}
	printf("part two: %s\n", password);

	free(password);
	for (int i = 0; i < num_commands; i++)
		free(commands[i]);
	free(commands);
	fclose(ifile);
	return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
