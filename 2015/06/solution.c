/* solution.c -- aoc 2015 06 -- troy brumley */

/*
 * probably a fire hazard
 *
 * lights in a 0,0 -> 999,999 grid are turned off and on as directed,
 * how many lights are left lit at the end of the run if all the
 * lights are off at the start.
 *
 * (turn on|turn off|toggle) x0,y0 through x1,y1
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

/*
 * the day's real code.
 */

/* part one: */
int
part_one(
        const char *fname
) {
	FILE *ifile;

	ifile = fopen(fname, "r");
	if (!ifile) {
		printf("could not open file: %s\n", fname);
		return EXIT_FAILURE;
	}

	/* line from input */
	char *iline;
	/* length, expected to have a trailing \n */
	size_t ilen;

	lights *g = init_grid(1);

	while ((iline = fgetln(ifile, &ilen))) {
		cmd c = parse_cmd(iline, ilen);
		do_cmd(g, c);
	}

	printf("part one: %ld\n", g->lit);

	fclose(ifile);
	return EXIT_SUCCESS;
}

/* part two: */
int
part_two(
        const char *fname
) {
	FILE *ifile;

	ifile = fopen(fname, "r");
	if (!ifile) {
		printf("could not open file: %s\n", fname);
		return EXIT_FAILURE;
	}

	/* line from input */
	char *iline;
	/* length, expected to have a trailing \n */
	size_t ilen;

	lights *g = init_grid(0);

	while ((iline = fgetln(ifile, &ilen))) {
		cmd c = parse_cmd(iline, ilen);
		do_cmd(g, c);
	}

	printf("part one: %ld\n", g->intensity);

	fclose(ifile);
	return EXIT_SUCCESS;
}


/* create a grid of lights: */
lights *
init_grid(
        bool digital
) {
	lights *g = malloc(sizeof(lights));
	memset(g, 0, sizeof(lights));
	assert(g);
	if (digital) {
		g->digital = 1;
		g->fnon = turn_on_d;
		g->fnoff = turn_off_d;
		g->fntog = toggle_d;
	} else {
		g->digital = 0;
		g->fnon = turn_on_a;
		g->fnoff = turn_off_a;
		g->fntog = toggle_a;
	}
	return g;
}

/* release the grid of lights: */
void
free_grid(
        lights *g
) {
	free(g);
}

/*
 * parse the input command:
 *
 * (turn on|turn off|toggle) x0,y0 through x1,y1
 *
 * if the cmd_t.cmd is invalid, some error was found and
 * the block is not usable. more in depth diagnostics are
 * not needed for advent of code type work.
 */

cmd
parse_cmd(
        const char *iline,
        int len
) {

	/* fill the shell */
	cmd cmd;                              /* our answer */
	cmd_e hold = e_invalid;               /* the command, plugged in at end */
	cmd.cmd = e_invalid;                  /* build invalid shell */
	cmd.p0.x = 0;
	cmd.p0.y = 0;
	cmd.p1.x = 0;
	cmd.p1.y = 0;

	/* identify command */
	const char *pos = NULL;             /* start position in string for scans */
	char *nxt = NULL;                   /* end pos of scan */
	const char *ton = "turn on ";       /* command prefixes */
	const char *toff = "turn off ";
	const char *togg = "toggle ";
	const char *tthr = "through ";      /* delimiter between coordinates */

	/* check prefix and determine command type. if at any time we have
	   an error, just return with the cmd code set to invalid. */

	/* length of prefix determins the start of the first coordinate pair. */
	if (strncmp(iline, ton, strlen(ton)) == 0) {
		pos = iline+strlen(ton);
		hold = e_on;
	} else if (strncmp(iline, toff, strlen(toff)) == 0) {
		pos = iline+strlen(toff);
		hold = e_off;
	} else if (strncmp(iline, togg, strlen(togg)) == 0) {
		pos = iline+strlen(togg);
		hold = e_toggle;
	} else {
		/* not recognized */
		return cmd;
	}

	/* get the coordinates, exit early with an invalid block if any parse
	   errors occur. */

	/* first corner */
	cmd.p0.x = strtol(pos, &nxt, 10);
	if (pos == nxt || *nxt != ',')
		return cmd;
	pos = nxt + 1;
	cmd.p0.y = strtol(pos, &nxt, 10);
	if (pos == nxt || *nxt != ' ')
		return cmd;
	pos = nxt + 1;

	/* delimiter between coordinate pairs is " through ", return invalid
	   if it isn't found. */
	if (strncmp(pos, tthr, strlen(tthr)) != 0)
		return cmd;

	/* second corner */
	pos = pos + strlen(tthr);
	cmd.p1.x = strtol(pos, &nxt, 10);
	if (pos == nxt || *nxt != ',')
		return cmd;
	pos = nxt + 1;
	cmd.p1.y = strtol(pos, &nxt, 10);
	if (pos == nxt || *nxt != '\n')
		return cmd;

	/* all fields processed, mark command as valid with the */
	/* command code and return. */
	cmd.cmd = hold;
	return cmd;
}

