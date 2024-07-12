/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */


#include <stdlib.h>
#include <string.h>

#include "minunit.h"
#include "solution.h"


/*
 * global/static data and constants.
 */

/* result of the following program should be a=2 */
char *prog_one[] = {
   "inc a\n",
   "jio a, +2\n",
   "tpl a\n",
   "inc a\n",
   NULL
};

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
 * sample test shell.
 */
MU_TEST(test_create) {
   cpu *eniac = create(RAM_MAX);
   mu_should(eniac);
   mu_should(eniac->pic == 0);
   mu_should(eniac->a == 0);
   mu_should(eniac->b == 0);
   mu_should(eniac->ram[0].opc == INV);
   free(eniac);
}

MU_TEST(test_assemble_one) {
   inst code = assemble("hlf a\n");
   mu_should(code.opc == HLF);
   mu_should(code.offset == 0);
   mu_should(code.reg == RA);
   code = assemble("tpl b\n");
   mu_should(code.opc == TPL);
   mu_should(code.offset == 0);
   mu_should(code.reg == RB);
   code = assemble("jmp +7\n");
   mu_should(code.opc == JMP);
   mu_should(code.offset == 7);
   mu_should(code.reg == 0);
   code = assemble("jie b, -3\n");
   mu_should(code.opc == JIE);
   mu_should(code.offset == -3);
   mu_should(code.reg == RB);
}
/*   "inc a\n",
   "jio a, +2\n",
   "tpl a\n",
   "inc a\n", */

MU_TEST(test_program_one) {
   cpu *eniac = create(10);
   int i = 0;
   while (prog_one[i]) {
      eniac->ram[i] = assemble(prog_one[i]);
      i += 1;
   }
   run(eniac);
   mu_should(eniac->pic == 4);
   mu_should(eniac->b == 0);
   mu_should(eniac->a == 2);
   free(eniac);
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

   /* run your tests here */
   MU_RUN_TEST(test_create);
   MU_RUN_TEST(test_assemble_one);
   MU_RUN_TEST(test_program_one);
}

/*
 * master control:
 */

int
main(int argc, char *argv[]) {
   MU_RUN_SUITE(test_suite);
   MU_REPORT();
   return MU_EXIT_CODE;
}
