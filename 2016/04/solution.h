/* solution.h -- aoc 2016 01 -- troy brumley */

#ifndef SOLUTION_H
#define SOLUTION_H

#include <stdbool.h>

#define INPUT_LINE_MAX 4096

/*
 * externally visible data.
 */

typedef struct room_checksum room_checksum;

#define CHECK_DIGITS 5
struct room_checksum {
   char digits[CHECK_DIGITS];
};

typedef struct room_code room_code;

/*
 * operations on the room code
 */

room_code *
create_room_code(char *str);

bool
validate_room_code(
   room_code *rc
);

void
destroy_room_code(room_code *rc);

/*
 * access and predicates. is_valid_room will call validate_room_code if
 * it has not been done for this instance.
 */

const char *
get_encrypted_name(room_code *rc);

const char *
get_sector_id(room_code *rc);

const room_checksum *
get_provided_checksum(room_code *rc);

const room_checksum *
get_valid_checksum(room_code *rc);

const char *
get_decrypted_name(room_code *rc);

bool
is_valid_room_code(room_code *rc);

/*
 * and the standard operations for aoc
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
