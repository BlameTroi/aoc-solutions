/* solution.c -- aoc 2016 04 -- troy brumley */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

#define TXBSTR_IMPLEMENTATION
#include "txbstr.h"

#define TXBMISC_IMPLEMENTATION
#include "txbmisc.h"

/*
 * find the right room:
 *
 * Each room consists of an encrypted name (lowercase letters
 * separated by dashes) followed by a dash, a sector ID, and a
 * checksum in square brackets.
 *
 * A room is real (not a decoy) if the checksum is the five most
 * common letters in the encrypted name, in order, with ties broken
 * by alphabetization.
 *
 * Room names are decrypted via rot-sector_id for letters and '-'
 * becomes a space.
 *
 * for part one, sum the sector ids of the valid rooms. for part two
 * return the sector id of the room 'northpole object storage'.
 */

#define ROOM_CODE_TAG "__ROOM__"

#define UNKNOWN 0
#define VALID 1
#define INVALID 2

typedef struct freq freq;

struct freq {
	int chr;
	int cnt;
} ;

struct room_code {
	char tag[8];
	room_code *next;
	char *as_read;
	char *encrypted_name;               /* these point into the as_read block */
	char *sector_id;                    /* .. */
	room_checksum provided;
	room_checksum calculated;
	char valid;
	freq frequency_map[26];
};

/*
 * qsort comparator to sort entries in the frequency map into
 * checksum order (descending on count, ascending within counts
 * by alphabetic order.
 */

static int
checksum_sort(const void *pa, const void *pb) {
	const freq *a = pa;
	const freq *b = pb;
	int res = a->cnt - b->cnt;
	if (res)
		return -res;
	res = a->chr - b->chr;
	return res;
}

/*
 * delete storage allocated for this room, including the room block
 * itself.
 */

void
destroy_room_code(room_code *rc) {
	assert(strcmp(rc->tag, ROOM_CODE_TAG) == 0);
	rc->sector_id = NULL;
	rc->encrypted_name = NULL;
	free(rc->as_read);
	memset(rc, 254, sizeof(*rc));
	free(rc);
}

/*
 * moderately trusting creation of a room code block from an input
 * line.
 */

room_code *
create_room_code(char *str) {
	room_code *rc = malloc(sizeof(room_code));
	memset(rc, 0, sizeof(room_code));
	memcpy(rc->tag, ROOM_CODE_TAG, sizeof(rc->tag));

	rc->as_read = strdup(str);
	rc->encrypted_name = strdup(str);

	char *p = rc->encrypted_name;
	int i = -1;
	while (true) {
		i += 1;
		assert(p[i]);
		if (is_lowercase(p[i]) ||
		(p[i] == '-' && is_lowercase(p[i+1])))
			continue;
		break;
	}

	/* if format is of input is correct, i should have us positioned
	 * at the dash in '-999[aaaaa]'. */

	p[i] = '\0';               /* end the encrypted room name */
	i += 1;
	assert(is_digit(p[i]));
	rc->sector_id = p+i;
	while (p[i] && p[i] != '[')
		i += 1;
	assert(p[i] == '[');
	p[i] = '\0';
	i += 1;
	int j = 0;
	for (j = 0; j < CHECK_DIGITS; j++) {
		assert(is_lowercase(p[i+j]));
		rc->provided.digits[j] = p[i+j];
	}
	assert(p[i+j] == ']');
	return rc;
}

/*
 * compute the valid checksum for comparison against the provided
 * checksum.
 */

static void
calculate_checksum(
	room_code *rc
) {

	for (int i = 0; i < 26; i++) {
		rc->frequency_map[i].chr = 'a' + i;
		rc->frequency_map[i].cnt = 0;
	}

	for (int i = 0; i < CHECK_DIGITS; i++)
		rc->calculated.digits[i] = ' ';

	char *p = rc->encrypted_name;
	while (*p) {
		if (is_lowercase(*p))
			rc->frequency_map[*p - 'a'].cnt += 1;
		p += 1;
	}

	/* sort frequency map by descending [1] and ascending [0] */
	qsort(rc->frequency_map, 26, sizeof(freq), checksum_sort);
	for (int i = 0; i < CHECK_DIGITS; i++)
		rc->calculated.digits[i] = rc->frequency_map[i].chr;
}

