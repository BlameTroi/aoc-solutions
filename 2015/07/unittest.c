/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include "minunit.h"
#include "solution.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>


/*
 * global/static data and constants.
 */


/*
 * minunit setup and teardown of infrastructure.
 */

void
test_setup(void) {
}

void
test_teardown(void) {
}


/*
 * may as well verify creation separately.
 */

MU_TEST(test_created) {
   mu_assert(1, "just do something");
}

MU_TEST(test_initialized) {
   int want=4;
   int got=4;
   mu_assert_int_eq(want, got);
}


/*
 * more sample input from problem text to mine for test
 * cases if needed.
 */

const char* cin2 = "456 -> y\n";
const char* cin4 = "xa OR y -> e\n";
const char* cin6 = "y RSHIFT 2 -> g\n";
const char* cin7 = "NOT xa -> h\n";           /* negation */


/*
 * test individual forms.
 */

MU_TEST(test_parse_signal) {
   const char* cin1 = "123 -> xa\n";             /* raw signals */
   connection_t pb = parseConnection(cin1);
   mu_assert_string_eq("xa", pb.wire);
   mu_assert_int_eq(se_value, pb.source_type);
   mu_assert_int_eq(123, pb.inp_value);
}

MU_TEST(test_parse_wire) {
   const char* cin9 = "xa -> zz\n";              /* wire to wire */
   connection_t pb = parseConnection(cin9);
   mu_assert_string_eq("zz", pb.wire);
   mu_assert_int_eq(se_wire, pb.source_type);
   mu_assert_string_eq("xa", pb.inp_wire);
}

MU_TEST(test_parse_and) {
   const char* cin3 = "xa AND y -> da\n";        /* and/or gates */
   connection_t pb = parseConnection(cin3);
   mu_assert_string_eq("da", pb.wire);
   mu_assert_int_eq(se_gate, pb.source_type);
   mu_assert_int_eq(ge_and, pb.inp_gate.type);
   mu_assert_string_eq("xa", pb.inp_gate.wire1);
   mu_assert_string_eq("y", pb.inp_gate.wire2);
}

MU_TEST(test_parse_and_literal) {
   const char* cin4a = "1 OR y -> de\n";         /* literals are possible */
   connection_t pb = parseConnection(cin4a);
   mu_assert_string_eq("de", pb.wire);
   mu_assert_int_eq(se_gate, pb.source_type);
   mu_assert_int_eq(ge_or, pb.inp_gate.type);
   mu_assert_string_eq("y", pb.inp_gate.wire1);   /* assume wire, if none found then it's a literal */
   mu_assert_int_eq(1, pb.inp_gate.mask);
   mu_assert_string_eq("", pb.inp_gate.wire2);
}

MU_TEST(test_parse_shift) {
   const char* cin5 = "xa LSHIFT 2 -> f\n";      /* bit shifters */
   connection_t pb = parseConnection(cin5);
   mu_assert_string_eq("f", pb.wire);
   mu_assert_int_eq(se_gate, pb.source_type);
   mu_assert_int_eq(ge_lshift, pb.inp_gate.type);
   mu_assert_string_eq("xa", pb.inp_gate.wire1);
   mu_assert_int_eq(2, pb.inp_gate.bit_shift);
}

MU_TEST(test_parse_not) {
   const char* cin8 = "NOT y -> i\n";
   connection_t pb = parseConnection(cin8);
   mu_assert_string_eq("i", pb.wire);
   mu_assert_int_eq(se_gate, pb.source_type);
   mu_assert_int_eq(ge_not, pb.inp_gate.type);
   mu_assert_string_eq("y", pb.inp_gate.wire1);
}

MU_TEST(test_example_one) {

   /* example circuit from problem statement */
   char *example[9];
   example[0] = "123 -> x\n";
   example[1] = "456 -> y\n";
   example[2] = "x AND y -> d\n";
   example[3] = "x OR y -> e\n";
   example[4] = "x LSHIFT 2 -> f\n";
   example[5] = "y RSHIFT 2 -> g\n";
   example[6] = "NOT x -> h\n";
   example[7] = "NOT y -> i\n";
   example[8] = NULL;                /* end sentinel */

   /* expected values after run */
   typedef struct expected_t {
      char *wire;
      uint16_t value;
   } expected_t;

   expected_t results[] = {
      {"d", 72},
      {"e", 507},
      {"f", 492},
      {"g", 114},
      {"h", 65412},
      {"i", 65079},
      {"x", 123},
      {"y", 456},
      {NULL, 0}                     /* end sentinel */
   };

   circuit_t *circuit = initCircuit();

   int i;
   for (i = 0; example[i]; i++) {
      mu_assert_int_eq(1, addConnection(circuit, example[i]));
   }

   runCircuit(circuit);

   i = 0;
   while(results[i].wire) {
      connection_t *conn = connectionFor(circuit, results[i].wire);
      printf("wire %s  expected %d  actual %d\n", results[i].wire, results[i].value, conn->signal);
      i += 1;
   }

   freeCircuit(circuit);
}

/*
 * here we define the whole test suite. sadly there's no runtime
 * introspection. there is probably an opportunity for an elisp helper
 * to create the suite in the editor, but for now it's just a matter
 * of doing it manually.
 */
MU_TEST_SUITE(test_suite) {

   /* always have a setup and teardown, even if they */
   /* do nothing. */

	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

   /* is it plugged in? */
	MU_RUN_TEST(test_created);
	MU_RUN_TEST(test_initialized);

   /* and more tests as needed */
   MU_RUN_TEST(test_parse_signal);
   MU_RUN_TEST(test_parse_wire);
   MU_RUN_TEST(test_parse_and);
   MU_RUN_TEST(test_parse_and_literal);
   MU_RUN_TEST(test_parse_shift);
   MU_RUN_TEST(test_parse_not);

   /* execution */
   MU_RUN_TEST(test_example_one);
}


int
main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
