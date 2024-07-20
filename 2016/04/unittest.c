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


/* sample input from problem specification */

char *iline[] = {
   "aaaaa-bbb-z-y-x-123[abxyz]\n",     /* is a real room */
   "a-b-c-d-e-f-g-h-987[abcde]\n",     /* is a real room */
   "not-a-real-room-404[oarel]\n",     /* is a real room */
   "totally-real-room-200[decoy]\n",   /* is not */
   "qzmt-zixmtkozy-ivhz-343[zimth]\n", /* not yet */
   NULL
};

/*
 * sample test shell.
 */

MU_TEST(test_test) {
   room_code *rc = NULL;
   int i = 0;
   while (iline[i]) {
      rc = create_room_code(iline[i]);
      mu_should(strncmp(get_encrypted_name(rc), iline[i], strlen(get_encrypted_name(rc))) == 0);
      printf("\n   original: %s", iline[i]);
      printf("       name: %s\n", get_encrypted_name(rc));
      printf("     sec id: %s\n", get_sector_id(rc));
      printf("  check sum: %.*s\n", CHECK_DIGITS, (char *)get_provided_checksum(rc));
      printf(" calculated: %.*s\n", CHECK_DIGITS, (char *)get_valid_checksum(rc));
      printf("      valid: %s\n", is_valid_room_code(rc) ? "yes" : "no");
      printf("  decrypted: %s\n", get_decrypted_name(rc));
      destroy_room_code(rc);
      i += 1;
   }
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
