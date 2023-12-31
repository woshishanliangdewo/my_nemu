/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>
// 这个是对应的寄存器的值
#define R(i) gpr(i)
#define Mr vaddr_read
#define Mw vaddr_write

enum {
  TYPE_I, TYPE_U, TYPE_S,
  TYPE_N, // none
  TYPE_J
};

#define src1R() do { *src1 = R(rs1); } while (0)
#define src2R() do { *src2 = R(rs2); } while (0)
#define immI() do { *imm = SEXT(BITS(i, 31, 20), 12); } while(0)
#define immU() do { *imm = SEXT(BITS(i, 31, 12), 20) << 12; } while(0)
#define immS() do { *imm = (SEXT(BITS(i, 31, 25), 7) << 5) | BITS(i, 11, 7); } while(0)
#define immJ() do { *imm = (SEXT(BITS(i, 30, 21), 10) << 1 | BITS(i, 19, 12) << 12 | BITS(i,31,31) << 20 | BITS(i,20,20) << 11);} while (0)

static void decode_operand(Decode *s, int *rd, word_t *src1, word_t *src2, word_t *imm, int type) {
  uint32_t i = s->isa.inst.val;
  // 这就是把不同的部分摘出来，其中i是这一条运行的指令
  int rs1 = BITS(i, 19, 15);
  int rs2 = BITS(i, 24, 20);
  *rd     = BITS(i, 11, 7);
  switch (type) {
    case TYPE_I: src1R();          immI(); break;
    case TYPE_U:                   immU(); break;
    case TYPE_S: src1R(); src2R(); immS(); break;
    case TYPE_J: immJ(); break;
  }
}

static int decode_exec(Decode *s) {
  int rd = 0;
  int temp = 0;
  word_t src1 = 0, src2 = 0, imm = 0;
  s->dnpc = s->snpc;
// 这是个获值
#define INSTPAT_INST(s) ((s)->isa.inst.val)
// rd,src1,src2是什么不重要，因为他们只是用来赋值的
#define INSTPAT_MATCH(s, name, type, ... /* execute body */ ) { \
  decode_operand(s, &rd, &src1, &src2, &imm, concat(TYPE_, type)); \
  __VA_ARGS__ ; \
}

  INSTPAT_START();
  INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc  , U, R(rd) = s->pc + imm);
  INSTPAT("??????? ????? ????? 011 ????? 00000 11", ld     , I, R(rd) = Mr(src1 + imm, 8));
  INSTPAT("??????? ????? ????? 011 ????? 01000 11", sd     , S, Mw(src1 + imm, 8, src2));
  INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, NEMUTRAP(s->pc, R(10))); // R(10) is $a0
  INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc));
  INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi   , I, R(rd) = R(src1) + imm);
  INSTPAT("??????? ????1 1???? ??? ????? 11011 11", jal    , J, R(rd) = s->pc + 4 , s->pc+=imm);
  INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, temp = s->pc + 4, s->pc = (R(src1) + imm) & (~1), R(rd) = temp);
  // INSTPAT("??????? ????? ????? 000 ????? 00100 11", li   , I, R(rd) =  imm);

  // INSTPAT("ret");
  // INSTPAT("mv");
  // INSTPAT("j");
  INSTPAT_END();

  R(0) = 0; // reset $zero to 0

  return 0;
}

int isa_exec_once(Decode *s) {
  s->isa.inst.val = inst_fetch(&s->snpc, 4);
  return decode_exec(s);
}
