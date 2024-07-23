/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"
#include "solution.h"

/*
 * minunit setup and teardown of infrastructure.
 */

bool got_args = false;
int num_args = 0;
char **the_args = NULL;
char *filename = NULL;
FILE *datafile = NULL;
char *test_string = NULL;

/*
 * unittest (filename | -f filename) | -e string ...
 */

bool
get_args(void) {
   if (num_args < 2) {
      got_args = true;
   }
   if (got_args) {
      return true;
   }

   if (num_args == 2) {
      filename = the_args[1];
      got_args = true;
      return got_args;
   }

   if (num_args > 2 && strcmp(the_args[1], "-f") == 0) {
      filename = the_args[2];
      got_args = true;
      return got_args;
   }

   if (strcmp(the_args[1], "-i") != 0) {
      fprintf(stderr, "error: illegal arguments to unittest\n");
      exit(EXIT_FAILURE);
   }

   int len = num_args;
   for (int i = 2; i < num_args; i++) {
      len += strlen(the_args[i]);
   }
   test_string = malloc(len+1);
   memset(test_string, 0, len+1);
   for (int i = 2; i < num_args; i++) {
      if (i != 2) {
         strcat(test_string, " ");
      }
      strcat(test_string, the_args[i]);
   }

   got_args = true;
   return got_args;
}

void
test_setup(void) {
   get_args();
   if (filename) {
      datafile = fopen(filename, "r");
      if (!datafile) {
         fprintf(stderr, "error: could not open test data file!\n");
         exit(EXIT_FAILURE);
      }
   }
}

void
test_teardown(void) {
   if (datafile) {
      fclose(datafile);
   }
}

/*
 * sample test shell.
 */

MU_TEST(test_basic) {

   program *p = create_program();
   mu_should(p);
   display *d = create_display();
   mu_should(d);

   instruction instr;

   /* 2 wide, 4 tall */
   mu_should(compile_instruction(&instr, "rect 2x4\n"));
   p->ip = 0;
   p->instructions[0] = instr;

   mu_should(run_program(p, d));

   show_display(d, stdout);

   mu_should(get_pixel(d, 0, 0) == ON);
   mu_should(get_pixel(d, 1, 0) == ON);
   mu_should(get_pixel(d, 2, 0) == ON);
   mu_should(get_pixel(d, 3, 0) == ON);
   mu_should(get_pixel(d, 4, 0) == OFF);
   mu_should(get_pixel(d, 4, 2) == OFF);
   mu_should(get_pixel(d, 3, 1) == ON);
   mu_should(get_pixel(d, 3, 2) == OFF);

   mu_should(count_lit_pixels(d) == 8);

   destroy_display(d);
   destroy_program(p);
}

MU_TEST(test_wrap) {
   program *p = create_program();
   display *d = create_display();

   instruction instr;

   mu_shouldnt(compile_instruction(&instr, "rect 51x7"));
   mu_should(compile_instruction(&instr, "rect 50x6"));
   p->ip = 0;
   p->instructions[0] = instr;
   run_program(p, d);
   show_display(d, stdout);
   mu_should(count_lit_pixels(d) == 50 * 6);
   clear_display(d);
   mu_should(count_lit_pixels(d) == 0);

   mu_should(compile_instruction(&instr, "rect 1x1"));
   p->ip = 0;
   p->instructions[0] = instr;
   run_program(p, d);
   show_display(d, stdout);
   mu_should(count_lit_pixels(d) == 1);
   mu_should(get_pixel(d, 0, 0) == ON);
   mu_should(get_pixel(d, 1, 0) == OFF);
   mu_should(get_pixel(d, 1, 1) == OFF);
   mu_should(get_pixel(d, 0, 1) == OFF);

   mu_should(compile_instruction(&instr, "rotate row y=0 by 5\n"));
   p->ip = 0;
   p->instructions[0] = instr;
   run_program(p, d);
   show_display(d, stdout);
   mu_should(count_lit_pixels(d) == 1);
   /* mu_should(d->pixels[0][0] == OFF); */
   /* mu_should(d->pixels[1][0] == ON); */

   mu_should(compile_instruction(&instr, "rotate column x=5 by 3\n"));
   p->ip = 0;
   p->instructions[0] = instr;
   run_program(p, d);
   show_display(d, stdout);
   mu_should(count_lit_pixels(d) == 1);
   /* mu_should(compile_instruction(&p->instructions[0], "rect 3x2\n")); */
   /* mu_should(compile_instruction(&p->instructions[1], "rotate column x=1 by 1\n")); */
   /* mu_should(compile_instruction(&p->instructions[2], "rotate row y=0 by 4\n")); */
   /* mu_should(compile_instruction(&p->instructions[3], "rotate column x=1 by 1\n")); */

   /* p->trace = true; */
   /* mu_should(run_program(p, d)); */

   /* show_display(d, stdout); */

   /* mu_should(count_lit_pixels(d) == 127); */

   destroy_display(d);
   destroy_program(p);
}

MU_TEST(test_sample) {
   program *p = create_program();
   display *d = create_display();

   /* from the problem description, but my display is too big
    * to demonstrate wrapping, so another test is called for. */
   mu_should(compile_instruction(&p->instructions[0], "rect 3x2\n"));
   mu_should(compile_instruction(&p->instructions[1], "rotate column x=1 by 1\n"));
   mu_should(compile_instruction(&p->instructions[2], "rotate row y=0 by 4\n"));
   mu_should(compile_instruction(&p->instructions[3], "rotate column x=1 by 1\n"));

   p->trace = stdout;
   mu_should(run_program(p, d));

   show_display(d, stdout);

   mu_should(count_lit_pixels(d) == 6);

   destroy_display(d);
   destroy_program(p);
}

MU_TEST_SUITE(test_suite) {

   MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

   /* run your tests here */
   MU_RUN_TEST(test_basic);
   MU_RUN_TEST(test_sample);
   MU_RUN_TEST(test_wrap);

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
