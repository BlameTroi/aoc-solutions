/* solution.h -- aoc 2016 14 -- troy brumley */

#ifndef SOLUTION_H
#define SOLUTION_H

#define INPUT_LINE_MAX 4096

#include <stdbool.h>
#include <stdint.h>

typedef struct hash_check hash_check;
struct hash_check {
   long idx;                   /* index appended to salt to get this hash */
   bool has_triple;            /* sequences of three repeating digits? we only */
   uint8_t triple_digit;       /* care about the first one */
   bool has_quintuples;        /* any sequences of five repeating digits? */
   bool quintuples[16];        /* table of digits that participate in quintuples */
};

hash_check *
create_hash_check(
   const char *salt,           /* why not seed? */
   long idx,                   /* current index */
   long stretcher              /* how many times to hash the hash */
);

void
print_hash_check(
   const hash_check *hc,
   const char *salt,
   long idx
);

long
generate(
   const char *salt,           /* salty goodness */
   long start_idx,             /* start from */
   long stretcher,             /* do we hash the hashes, if so how often */
   long desired_key            /* which key number index is desired */
);

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
