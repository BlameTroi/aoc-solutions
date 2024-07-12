/* solution.c -- aoc 2015 13 -- troy brumley */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define TXBSTR_IMPLEMENTATION
#include "txbstr.h"

#define TXBMISC_IMPLEMENTATION
#include "txbmisc.h"

#include "solution.h"

/*
 * make a new cpu
 */

cpu *
create(
   int size
) {
   if (size > RAM_MAX) {
      return NULL;
   }
   cpu *r = malloc(sizeof(cpu));
   memset(r, 0, sizeof(cpu));
   return r;
}

itab iset[] = {
   { "hlf", HLF, true, false },
   { "tpl", TPL, true, false },
   { "inc", INC, true, false },
   { "jmp", JMP, false, true },
   { "jie", JIE, true, true },
   { "jio", JIO, true, true },
   { NULL, 0, false, false }
};

char *
disassemble(char *buff, inst *curr) {
   memset(buff, 0, 255);
   int i = 0;
   char *mne = "inv";
   while (iset[i].mne) {
      if (iset[i].opc == curr->opc) {
         mne = iset[i].mne;
         break;
      }
      i += 1;
   }
   char *reg = "?";
   reg = (iset[i].reg && curr->reg == RA) ? "a" : "b";
   if (iset[i].mne == NULL) {
      snprintf(buff, 255, "%s ERROR", mne);
   } else if (iset[i].reg && iset[i].offset) {
      snprintf(buff, 255, "%s %s, %+d", mne, reg, curr->offset);
      return buff;
   } else if (iset[i].reg) {
      snprintf(buff, 255, "%s %s", mne, reg);
   } else if (iset[i].offset) {
      snprintf(buff, 255, "%s %+d", mne, curr->offset);
   } else {
      snprintf(buff, 255, "%s ERROR", mne);
   }
   return buff;
}

void
trace(cpu *proc) {
   char buff[256];
   printf("%4d %8ld %8ld %s\n",
          proc->pic, proc->a, proc->b,
          disassemble(buff, &proc->ram[proc->pic]));
}

void
run(cpu *proc) {
   proc->pic = 0;
   proc->a = 0;
   proc->b = 0;
   int *pic = &proc->pic;
   unsigned long *reg = NULL;
   unsigned long *ra = &proc->a;
   unsigned long *rb = &proc->b;
   while (*pic >= 0 && *pic < RAM_MAX && proc->ram[proc->pic].opc != INV) {
      /* trace(proc); */
      switch (proc->ram[*pic].opc) {
      case HLF:
         reg = proc->ram[*pic].reg == RA ? ra : rb;
         *reg = *reg / 2;
         *pic += 1;
         break;
      case TPL:
         reg = proc->ram[*pic].reg == RA ? ra : rb;
         *reg = *reg + *reg + *reg;
         *pic += 1;
         break;
      case INC:
         reg = proc->ram[*pic].reg == RA ? ra : rb;
         *reg += 1;
         *pic += 1;
         break;
      case JMP:
         *pic += proc->ram[*pic].offset;
         break;
      case JIE:
         reg = proc->ram[*pic].reg == RA ? ra : rb;
         *pic += (is_even(*reg)) ? proc->ram[*pic].offset : 1;
         break;
      case JIO:
         reg = proc->ram[*pic].reg == RA ? ra : rb;
         *pic += *reg == 1 ? proc->ram[*pic].offset : 1;
         break;
      default:
         /* this is just to shut up clang warnings */
         proc->pic = -1;
      }
   }
}

/*
 * assemble one instruction
 */
inst
assemble(
   char *stmt
) {
   inst r = {INV, 0, 0};
   const char **tokens = split_string(stmt, " ,\n");
   assert(tokens);
   int i = 0;
   while (iset[i].mne) {
      if (strcmp(tokens[1], iset[i].mne) == 0) {
         r.opc = iset[i].opc;
         int sp = 2;
         if (iset[i].reg) {
            if (strcmp(tokens[sp], "a") == 0) {
               r.reg = RA;
            }
            if (strcmp(tokens[sp], "b") == 0) {
               r.reg = RB;
            }
            sp += 1;
         }
         if (iset[i].offset) {
            r.offset = strtol(tokens[sp], NULL, 10);
         }
         break;
      }
      i += 1;
   }

   free((void *)tokens[0]);
   free(tokens);
   /* char buff[256]; */
   /* printf("source->%sdisasm->%s\n\n", stmt, disassemble(buff, &r)); */
   return r;
}

/*
 * part one:
 *
 */

int
part_one(
   const char *fname
) {

   FILE *ifile = fopen(fname, "r");
   if (!ifile) {
      printf("could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }

   char iline[INPUT_LEN_MAX];

   cpu *eniac = create(255);
   int i = 0;

   while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
      /* printf("%3d %s", i, iline); */
      eniac->ram[i] = assemble(iline);
      i += 1;
   }

   run(eniac);

   printf("part one: %ld\n", eniac->b);

   free(eniac);
   fclose(ifile);
   return EXIT_SUCCESS;
}


/*
 * part two:
 *
 * same input program as part one but start a at 1 and not 0.
 * since all jumps are relative, we can just add one instruction
 * to the program rather than horking up the cpu's initial state.
 *
 */

int
part_two(
   const char *fname
) {
   FILE *ifile;

   ifile = fopen(fname, "r");
   if (!ifile) {
      printf("could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }
   char iline[INPUT_LEN_MAX];

   cpu *eniac = create(255);
   int i = 0;

   eniac->ram[i] = assemble("inc a\n");
   while (fgets(iline, INPUT_LEN_MAX - 1, ifile)) {
      i += 1;
      eniac->ram[i] = assemble(iline);
   }

   run(eniac);

   printf("part two: %ld\n", eniac->b);

   free(eniac);
   fclose(ifile);
   return EXIT_SUCCESS;
}
