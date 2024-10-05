/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <limits.h>
#include <stdio.h>

#include "minunit.h"
#include "solution.h"
#include "txbmisc.h"

/*
 * this is another reddit find that i wanted to play with, translated from
 * c# on the solutions megathread. i kept the basic recursive algorithm but
 * wired it into my existing code. does not return a valid answer. i've
 * double checked against the source, it ain't me babe.
 */

unsigned long
min_qe_r(int i, unsigned long accum_qe, int accum_weight) {
	if (accum_weight == target_weight)
		return accum_qe;
	if (i >= num_packages || accum_weight > target_weight)
		return LONG_MAX;
	unsigned long lhs = min_qe_r(i + 1, accum_qe *packages[i],
			accum_qe + packages[i]);
	unsigned long rhs = min_qe_r(i + 1, accum_qe, accum_weight);
	if (lhs == LONG_MAX)
		return rhs;
	if (rhs == LONG_MAX)
		return lhs;
	return min(lhs, rhs);
}

unsigned long
try_recursive(char *fname, int groups) {
	load_data(fname, true, groups);
	return min_qe_r(0, 1, 0);
}

/* this is a not sufficiently general solution from the solutions
 * megathread on reddit. i tweaked a few things to convert it from
 * python to c, but it's the same algorithm. unlike my combinator,
 * this does not work on the small dataet from the problem
 * description. */

unsigned long
try_loopy(const char *fname, int groups) {
	load_data(fname, true, groups);
	int total = total_weight;
	int third = total / 3;
	int fourth = total / 4;

	unsigned long quantum_t = LONG_MAX;
	unsigned long quantum_f = LONG_MAX;

	for (int z = num_packages - 1; z > -1; z--) {
		unsigned long a = packages[z];
		for (int y = z - 1; y > -1; y--) {
			unsigned long b = packages[y];
			for (int x = y - 1; x > -1; x--) {
				unsigned long c = packages[x];
				for (int w = x - 1; w > -1; w--) {
					unsigned long d = packages[w];
					if (a+b+c+d == fourth) {
						unsigned long prod = a*b*c*d;
						quantum_f = min(quantum_f, prod);
					}
					for (int v = w - 1; v > -1; v--) {
						unsigned long e = packages[v];
						if (a+b+c+d+e == fourth) {
							unsigned long prod = a*b*c*d*e;
							quantum_f = min(quantum_f, prod);
						}
						for (int u = v - 1; u > -1; u--) {
							unsigned long f = packages[u];
							if (a+b+c+d+e+f == third) {
								unsigned long prod = a*b*c*d*e*f;
								quantum_t = min(quantum_t, prod);
							}
						}
					}
				}
			}
		}
	}
	printf("qt %lu\n", quantum_t);
	printf("qf %lu\n", quantum_f);
	return groups == 3 ? quantum_t : quantum_f;
}

static char *data_file_name = NULL;

void
test_setup(void) {
	zero_data();
}

void
test_teardown(void) {
	zero_data();
}

MU_TEST(test_loopy) {
	unsigned long qe1 = try_loopy(data_file_name, 3);
	unsigned long qe2 = try_loopy(data_file_name, 4);
	mu_should(qe1 > 0 && qe2 > 0);
}

MU_TEST(test_combination) {
	unsigned long qe1 = try_combination(data_file_name, 3);
	unsigned long qe2 = try_combination(data_file_name, 4);
	printf("qe1 %lu\n", qe1);
	printf("qe2 %lu\n", qe2);
	mu_should(qe1 > 0 && qe2 > 0);
}

MU_TEST(test_permute) {
	unsigned long qe1 = try_permutation(data_file_name, 3);
	unsigned long qe2 = try_permutation(data_file_name, 4);
	printf("qe1 %lu\n", qe1);
	printf("qe2 %lu\n", qe2);
	mu_should(qe1 > 0 && qe2 > 0);
}

MU_TEST(test_recursive) {
	unsigned long qe1 = try_recursive(data_file_name, 3);
	unsigned long qe2 = try_recursive(data_file_name, 4);
	printf("qe1 %lu\n", qe1);
	printf("qe2 %lu\n", qe2);
	mu_should(qe1 > 0 && qe2 > 0);
}

MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	MU_RUN_TEST(test_recursive);
	return;
	MU_RUN_TEST(test_loopy);
	MU_RUN_TEST(test_permute);
	MU_RUN_TEST(test_combination);

	return;
}

int
main(int argc, char *argv[]) {

	if (argc > 1)
		data_file_name = argv[1];
	else
		data_file_name = "unitsmall.txt";
	MU_RUN_SUITE(test_suite);
	MU_REPORT();

	return MU_EXIT_CODE;
}
