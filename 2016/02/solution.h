/* solution.h -- aoc 2016 01 -- troy brumley */

#ifndef SOLUTION_H
#define SOLUTION_H

#define UP 'U'
#define RIGHT 'R'
#define DOWN 'D'
#define LEFT 'L'

#define EMPTY ' '

#define RESULT_MAX 15

/*
 * row column from upper left on keypad.
 */

typedef struct keypad_pos keypad_pos;

struct keypad_pos {
	int row;
	int col;
};

/*
 * describe the keypad geometry, and how to read
 * a key from it.
 */

typedef struct keypad_def keypad_def;

struct keypad_def {
	int rows;
	int cols;
	keypad_pos start_key;
	char (*fnreader)(int, int);
};

void
move_key(keypad_pos *pos, keypad_def def, char move);

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
