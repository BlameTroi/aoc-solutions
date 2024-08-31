/* solution.c -- aoc 2016 14 -- troy brumley */

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

#define TXBMD5_IMPLEMENTATION
#include "txbmd5.h"

#define TXBSTR_IMPLEMENTATION
#include "txbstr.h"

#define TXBMISC_IMPLEMENTATION
#include "txbmisc.h"

#define SALT "ahsbgdzn"
#define WITHIN_NEXT 1000
#define DESIRED_KEY 64

/*
 * for this problem (aoc 2016 day 14) we are looking for md5 hashes with either
 * three or five repeating digits. while i could convert to character and use
 * regular expressions, that's no fun.
 *
 * since the hash can be recreated from the salt and an integer index, and
 * we are looking for a hash with a triple followed within the next thousdand
 * possible hashes by a quintuple, and we check every hash in sequence, it's
 * better to cache the salt, index, and analysis for each hash even though
 * most will not be hits.
 *
 * since we don't care about those hashes that don't contain a triple or a
 * quintuple, return NULL for those.
 */

hash_check *
create_hash_check(
        const char *salt,
        long idx,
        long stretcher
) {

	/* build string to take md5 hash of. salt and a long (decimal)
	 * integer. */

	char source[33];
	memset(source, 0, sizeof(source));
	strcpy(source, salt);
	snprintf(source+strlen(salt), 255-strlen(salt), "%ld", idx);

	/* to simplify the cross byte boundary checks, the digest is padded
	 * with leading and trailing bytes that are then set to values that
	 * won't satisfy the cross byte check. */

	uint8_t digest_buffer[20];
	memset(digest_buffer, 0, sizeof(digest_buffer));
	md5_string(source, digest_buffer+2);

	char stretched[33];
	while (stretcher) {
		memset(stretched, 0, sizeof(stretched));
		hex_unpack(stretched, 33, digest_buffer+2, 16);
		md5_string(stretched, digest_buffer+2);
		stretcher -= 1;
	}

	digest_buffer[1] = ~digest_buffer[2];
	digest_buffer[0] = ~digest_buffer[1];
	digest_buffer[18] = ~digest_buffer[17];
	digest_buffer[19] = ~digest_buffer[18];

	/* scan through the hash looking for sequences of three and five
	 * repeating hexadecimal digits. rather than unpack the bytes to
	 * character and doing a character level scan, i opted for doing
	 * things at the half byte level.
	 *
	 * there are some redundant checks since i'm managing the byte by
	 * byte loop with for. i could skip past the next byte when i find
	 * a quintuple, but out of 24k hashes with a salt of abc, i found
	 * 2.4k hashes with triples and 12 hashes with quintuples. trimming
	 * 12 out of 24k searches isn't worth it. */

	hash_check hc;
	memset(&hc, 0, sizeof(hc));
	hc.idx = idx;
	hc.has_triple = false;
	hc.has_quintuples = false;

	for (int j = 0; j < 16; j++) {

		/* unpack this byte, do its digits match? if not, skip to the
		 * next byte. */

		uint8_t *db = digest_buffer +       /* the digest is 2 bytes into */
		              2 +                   /* the buffer */
		              j;

		uint8_t ld = (*db & 0xf0);          /* high digit */
		uint8_t rd = (*db & 0x0f);          /* low digit */

		if ((ld >> 4) != rd)
			continue;

		/* it's a double, if we haven't already detected a triple, check
		 * for one by looking at the right or left four bits of the
		 * surrounding bytes.*/

		if (!hc.has_triple) {
			if ((*(db-1) & 0x0f) == rd) {
				hc.has_triple = true;
				hc.triple_digit = rd;
			}
			if ((*(db+1) & 0xf0) == ld) {
				hc.has_triple = true;
				hc.triple_digit = rd;
			}
		}

		/* there's no point in looking for a quintuple until we've
		 * seen a triple in this hash. */

		if (!hc.has_triple)
			continue;

		/* preceeding byte and left side of following byte? */

		if ((*(db-1) == *db) && ((*(db+1) & 0xf0) == ld)) {
			hc.has_quintuples = true;
			hc.quintuples[rd] = true;
		}

		/* right side of preceeding byte and following byte? */
		if ((*(db+1) == *db) && ((*(db-1) & 0x0f) == rd)) {
			hc.has_quintuples = true;
			hc.quintuples[rd] = true;
		}
	}

	/* if there are no triples, this hash is not of interest */

	if (!hc.has_triple)
		return NULL;

	/* return a copy */

	hash_check *res = malloc(sizeof(*res));
	*res = hc;
	return res;
}

