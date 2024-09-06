/* solution.h -- aoc 2016 01 -- troy brumley */

#ifndef SOLUTION_H
#define SOLUTION_H

#include <stdbool.h>

#define UNHASHED "abcdefgh"
#define HASHED   "fbgdceah"

void
swap_position(char *str, int n, int x, int y);

void
swap_letter(char *str, int n, char x, char y);

void
rotate_left(char *str, int n, int x);

void
rotate_right(char *str, int n, int x);

void
rotate_position(char *str, int n, char x);

void
unrotate_position(char *str, int n, char x);

void
move_position(char *str, int n, int x, int y);

void
reverse_positions(char *str, int n, int x, int y);

bool
processor(char *str, int n, char *cmd);

bool
unprocessor(char *str, int n, char *cmd);

#define INPUT_LINE_MAX 4096

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