/*
 * return the encrypted name within the room code block.
 */

const char *
get_encrypted_name(room_code *rc) {
	assert(rc && memcmp(rc->tag, ROOM_CODE_TAG, sizeof(rc->tag)) == 0);
	return rc->encrypted_name;
}

/*
 * return the sector id of the room.
 */

const char *
get_sector_id(room_code *rc) {
	assert(rc && memcmp(rc->tag, ROOM_CODE_TAG, sizeof(rc->tag)) == 0);
	return rc->sector_id;
}

/*
 * return the provided checksum.
 */

const room_checksum *
get_provided_checksum(room_code *rc) {
	assert(rc && memcmp(rc->tag, ROOM_CODE_TAG, sizeof(rc->tag)) == 0);
	return &rc->provided;
}

/*
 * return the calculated checksum.
 */

const room_checksum *
get_valid_checksum(room_code *rc) {
	assert(rc && memcmp(rc->tag, ROOM_CODE_TAG, sizeof(rc->tag)) == 0);
	if (rc->calculated.digits[0] == '\0')
		calculate_checksum(rc);
	return &rc->calculated;
}

/*
 * decrypt the name as a shift cypher using rot-sector_id for
 * lower case letters. dashes becomes spaces. anything else will
 * end up mangled.
 */

static char
decrypt_char(int rot, char c) {
	if (c == '-')
		return ' ';
	rot %= 26;
	return 'a' + (((c - 'a') + rot) % 26);
}

/*
 * decrype the room name. this only produces something
 * readable if the checksum is valid.
 */

const char *
get_decrypted_name(room_code *rc) {
	char *decrypted = strdup(get_encrypted_name(rc));
	int rot = strtol(get_sector_id(rc), NULL, 10);
	char *p = decrypted;
	while (*p) {
		*p = decrypt_char(rot, *p);
		p += 1;
	}
	return decrypted;
}

bool
is_valid_room_code(room_code *rc) {
	assert(rc && memcmp(rc->tag, ROOM_CODE_TAG, sizeof(rc->tag)) == 0);
	if (rc->valid == UNKNOWN) {
		rc->valid = memcmp(get_provided_checksum(rc),
			get_valid_checksum(rc),
		sizeof(rc->provided)) == 0 ? VALID : INVALID;
	}
	return rc->valid == VALID;
}

/*
 * part one. how many encrypted names appear to be valid. return the
 * sum of the sector ids.
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

	long i = 0;
	while (fgets(iline, INPUT_LINE_MAX - 1, ifile)) {
		room_code *rc = NULL;
		rc = create_room_code(iline);
		if (is_valid_room_code(rc)) {
			int sid = strtol(get_sector_id(rc), NULL, 10);
			i += sid;
		}
		destroy_room_code(rc);
	}

	printf("part one: %ld\n", i);

	fclose(ifile);
	return EXIT_SUCCESS;
}


/*
 * part two. what is the sector id of a specific room.
 */

#define TARGET_NAME "northpole object storage"

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

	long sid = 0;
	while (sid == 0 && fgets(iline, INPUT_LINE_MAX - 1, ifile)) {
		room_code *rc = NULL;
		rc = create_room_code(iline);
		if (is_valid_room_code(rc) &&
		strcmp(get_decrypted_name(rc), TARGET_NAME) == 0)
			sid = strtol(get_sector_id(rc), NULL, 10);
		destroy_room_code(rc);
	}

	printf("part two: %ld\n", sid);

	fclose(ifile);
	return EXIT_SUCCESS;
}