/*
 * display for debugging.
 */

void
print_hash_check(
        const hash_check *hc,
        const char *salt,
        long idx
) {
	char chr[33];
	uint8_t hex[16];
	char str[33];
	memset(str, 0, sizeof(str));
	memset(chr, 0, sizeof(str));
	memset(hex, 0, sizeof(hex));
	snprintf(str, 32, "%s%ld", salt, idx);
	md5_string(str, hex);
	printf("%-16s  %s %c [%x] %c [",
	       str, hex_unpack(chr, 33, hex, 16),
	       hc->has_triple ? 'T' : ' ',
	       hc->triple_digit,
	       hc->has_quintuples ? 'Q' : ' ');
	for (int i = 0; i < 16; i++) {
		if (hc->quintuples[i])
			printf("%x", i);
	}
	printf("]\n");
}

/*
 * qsort comparator for an array of longs.
 */

int
fn_cmp_long_asc(const void *a, const void *b) {
	return *(long *)a - *(long *)b;
}

/*
 * generate one time pad keys via salt + index -> md5 -> (stretch) ->
 * look for triples and quintuples of digits.
 */

long
generate(const char *salt,
         long start_idx,
         long stretcher,
         long desired_key
        ) {

	/* keep the prior thousdand hash analysis blocks for checking. */

	const long wrap_around = WITHIN_NEXT + 1;
	hash_check **checks = malloc(sizeof(hash_check) * wrap_around);
	memset(checks, 0, sizeof(hash_check) * wrap_around);

	/* churn until we've found enough keys. since we are finding keys
	 * by looking backward instead of forward, we may detect the keys
	 * out of index sequence. while we need only the 64th key index,
	 * run past that to make sure we get the right one.
	 *
	 * i went with half again, 64 + 32 = 96. */

	const int max_keys = desired_key + (desired_key / 2);
	long key_indices[max_keys];
	memset(key_indices, 0, sizeof(key_indices));
	int num_keys = 0;

	long idx = -1;
	while (num_keys < max_keys) {
		idx += 1;

		/* calculate and analyze this hash, saving the result. a NULL is
		 * ok, it means the hash isn't interesting. the buffer holding
		 * results is meant to wrap around.
		 *
		 * the hash analysis blocks are dynamically allocated, so be
		 * sure to free them before ovewriting them. */

		hash_check *hc = create_hash_check(salt, idx, stretcher);
		if (checks[idx % wrap_around])
			free(checks[idx % wrap_around]);
		checks[idx % wrap_around] = hc;

		/* if it isn't a hash with a quintuple, just keep looping. */

		if (!hc || !hc->has_quintuples)
			continue;

		/* every time we get a quintuple, we can look back for potential
		 * keys. over the prior check range, if the hash had a triple
		 * digit, does it match this quintuple's digit? only the first
		 * triple digit in a hash is considered. */

		for (long i = max(1, idx - WITHIN_NEXT);
		                i < idx && num_keys < max_keys;
		                i++) {
			hash_check *possible = checks[i % wrap_around];
			if (!possible)
				continue;
			if (hc->quintuples[possible->triple_digit]) {
				/* printf("(%3d)(%4ld) quintuple at %ld %c%c satisfies possible key at %ld %c%c \n", */
				/*        num_keys, */
				/*        hc->idx - possible->idx, */
				/*        hc->idx, hc->has_triple ? 'T' : ' ', hc->has_quintuples ? 'Q' : ' ', */
				/*        possible->idx, possible->has_triple ? 'T' : ' ', possible->has_quintuples ? 'Q' : ' '); */
				key_indices[num_keys] = possible->idx;
				num_keys += 1;
			}
		}
		idx += 1;
	}

	/* since we're looking backward from the satisfier instead of
	 * forward from the potential, the keys aren't found in
	 * sequence. */

	qsort(key_indices, num_keys, sizeof(long), fn_cmp_long_asc);

	/* be a good citizen and free up storage. */
	for (int i = 0; i < wrap_around; i++) {
		if (checks[i]) {
			free(checks[i]);
			checks[i] = NULL;
		}
	}
	return key_indices[DESIRED_KEY-1];
}

/*
 * part one:
 */

int
part_one(
        const char *fname
) {
	long res = generate(fname, 0, 0, DESIRED_KEY);
	printf("part one: %ld\n", res);

	return EXIT_SUCCESS;
}


/*
 * part two:
 */

int
part_two(
        const char *fname
) {
	long res = generate(fname, 0, 2016, DESIRED_KEY);
	printf("part two: %ld\n", res);

	return EXIT_SUCCESS;
}
