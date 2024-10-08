/* solution.c -- aoc 2015 19 -- troy brumley */

#include <assert.h>
#include <limits.h>
#include <time.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TXBMISC_IMPLEMENTATION
#include "txbmisc.h"
#define TXBSTR_IMPLEMENTATION
#include "txbstr.h"
#define TXBKL_IMPLEMENTATION
#include "txbkl.h"
#define TXBRAND_IMPLEMENTATION
#include "txbrand.h"

#include "solution.h"

/*
 * output transformations stored as an array of string pointers. grows
 * dynamically by doubling in size.
 */

int num_transforms;
transform *transforms[TRANSFORM_MAX];

char base[INPUT_LEN_MAX];

int num_run = 0;
klcb *unique_transforms = NULL;

/*
 * the usual utility functions
 */

static
int
key_compare(
	void *s1,
	void *s2
) {
	return strcmp(s1, s2);
}


void
reset_state(bool rel) {

	if (rel) {
		if (unique_transforms != NULL) {
			void *key;
			void *value;
			/* TODO leaks */
			while (kl_get_first(unique_transforms, &key, &value))
				kl_delete(unique_transforms, key);
			kl_destroy(unique_transforms);
			unique_transforms = NULL;
		}
		for (int i = 0; i < TRANSFORM_MAX; i++) {
			if (transforms[i])
				free(transforms[i]);
		}
	}

	unique_transforms = kl_create(key_compare);

	num_transforms = 0;
	memset(transforms, 0, sizeof(transforms));
	memset(base, 0, sizeof(base));
	num_run = 0;
}

/*
 * our input is a block of transformation rules (from => to), a blank
 * line, and the base text to run the rules against. if this line does
 * not match the pattern for a rule, ignore it and return 0.
 * otherwise, parse it, add the rule, and return 1.
 *
 * bug: assumes the rule marker text will not be found anywhere but
 *      rule defintions.
 */

bool
parse_line(
	const char *iline
) {

	/* do we have a rule marker? */
	char *c = strchr(iline, '=');
	if (c == NULL || *(c+1) != '>')
		return false;

	/* build the rule in the next available slot */
	assert(num_transforms < TRANSFORM_MAX);
	const char **t = split_string(iline, " \n");
	transform *p = malloc(sizeof(transform));
	memset(p, 0, sizeof(transform));
	transforms[num_transforms] = p;
	strncpy(p->from, t[1], sizeof(p->from));
	p->from_len = strlen(t[1]);
	strncpy(p->to, t[3], sizeof(p->to));
	p->to_len = strlen(t[3]);
	num_transforms += 1;

	free_split(t);

	return true;
}

/*
 * transform one rule one time in a string. updates *pos to point to
 * next start point.
 *
 * t -> a transform block
 * s -> a string
 * pos -> how far into s to start scanning for transform from value
 *
 * returns NULL if no transform possible. value of pos updated.
 */

char *
transformer(
	transform *t,
	const char *s,
	int *pos
) {

	if (t == NULL || s == NULL)
		return NULL;

	int slen = strlen(s);

	/* if we can do a transformation, this is the length of
	 * the buffer we'll need for the output string. */

	size_t max_transform_len = strlen(s) + 1 /* NUL */ + t->to_len - t->from_len;

	/* start scanning for the from string, memcmp was easier */

	int p = *pos;
	while (p + t->from_len <= slen) {
		if (memcmp(s + p, t->from, t->from_len) == 0) {
			/* and we found a starting point */
			char *res = malloc(max_transform_len);
			memset(res, 0, max_transform_len);
			memcpy(res, s, p);
			memcpy(res + p, t->to, t->to_len);
			strcpy(res + p + t->to_len, s + p + t->from_len);
			*pos = p + 1;
			return res;
		}
		p += 1;
	}

	/* no from string found, so nothing returned */
	return NULL;
}

/*
 * for part two we want to work in reverse from the ending 'base'
 * string back to 'e'. flip the from and to of the transformation
 * rules.
 */

