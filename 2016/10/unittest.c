/*  unittest.c -- shell for advent of code unit tests -- troy brumley */

/*  because you should always make an effort to test first! */

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"
#include "solution.h"

#include "txbstr.h"
#include "txbmisc.h"
#include "txbda.h"
#include "txbqu.h"
#include "txbfs.h"

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

char *sample_data[] = {
   "value 5 goes to bot 2\n",
   "bot 2 gives low to bot 1 and high to bot 0\n",
   "value 3 goes to bot 1\n",
   "bot 1 gives low to output 1 and high to bot 0\n",
   "bot 0 gives low to output 2 and high to output 0\n",
   "value 2 goes to bot 2\n",
   NULL
};

typedef struct bot_node bot_node;
struct bot_node {
   bool ref_load;
   bool ref_input;
   bool ref_inferred;
   bool low_bot;
   int low_dest;
   bool high_bot;
   int high_dest;
   bool seen_17;
   bool seen_61;
   int low_value;
   int high_value;
   bool full;
};

typedef struct output_bin output_bin;
struct output_bin {
   fscb *fs;
};

typedef struct load_command load_command;
struct load_command {
   int value;
   int bot;
};

void
load_value(bot_node *bot, int value) {
   assert(bot->low_value == -1 || bot->high_value == -1);
   if (value == 17) {
      bot->seen_17 = true;
   }
   if (value == 61) {
      bot->seen_61 = true;
   }
   if (bot->low_value == -1) {
      bot->low_value = value;
   } else {
      bot->high_value = value;
   }
   if (bot->low_value > bot->high_value) {
      int swap = bot->low_value;
      bot->low_value = bot->high_value;
      bot->high_value = swap;
   }
   if (bot->low_value != -1 && bot->high_value != -1) {
      bot->full = true;
   }
}

void
give_to_bot(bot_node *bot, int value) {
   assert(bot->low_value == -1 || bot->high_value == -1);
   if (value == 17) {
      bot->seen_17 = true;
   }
   if (value == 61) {
      bot->seen_61 = true;
   }
   if (bot->low_value == -1) {
      bot->low_value = value;
   } else {
      bot->high_value = value;
   }
   if (bot->low_value > bot->high_value) {
      int swap = bot->low_value;
      bot->low_value = bot->high_value;
      bot->high_value = swap;
   }
   if (bot->low_value != -1 && bot->high_value != -1) {
      bot->full = true;
   }
}

void
give_to_bin(output_bin *bin, int value) {
   fs_push(bin->fs, (void *)(long)value);
}

char
bool_char(bool b) {
   return b ? 'T' : 'F';
}

char *
dest_string(bool b) {
   return b ? "bot" : "bin";
}

MU_TEST(test_build) {
   qucb *loadq = qu_create();

   int last_bin = -1;
   output_bin bins[100];
   memset(bins, 0, sizeof(bins));
   for (int i = 0; i < 100; i++) {
      bins[i].fs = fs_create(10);
   }

   int last_bot = -1;
   bot_node bots[250];
   memset(bots, 0, sizeof(bots));
   for (int i = 0; i < 250; i++) {
      bots[i].ref_load = false;
      bots[i].ref_input = false;
      bots[i].ref_inferred = false;
      bots[i].seen_17 = false;
      bots[i].seen_61 = false;
      bots[i].low_value = -1;
      bots[i].high_value = -1;
      bots[i].full = false;
   }

   char buffer[255];
   memset(buffer, 0, 255);

   /* for (int i = 0; sample_data[i]; i++) { */
   /* const char **tokens = split_string(sample_data[i], " \n"); */
   while (fgets(buffer, 255, datafile)) {
      const char **tokens = split_string(buffer, " \n");
      if (equal_string(tokens[1], "value")) {
         load_command *cmd = malloc(sizeof(*cmd));
         cmd->value = strtol(tokens[2], NULL, 10);
         cmd->bot = strtol(tokens[6], NULL, 10);
         qu_enqueue(loadq, cmd);
         free_split(tokens);
         continue;
      }
      if (equal_string(tokens[1], "bot")) {
         int bot = strtol(tokens[2], NULL, 10);
         bots[bot].ref_input = true;
         bots[bot].low_bot = equal_string(tokens[6], "bot");
         bots[bot].low_dest = strtol(tokens[7], NULL, 10);
         bots[bot].high_bot = equal_string(tokens[11], "bot");
         bots[bot].high_dest = strtol(tokens[12], NULL, 10);
         if (!bots[bot].low_bot) {
            last_bin = max(last_bin, bots[bot].low_dest);
         }
         if (!bots[bot].high_bot) {
            last_bin = max(last_bin, bots[bot].high_dest);
         }
         last_bot = max(last_bot, bot);
         free_split(tokens);
         continue;
      }
      assert(NULL);
   }

   load_command *cmd = NULL;
   while ((cmd = qu_dequeue(loadq))) {
      load_value(&bots[cmd->bot], cmd->value);
      bots[cmd->bot].ref_load = true;
      free(cmd);
   }
   qu_destroy(loadq);
   loadq = NULL;

   printf("\npost load cmds\n");
   for (int i = 0; i <= last_bot; i++) {
      printf(
         "%3d %c %c %c %s %3d %s %3d %c %3d %3d\n",
         i,
         bool_char(bots[i].ref_load), bool_char(bots[i].ref_input), bool_char(bots[i].ref_inferred),
         dest_string(bots[i].low_bot), bots[i].low_dest,
         dest_string(bots[i].high_bot), bots[i].high_dest,
         bool_char(bots[i].full), bots[i].low_value, bots[i].high_value
      );
   }
   printf("\nrun until no full bots\n");
   bool done = false;
   while (!done) {
      done = true;
      int i = 0;
      while (i <= last_bot) {
         if (bots[i].full) {
            done = false;
            if (bots[i].low_bot) {
               give_to_bot(&bots[bots[i].low_dest], bots[i].low_value);
            } else {
               give_to_bin(&bins[bots[i].low_dest], bots[i].low_value);
            }
            bots[i].low_value = -1;
            if (bots[i].high_bot) {
               give_to_bot(&bots[bots[i].high_dest], bots[i].high_value);
            } else {
               give_to_bin(&bins[bots[i].high_dest], bots[i].high_value);
            }
            bots[i].high_value = -1;
            bots[i].full = false;
            break;
         }
         i += 1;
      }
   }

   printf("\npost run cmds\n");
   for (int i = 0; i <= last_bot; i++) {
      printf(
         "%3d %c %c %c %s %3d %s %3d %c %3d %3d\n",
         i,
         bool_char(bots[i].ref_load), bool_char(bots[i].ref_input), bool_char(bots[i].ref_inferred),
         dest_string(bots[i].low_bot), bots[i].low_dest,
         dest_string(bots[i].high_bot), bots[i].high_dest,
         bool_char(bots[i].full), bots[i].low_value, bots[i].high_value
      );
      if (bots[i].seen_17 && bots[i].seen_61) {
         printf("saw both 17 and 61\n");
      }
   }
   for (int i = 0; i <= last_bin; i++) {
      printf("%3d [%d] ", i, fs_depth(bins[i].fs));
      void *v;
      while (!fs_empty(bins[i].fs)) {
         printf(" %ld", (long)fs_pop(bins[i].fs));
      }
      printf("\n");
   }
   printf("\ndone?\n");
}

MU_TEST(test_test) {
   mu_should(true);
   mu_shouldnt(false);
}

/*
 * here we defini the whole test suite.
 */
MU_TEST_SUITE(test_suite) {

   MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

   /* run your tests here */
   MU_RUN_TEST(test_build);
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
