/* solution.h -- aoc 2016 08 -- troy brumley */

#ifndef SOLUTION_H
#define SOLUTION_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define INPUT_LINE_MAX 4096

/*
 * for day 8 a mini display (6 rows, 50 columns) is driven by an
 * obtuse programming language.
 *
 * rect AxB turns on all of the pixels in a rectangle at the top-left
 * of the screen which is A wide and B tall.
 *
 * rotate row y=A by B shifts all of the pixels in row A (0 is the
 * top row) right by B pixels. Pixels that would fall off the right
 * end appear at the left end of the row.
 *
 * rotate column x=A by B shifts all of the pixels in column A (0 is
 * the left column) down by B pixels. Pixels that would fall off the
 * bottom appear at the top of the column.
 */

/*
 * the display and its api
 */

#define DISPLAY_TAG "__DISP__"
#define ROW_MAX 6
#define COLUMN_MAX 50
#define ON 1
#define OFF 0

typedef struct display display;
struct display {
	char tag[8];
	uint8_t pixels[ROW_MAX][COLUMN_MAX];
};

display *
create_display(
        void
);

void
clear_display(
        display *d
);

uint8_t
get_pixel(
        display *d,
        int row,
        int col
);

void
show_display(
        display *d,
        FILE *f
);

int
count_lit_pixels(
        display *d
);

void
destroy_display(
        display *d
);

/*
 * language statements, compilation, program storage
 */

#define PROGRAM_TAG "__PROG__"
#define INSTRUCTION_MAX 200

typedef struct instruction instruction;

typedef void (fninstruction)(display *, uint8_t, uint8_t);

struct instruction {
	fninstruction *fn;
	uint8_t a;
	uint8_t b;
};

typedef struct program program;

struct program {
	char tag[8];
	FILE *trace;
	int ip;
	instruction instructions[INSTRUCTION_MAX + 1];
};

program *
create_program(
        void
);

bool
compile_instruction(
        instruction *i,
        const char *source
);

void
show_instruction(
        instruction *i,
        FILE *f
);

bool
load_program(
        program *p,
        FILE *f
);

bool
run_program(
        program *p,
        display *d
);

void
set_program_trace(
        FILE *f
);

void
destroy_program(
        program *p
);

/*
 * implementation of the instructions
 */

void
do_rect(
        display *d,
        uint8_t a,
        uint8_t b);

void
do_rotate_row(
        display *d,
        uint8_t a,
        uint8_t b
);

void
do_rotate_column(
        display *d,
        uint8_t a,
        uint8_t b
);

/*
 * the parts
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
