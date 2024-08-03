/* solution.h -- aoc 2016 01 -- troy brumley */

#ifndef SOLUTION_H
#define SOLUTION_H

#include <stdbool.h>
#include <stdint.h>

#define INPUT_LINE_MAX 4096

/*
 * since the data would all fit in 16 bits, i opted for using bitwise
 * operations.
 *
 * interestingly, #defines of the form 0x00000001 don't work as i expect.
 * one misses the rational mainframe memory layout.
 *
 * there are five elements in the live data, and two in the sample.
 * each element has a generator paired with a microchip.
 */

typedef uint16_t element;

/* elements in live */
#define   promethium  (1 << 0)
#define   cobalt      (1 << 1)
#define   curium      (1 << 2)
#define   ruthenium   (1 << 3)
#define   plutonium   (1 << 4)

/* elements in sample */
#define   hydrogen    (1 << 5)
#define   lithium     (1 << 6)

/* convert via shift to and from generator or microchip. microchip is
 * actually unshifted and the from/to_microchip macros end up doing
 * nothing, but the symmetry makes the code read better and would be
 * more maintainable if bit assignments changed. */

#define   to_generator(e)   (element)((e) << 8)
#define   to_microchip(e)   (element)(e)
#define   from_generator(e) (element)((e) >> 8)
#define   from_microchip(e) (element)(e)

/* collected */

#define all_elements (element)(promethium | cobalt | curium | ruthenium | plutonium | hydrogen | lithium)
#define all_as_microchips (element)to_microchip(all_elements)
#define all_as_generators (element)to_generator(all_elements)
#define all_as_parts (element)(all_as_microchips | all_as_generators)

typedef struct printable printable;
struct printable {
   element e;
   char *name;
   char *gen;
   char *chip;
};

printable *
get_printable(
   element
);

/* the floors of the building. */
enum level { f1 = 0, f2 = 1, f3 = 2, f4 = 3 };
#define LEVEL_DIM 4

/* state of a move */
typedef struct move move;
struct move {
   int no;
   int elevator;
   element parts[LEVEL_DIM];
};

void
init_and_load(
   move *,
   bool use_live
);

bool
is_valid(
   move *m
);

bool
is_goal(
   move *m
);

int
seek_goal(
   move *start
);

void
report(
   move *m
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
