
/*
 *      National Semiconductor Corporation CR16 processor module for IDA.
 *      Copyright (c) 2002-2006 Konstantin Norvatoff, <konnor@bk.ru>
 *      Freeware.
 */

#include "cr16.hpp"

// NB! word versions must follow byte versions
// this is done to simplify decoding in ana.c
instruc_t Instructions[] = {
  {"",            0                               },
  {"addb",        CF_USE1|CF_USE2|CF_CHG2         },
  {"addw",        CF_USE1|CF_USE2|CF_CHG2         },
  {"addub",       CF_USE1|CF_USE2|CF_CHG2         },
  {"adduw",       CF_USE1|CF_USE2|CF_CHG2         },
  {"addcb",       CF_USE1|CF_USE2|CF_CHG2         },
  {"addcw",       CF_USE1|CF_USE2|CF_CHG2         },
  {"andb",        CF_USE1|CF_USE2|CF_CHG2         },
  {"andw",        CF_USE1|CF_USE2|CF_CHG2         },
  {"ashub",       CF_USE1|CF_USE2|CF_CHG2|CF_SHFT },
  {"ashuw",       CF_USE1|CF_USE2|CF_CHG2|CF_SHFT },
  {"beq",         CF_USE1|CF_JUMP                 },
  {"bne",         CF_USE1|CF_JUMP                 },
  {"bcs",         CF_USE1|CF_JUMP                 },
  {"bcc",         CF_USE1|CF_JUMP                 },
  {"bhi",         CF_USE1|CF_JUMP                 },
  {"bls",         CF_USE1|CF_JUMP                 },
  {"bgt",         CF_USE1|CF_JUMP                 },
  {"ble",         CF_USE1|CF_JUMP                 },
  {"bfs",         CF_USE1|CF_JUMP                 },
  {"bfc",         CF_USE1|CF_JUMP                 },
  {"blo",         CF_USE1|CF_JUMP                 },
  {"bhs",         CF_USE1|CF_JUMP                 },
  {"blt",         CF_USE1|CF_JUMP                 },
  {"bge",         CF_USE1|CF_JUMP                 },
  {"br",          CF_USE1|CF_JUMP|CF_STOP         },
  {"bal",         CF_USE1|CF_CHG1|CF_USE2|CF_CALL },
  {"cmpb",        CF_USE1|CF_USE2                 },
  {"cmpw",        CF_USE1|CF_USE2                 },
  {"beq1b",       CF_USE1|CF_USE2|CF_JUMP         },
  {"beq1w",       CF_USE1|CF_USE2|CF_JUMP         },
  {"beq0b",       CF_USE1|CF_USE2|CF_JUMP         },
  {"beq0w",       CF_USE1|CF_USE2|CF_JUMP         },
  {"bne1b",       CF_USE1|CF_USE2|CF_JUMP         },
  {"bne1w",       CF_USE1|CF_USE2|CF_JUMP         },
  {"bne0b",       CF_USE1|CF_USE2|CF_JUMP         },
  {"bne0w",       CF_USE1|CF_USE2|CF_JUMP         },
  {"di",          0                               },
  {"ei",          0                               },
  {"excp",        CF_USE1                         },
  {"jeq",         CF_USE1|CF_JUMP                 },
  {"jne",         CF_USE1|CF_JUMP                 },
  {"jcs",         CF_USE1|CF_JUMP                 },
  {"jcc",         CF_USE1|CF_JUMP                 },
  {"jhi",         CF_USE1|CF_JUMP                 },
  {"jls",         CF_USE1|CF_JUMP                 },
  {"jgt",         CF_USE1|CF_JUMP                 },
  {"jle",         CF_USE1|CF_JUMP                 },
  {"jfs",         CF_USE1|CF_JUMP                 },
  {"jfc",         CF_USE1|CF_JUMP                 },
  {"jlo",         CF_USE1|CF_JUMP                 },
  {"jhs",         CF_USE1|CF_JUMP                 },
  {"jlt",         CF_USE1|CF_JUMP                 },
  {"jge",         CF_USE1|CF_JUMP                 },
  {"jump",        CF_USE1|CF_JUMP|CF_STOP         },
  {"jal",         CF_USE1|CF_CHG1|CF_USE2|CF_CALL },
  {"loadb",       CF_USE1|CF_USE2|CF_CHG2         },
  {"loadw",       CF_USE1|CF_USE2|CF_CHG2         },
  {"loadm",       CF_USE1                         },
  {"lpr",         CF_USE1|CF_USE2|CF_CHG2         },
  {"lshb",        CF_USE1|CF_USE2|CF_CHG2|CF_SHFT },
  {"lshw",        CF_USE1|CF_USE2|CF_CHG2|CF_SHFT },
  {"movb",        CF_USE1|CF_USE2|CF_CHG2         },
  {"movw",        CF_USE1|CF_USE2|CF_CHG2         },
  {"movxb",       CF_USE1|CF_USE2|CF_CHG2         },
  {"movzb",       CF_USE1|CF_USE2|CF_CHG2         },
  {"movd",        CF_USE1|CF_USE2|CF_CHG2         },
  {"mulb",        CF_USE1|CF_USE2|CF_CHG2         },
  {"mulw",        CF_USE1|CF_USE2|CF_CHG2         },
  {"mulsb",       CF_USE1|CF_USE2|CF_CHG2         },
  {"mulsw",       CF_USE1|CF_USE2|CF_CHG2         },
  {"muluw",       CF_USE1|CF_USE2|CF_CHG2         },
  {"nop",         0                               },
  {"orb",         CF_USE1|CF_USE2|CF_CHG2         },
  {"orw",         CF_USE1|CF_USE2|CF_CHG2         },
  {"push",        CF_USE1|CF_USE2                 },
  {"pop",         CF_USE1|CF_USE2|CF_CHG2         },
  {"popret",      CF_USE1|CF_USE2|CF_CHG2         },
  {"retx",        CF_STOP                         },
  {"seq",         CF_USE1|CF_CHG1                 },
  {"sne",         CF_USE1|CF_CHG1                 },
  {"scs",         CF_USE1|CF_CHG1                 },
  {"scc",         CF_USE1|CF_CHG1                 },
  {"shi",         CF_USE1|CF_CHG1                 },
  {"sls",         CF_USE1|CF_CHG1                 },
  {"sgt",         CF_USE1|CF_CHG1                 },
  {"sle",         CF_USE1|CF_CHG1                 },
  {"sfs",         CF_USE1|CF_CHG1                 },
  {"sfc",         CF_USE1|CF_CHG1                 },
  {"slo",         CF_USE1|CF_CHG1                 },
  {"shs",         CF_USE1|CF_CHG1                 },
  {"slt",         CF_USE1|CF_CHG1                 },
  {"sge",         CF_USE1|CF_CHG1                 },
  {"spr",         CF_USE1|CF_USE2|CF_CHG2         },
  {"storb",       CF_USE1|CF_USE2|CF_CHG2         },
  {"storw",       CF_USE1|CF_USE2|CF_CHG2         },
  {"storm",       CF_USE1                         },
  {"subb",        CF_USE1|CF_USE2|CF_CHG2         },
  {"subw",        CF_USE1|CF_USE2|CF_CHG2         },
  {"subcb",       CF_USE1|CF_USE2|CF_CHG2         },
  {"subcw",       CF_USE1|CF_USE2|CF_CHG2         },
  {"tbit",        CF_USE1|CF_USE2                 },
  {"tbitb",       CF_USE1|CF_USE2                 },
  {"tbitw",       CF_USE1|CF_USE2                 },
  {"sbitb",       CF_USE1|CF_USE2                 },
  {"sbitw",       CF_USE1|CF_USE2                 },
  {"cbitb",       CF_USE1|CF_USE2|CF_CHG2         },
  {"cbitw",       CF_USE1|CF_USE2|CF_CHG2         },
  {"wait",        0                               },
  {"eiwait",      0                               },
  {"xorb",        CF_USE1|CF_USE2|CF_CHG2         },
  {"xorw",        CF_USE1|CF_USE2|CF_CHG2         }
};

CASSERT(qnumber(Instructions) == CR16_last);
