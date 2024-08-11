/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"
#include "solution.h"

#define TXBMISC_IMPLEMENTATION
#include "txbmisc.h"

/*
 * minunit setup and teardown of infrastructure.
 */

int num_args = 0;
char **the_args = NULL;

void
test_setup(void) {
}

void
test_teardown(void) {
}

/*
 * sample test shell.
 */

/* the illustration from the problem statement */
int sample_min = 0;
int sample_max = 9;
char *sample_input[] = {
   "5-8",
   "0-2",
   "4-7",
   NULL
};

int sample_allowed[] = { 3, 9, -1 };

/* for a solution */

#define IP_MIN 0
#define IP_MAX 4294967295

uint32_t ip_min = 0;
uint32_t ip_max = 4294967295;

typedef struct ip_blacklist_range ip_blacklist_range;
struct ip_blacklist_range {
   uint32_t ip_min;
   uint32_t ip_max;
};
ip_blacklist_range big_range[] = {
   {1829303629, 1830213769},
   {2912748047, 2912973420},
   {296450718, 298533185},
   {0, 296167},
   {3007232257, 3021954584},
   {2582574639, 2595855729},
   {1126754955, 1140997648},
   {2752347098, 2755308045},
   {321293830, 342822868},
   {3044917961, 3053774169},
   {296168, 338264},
   {0, 0}
};

#include <assert.h>

#define BITS ((uint64_t)8)
#define BITMASK(b) (1 << (((uint64_t)(b) % BITS)))
#define BITSLOT(b) (((uint64_t)(b)) / BITS)
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) (((uint64_t)(nb) + BITS - 1) / BITS)






uint32_t
try_it_big() {
   uint32_t c = 4294967295;
   uint64_t n = BITNSLOTS(c);
   uint8_t *bits = malloc(n);
   uint32_t max_seen = 0;
   uint32_t min_seen = 0xFFFFFFFF;
   memset(bits, 0xff, n);
   assert(bits);
   for (uint32_t i = 0; big_range[i].ip_min != big_range[i].ip_max; i++) {
      max_seen = ulmax(max_seen, big_range[i].ip_max);
      min_seen = ulmax(min_seen, big_range[i].ip_min);
      for (uint32_t j = big_range[i].ip_min; j <= big_range[i].ip_max; j++) {
         BITCLEAR(bits, j);
      }
   }
   uint32_t res = 0xFFFFFFFF;
   for (uint32_t i = 0; i <= max_seen; i++) {
      if (BITTEST(bits, i)) {
         res = i;
         break;
      }
   }
   return res;
}

uint32_t
try_it_small() {
   uint8_t *bits = malloc(10); /* calloc(10, 1);*/ /* 0->9 */
   memset(bits, 0xff, 10);
   for (int i = 5; i <= 8; i++) {
      BITCLEAR(bits, i);
   }
   for (int i = 0; i <= 2; i++) {
      BITCLEAR(bits, i);
   }
   for (int i = 4; i <= 7; i++) {
      BITCLEAR(bits, i);
   }
   for (int i = 0; i <= 9; i++) {
      printf("%d %c\n", i, BITTEST(bits, i) ? '1' : '0');
   }
   return 0;
}


MU_TEST(test_test) {
   printf("\n");
   printf("%u\n", try_it_big());
   try_it_small();
   mu_should(true);
   mu_shouldnt(false);
}

/*
 * here we defini the whole test suite.
 */
MU_TEST_SUITE(test_suite) {

   MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

   /* run your tests here */
   MU_RUN_TEST(test_test);
}

/*
 * master control:
 */

int
main(int argc, char *argv[]) {
   num_args = argc;
   the_args = argv;
   MU_RUN_SUITE(test_suite);
   MU_REPORT();
   return MU_EXIT_CODE;
}
