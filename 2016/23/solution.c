/* solution.c -- aoc 2016 23 -- troy brumley */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "solution.h"

#define TXBSTR_IMPLEMENTATION
#include "txbstr.h"
void
debug_break(void) {
   /* printf("blarg\n"); */
}

char *
disassembled_operand(operand n, char *buffer, int buflen) {
   if (n.reg) {
      snprintf(buffer, buflen-1, "%c", n.opr + 'a');
   } else {
      snprintf(buffer, buflen-1, "%d", n.opr);
   }
   return buffer;
}

char *
disassembled(instruction n, char *buffer, int buflen) {
   char *sopc;
   char sopr1[32];
   memset(sopr1, 0, 32);
   char sopr2[32];
   memset(sopr2, 0, 32);
   switch (n.opc) {
   case inv:
      sopc = "inv";
      break;
   case cpy:
      sopc = "cpy";
      disassembled_operand(n.op1, sopr1, sizeof(sopr1));
      disassembled_operand(n.op2, sopr2, sizeof(sopr2));
      break;
   case inc:
      sopc = "inc";
      disassembled_operand(n.op1, sopr1, sizeof(sopr1));
      break;
   case dec:
      sopc = "dec";
      disassembled_operand(n.op1, sopr1, sizeof(sopr1));
      break;
   case jnz:
      sopc = "jnz";
      disassembled_operand(n.op1, sopr1, sizeof(sopr1));
      disassembled_operand(n.op2, sopr2, sizeof(sopr2));
      break;
   case tgl:
      sopc = "tgl";
      disassembled_operand(n.op1, sopr1, sizeof(sopr1));
      break;
   case nop:
      sopc = "nop";
      break;
   }
   snprintf(
      buffer, buflen, "%s %s %s %s",
      sopc, sopr1, sopr2, n.invalid ? "<- nopped" : "");
   return buffer;
}

char *
trace_line(cpu *m, char *buffer, int buflen) {
   char dis[256];
   memset(dis, 0, 256);
   snprintf(
      buffer, buflen, "[%8d] [%8d] [%8d] [%8d]  %d->%s",
      m->registers[a], m->registers[b], m->registers[c], m->registers[d],
      m->ip, disassembled(m->core[m->ip], dis, 255));
   return buffer;
}

bool
is_register_operand(const char *opr) {
   if (strlen(opr) > 1) {
      return false;
   }
   return opr[0] == 'a' || opr[0] == 'b' || opr[0] == 'c' || opr[0] == 'd';
}

/*
 * conversion of toggled instruction opcodes. from, to, invalid
 * result.
 *
 * for one-argument instructions, inc becomes dec, and all other one-
 * argument instructions become inc. (dec, inc, tgl)
 *
 * for two-argument instructions, jnz becomes cpy, and all other two-
 * instructions become jnz.
 *
 * if an attempt is made to toggle an instruction outside the program,
 * nothing happens.
 *
 * if toggling produces an invalid instruction (like cpy 1 2) and an
 * attempt is later made to execute that instruction, skip it instead.
 *
 * if tgl toggles itself (for example, if a is 0, tgl a would target
 * itself and become inc a), the resulting instruction is not executed
 * until the next time it is reached.
 */

opcode toggle_map[][2] = {
   { cpy, jnz },
   { inc, dec },
   { dec, inc },
   { jnz, cpy },
   { tgl, inc },
   { inv, inv },
   { -1, -1 }
};

operand
assemble_operand(const char *str) {
   operand res;
   if (is_register_operand(str)) {
      res.reg = true;
      res.opr = str[0] - 'a';
   } else {
      res.reg = false;
      res.opr = strtol(str, NULL, 10);
   }
   return res;
}

instruction
assemble(char *str) {
   instruction res;
   memset(&res, 0, sizeof(res));
   res.invalid = false;
   const char **tokens = split_string(str, " \n");
   /* printf("\nsrc: %s", str); */
   if (equal_string("cpy", tokens[1])) {
      /* cpy val reg -or- cpy reg reg */
      res.opc = cpy;
      res.op1 = assemble_operand(tokens[2]);
      res.op2 = assemble_operand(tokens[3]);
   } else if (equal_string("inc", tokens[1])) {
      /* inc reg */
      res.opc = inc;
      res.op1 = assemble_operand(tokens[2]);
   } else if (equal_string("dec", tokens[1])) {
      /* dec reg */
      res.opc = dec;
      res.op1 = assemble_operand(tokens[2]);
   } else if (equal_string("jnz", tokens[1])) {
      /* jnz reg val -or- jnz lit val */
      res.opc = jnz;
      res.op1 = assemble_operand(tokens[2]);
      res.op2 = assemble_operand(tokens[3]);
   } else if (equal_string("tgl", tokens[1])) {
      debug_break();
      res.opc = tgl;
      res.op1 =assemble_operand(tokens[2]);
   } else {
      res.opc = nop;
   }
   free_split(tokens);
   char buffer[256];
   memset(buffer, 0, 256);
   /* printf("dis: %s\n", disassembled(res, buffer, 255)); */
   return res;
}