static
void
invert(
	transform *t
) {

	char swap_text[TO_MAX];
	int swap_len;
	memcpy(swap_text, t->to, sizeof(swap_text));
	memcpy(t->to, t->from, sizeof(t->to));
	memcpy(t->from, swap_text, sizeof(t->from));
	swap_len = t->to_len;
	t->to_len = t->from_len;
	t->from_len = swap_len;
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
		printf("could not open file: %s\n", fname);
		return EXIT_FAILURE;
	}

	char iline[INPUT_LEN_MAX];
	reset_state(false);

	while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {

		/* input is ordered transform rules, blank line, and the base
		 * text. if the line fails parsing, it is either blank or the
		 * base text. parsing it requires enough code to hide
		 * elsewhere. */

		if (parse_line(iline) == 0) {
			if (strlen(iline) > 1) {
				strncpy(base, iline, min(strlen(iline) - 1, sizeof(base)));
				break;
			}
		}
	}

	/* pass each transformation through the base and log each unique
	 * transform created */

	for (int i = 0; i < num_transforms; i++) {
		int pos = 0;
		void *try = (void *)transformer(transforms[i], base, &pos);
		while (try) {
			kl_insert(unique_transforms, try, try);
			try = transformer(transforms[i], base, &pos);
		}
	}

	printf("part one: %d\n", kl_count(unique_transforms));

	reset_state(true);

	fclose(ifile);
	return EXIT_SUCCESS;
}


/*
 * part two:
 *
 */

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
	char iline[INPUT_LEN_MAX];
	reset_state(0);

	while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {

		/* input is ordered transform rules, blank line, and the base
		 * text. if the line fails parsing, it is either blank or the
		 * base text. the parsing requires enough code to hide
		 * elsewhere.
		 *
		 * for part two, we should switch the from and to so we can work
		 * from the base down to a single electron. */

		if (parse_line(iline))
			invert(transforms[num_transforms-1]);
		else {
			if (strlen(iline) > 1) {
				strncpy(base, iline, min(strlen(iline) - 1, sizeof(base)));
				break;
			}
		}
	}

	/* where in part one we were looking for how many unique
	 * transformations we can make, in part two we want to know how
	 * long it takes to get from one single electron to the final
	 * molecule in the input. i don't see an elegant way to do this, so
	 * we'll churn things in order and see what we get.
	 *
	 * there are two different approaches to take with the
	 * transformations. sorting by descending length of the inverted
	 * from text should more quickly consume the base string. just be
	 * sure to leave the to 'e' rules at the end.
	 *
	 * another approach is to randomize the input. i tried both and am
	 * happier with the randomized approach. the sort code was straight
	 * forward but unless the data is cooked for the problem, i don't
	 * see it as a general solution.
	 *
	 * for either the sort or the shuffle, keep the 'e' transforms at
	 * the end of the rule list. the come at the end of the provied
	 * rules. */

	int trimmed = 0;
	for (int i = 0; i < num_transforms; i++) {
		if (transforms[i]->to[0] == 'e') {
			trimmed = i;
			break;
		}
	}

	/* resist the urge to put this right before the shuffle() call. once
	 * is sufficient. */

	srand(time(NULL));

	/* a big loop with some blinking lights so you can be sure it's not
	 * stuck. */

	int churn = 0;
	int min_run = INT_MAX;
	char *shrinking = NULL;

	while (churn < 100) {
		churn += 1;

		/* blink the lights */
		if (churn % 1000 == 0)
			printf("%d...\n", churn);

		/* reset rule order and starting molecule */

		shuffle((void *)transforms, trimmed);
		shrinking = strdup(base);
		num_run = 0;

		bool did_one = true;

		while (did_one) {
			did_one = false;

			/* run each transformation rule in this sequence */

			for (int i = 0; i < num_transforms; i++) {
				int pos = 0;
				char *try = transformer(transforms[i], shrinking, &pos);
				while (try) {
					did_one = true;
					free(shrinking);
					shrinking = try;
					num_run += 1;
					try = transformer(transforms[i], shrinking, &pos);
				}
			}

			/* how did that ordering work out? */
			if (strcmp(shrinking, "e") == 0) {
				min_run = num_run;
				printf("success in %d transforms at trial %d\n", num_run, churn);
				break;
			}
		}
	}

	free(shrinking);

	printf("part two: %d\n", min_run);

	reset_state(1);

	fclose(ifile);
	return EXIT_SUCCESS;
}