/*
 * how many lights are on?
 */
long
number_on(lights *g) {
	return g->lit;
}

/*
 * what is their total intensity if analog mode?
 */
long
total_intensity(lights *g) {
	return g->intensity;
}

/*
 * turn single light on, off, or toggle it.
 */
void
turn_on_d(lights *g, coord p) {
	if (is_lit(g, p))
		return;
	g->bulb[p.x][p.y] = 1;
	g->lit += 1;
}

void
turn_off_d(lights *g, coord p) {
	if (!is_lit(g, p))
		return;
	g->bulb[p.x][p.y] = 0;
	g->lit -= 1;
}

void
toggle_d(lights *g, coord p) {
	is_lit(g, p) ? g->fnoff(g, p) : g->fnon(g, p);
}


/*
 * turn single light on, off, or toggle it.
 *
 * in analog mode, on = +1 to brightness
 *                 off = -1 to brightness, but can not go below 0.
 *                 toggle = +2 to brightness
 *
 * does not yet track total count of lights actually lit.
 */
void
turn_on_a(lights *g, coord p) {
	g->bulb[p.x][p.y] += 1;
	g->intensity += 1;
}

void
turn_off_a(lights *g, coord p) {
	if (g->bulb[p.x][p.y] == 0)
		return;
	g->bulb[p.x][p.y] -= 1;
	g->intensity -= 1;
}

void
toggle_a(lights *g, coord p) {
	g->bulb[p.x][p.y] += 2;
	g->intensity += 2;
}


/*
 * single light's status.
 */

bool
is_lit(lights *g, coord p) {
	return g->bulb[p.x][p.y];
}



/*
 * by your command...
 */
int
min(int a, int b) {
	return a < b ? a : b;
}

void
do_cmd(lights *g, cmd c) {

	/* get light indices in proper ordering */
	int x1 = min(c.p0.x, c.p1.x);
	int x2 = x1 == c.p0.x ? c.p1.x : c.p0.x;
	int y1 = min(c.p0.y, c.p1.y);
	int y2 = y1 == c.p0.y ? c.p1.y : c.p0.y;

	/* get function for command */
	void (*fn)(lights *, coord) = NULL;
	switch (c.cmd) {
	case e_on:
		fn = g->fnon;
		break;
	case e_off:
		fn = g->fnoff;
		break;
	case e_toggle:
		fn = g->fntog;
		break;
	case e_invalid:
		/* does nothing, should not occur, will get a segmentation fault
		   if we get this far and issue the call, but we should never ever
		   execute this. */
		break;
	}

	/* do whatever we need to */
	int i, j;
	for (i = x1; i <= x2; i++) {
		coord p = {i, -1};
		for (j = y1; j <= y2; j++) {
			p.y = j;
			(*fn)(g, p);
		}
	}
}