cpu *
load(FILE *ifile) {
   cpu *m = malloc(sizeof(cpu));
   memset(m, 0, sizeof(cpu));

   char iline[INPUT_LINE_MAX];

   m->ip = 0;
   while (fgets(iline, INPUT_LINE_MAX - 1, ifile)) {
      m->core[m->ip] = assemble(iline);
      m->ip += 1;
   }
   m->core[m->ip].opc = inv;

   return m;
}

int
literal_or_register(cpu *m, operand *o) {
   int res = 0;
   if (o->reg) {
      res = m->registers[o->opr];
   } else {
      res = o->opr;
   }
   return res;
}

bool
execute(cpu *m) {
   m->ip = 0;
   int n;
   char buffer[256];
   memset(buffer, 0, 256);
   instruction *ip = &m->core[m->ip];
   long excp = 0;
   while (ip->opc != inv) {
      /* excp += 1; */
      /* printf("%ld : %s\n", excp, trace_line(m, buffer, 255)); */
      /* char c = getchar(); */
      switch (ip->opc) {
      case cpy:
         if (ip->op2.reg) {
            m->registers[ip->op2.opr] = literal_or_register(m, &ip->op1);
         }
         n = 1;
         break;
      case inc:
         if (ip->op1.reg) {
            m->registers[ip->op1.opr] += 1;
         }
         n = 1;
         break;
      case dec:
         if (ip->op1.reg) {
            m->registers[ip->op1.opr] -= 1;
         }
         n = 1;
         break;
      case jnz:
         if (ip->op1.reg) {
            n = m->registers[ip->op1.opr] == 0 ? 1 : literal_or_register(m, &ip->op2);
         } else {
            n = ip->op1.opr == 0 ? 1 : literal_or_register(m, &ip->op2);
         }
         break;
      case tgl:
         n = 1;
         int mod_ip = m->ip + literal_or_register(m, &ip->op1);
         debug_break();
         if (mod_ip < 0 || mod_ip > 31) {
            break;
         }
         instruction *mp = &m->core[mod_ip];
         int toggle = 0;
         while (toggle_map[toggle][0] != -1) {
            if (toggle_map[toggle][0] == mp->opc) {
               debug_break();
               mp->opc = toggle_map[toggle][1];
               break;
            }
            toggle += 1;
         }
         debug_break();
         assert(toggle_map[toggle][0] != -1);
         break;
      default:
         return false;
      }
      m->ip += n;
      ip = &m->core[m->ip];
   }
   return true;
}

/*
 * part one:
 *
 * this day (23) starts with an updated version of the day 12 asmbunny
 * language. one instruction to modify another instruction is added.
 */

int
part_one(
   const char *fname
) {

   FILE *ifile = fopen(fname, "r");
   if (!ifile) {
      fprintf(stderr, "error: could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }

   cpu *m = load(ifile);
   m->registers[a] = 7;
   m->registers[b] = 0;
   m->registers[c] = 0;
   m->registers[d] = 0;
   int ok = execute(m);

   if (ok) {
      printf("part one: %d\n", m->registers[a]);
   } else {
      printf("part one: error\n");
   }

   fclose(ifile);
   return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}


/*
 * part two:
 *
 */

int
part_two(
   const char *fname
) {
   FILE *ifile;
   ifile = fopen(fname, "r");
   if (!ifile) {
      fprintf(stderr, "error: could not open file: %s\n", fname);
      return EXIT_FAILURE;
   }

   cpu *m = load(ifile);
   /* for part two, c is 1 not 0 at start */
   m->registers[a] = 12;
   m->registers[b] = 0;
   m->registers[c] = 0;
   m->registers[d] = 0;
   int ok = execute(m);

   if (ok) {
      printf("part two: %d\n", m->registers[a]);
   } else {
      printf("part two: error\n");
   }


   fclose(ifile);
   return EXIT_SUCCESS;
}
