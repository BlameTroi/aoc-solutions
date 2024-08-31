/* solution.h -- aoc 2016 01 -- troy brumley */

#ifndef SOLUTION_H
#define SOLUTION_H

#include <stdbool.h>

#define INPUT_LINE_MAX 4096

bool
has_good_abba(
        const char *str
);

bool
has_bad_abba(
        const char *str
);

bool
supports_ssl(
        const char *ipv7
);

bool
suports_tls(
        const char *ipv7
);

bool
supports_tls(
        const char *ipv7
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
