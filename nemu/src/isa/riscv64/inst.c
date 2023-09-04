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
// 不知道为什么当我以前一样把*所有的dest换为rd就会报错
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
  TYPE_J,
  TYPE_N, // none
  TYPE_B,
};
// 为什么这一堆是* ，这里的*不是decode_exec里边的值
// 还有就是*imm这些东西是decode_operand里边的
// src1是src2是地址，因为这里是局部变量不是环境变量，*是值，R函数是获得值，rs1是一个二进制的值对应的整数
#define src1R() do { *src1 = R(rs1); } while (0)
#define src2R() do { *src2 = R(rs2); } while (0)
#define immI() do { *imm = SEXT(BITS(i, 31, 20), 12); } while(0)
#define immU() do { *imm = SEXT(BITS(i, 31, 12), 20) << 12; } while(0)
#define immS() do { *imm = (SEXT(BITS(i, 31, 25), 7) << 5) | BITS(i, 11, 7); } while(0)
// #define immJ() do { *imm = (SEXT(BITS(i, 30, 21), 10) << 1 | BITS(i, 19, 12) << 12 | BITS(i,31,31) << 20 | BITS(i,20,20) << 11);} while (0)
#define immB() do { *imm = (SEXT(BITS(i, 31, 31), 1) << 12) | BITS(i, 7, 7) << 11 | BITS(i, 30, 25) << 5 | BITS(i, 11, 8) << 1; } while(0)
#define immJ() do {  *imm =(SEXT(BITS(i, 30, 21),21) << 1) | (BITS(i, 31, 31) << 20) | (BITS(i, 19, 12) << 12) | (BITS(i, 20, 20) << 11)  ;} while(0)

// rd是个地址，rs1一个变量
//static void decode_operand(Decode *s, int *rd, word_t *src1, word_t *src2, word_t *imm, int type) {


static void decode_operand(Decode *s, int *dest, word_t *src1, word_t *src2, word_t *imm, int type) {
  uint32_t i = s->isa.inst.val;
  // 这就是把不同的部分摘出来，其中i是这一条运行的指令
  int rs1 = BITS(i, 19, 15);
  int rs2 = BITS(i, 24, 20);
  // *rd     = BITS(i, 11, 7);
  int rd = BITS(i, 11,7);
  *dest = rd;
  switch (type) {
    case TYPE_I: src1R();          immI(); break;
    case TYPE_U:                   immU(); break;
    case TYPE_S: src1R(); src2R(); immS(); break;
    case TYPE_J: immJ(); break;
    case TYPE_B: src1R(); src2R(); immB(); break;

  }
}

static int decode_exec(Decode *s) {
  // int rd = 0;

  int dest = 0;

  word_t src1 = 0, src2 = 0, imm = 0;
  s->dnpc = s->snpc;
// 这是个获值
#define INSTPAT_INST(s) ((s)->isa.inst.val)
// rd,src1,src2是什么不重要，因为他们只是用来赋值的
// #define myprintf(...) printf( __VA_ARGS__)
// 总体来说就是将左边宏中 .. 的内容原样抄写在右边 __VA_ARGS__ 所在的位置
// https://zhuanlan.zhihu.com/p/410584385
// 只支持字符串，不支持可变参数或者多个参数
// 只能是一些不含任何变量的字符串常量
//错误写法：只支持字符串，不支持可变参数或者多个参数
//   LOGFUNC("%d,%d",1,2); //编译报错
// #define LOGSTRINGS(fm, ...) printf(fm,__VA_ARGS__)
// LOGSTRINGS("0123456789,%d%s",1,"sd");
// 这里是将可变的参数输出了

//  decode_operand(s, &rd, &src1, &src2, &imm, concat(TYPE_, type)); \

#define INSTPAT_MATCH(s, name, type, ... /* execute body */ ) { \
  decode_operand(s, &dest, &src1, &src2, &imm, concat(TYPE_, type)); \
  __VA_ARGS__ ; \
}

  INSTPAT_START();
  INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc  , U, R(dest) = s->pc + imm);
  INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu    , I, R(dest) = Mr(src1 + imm, 1));
  INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb     , S, Mw(src1 + imm, 1, src2));

  INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, NEMUTRAP(s->pc, R(10))); // R(10) is $a0
  INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi   , I, R(dest) = src1 + imm);
  INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal    , J, R(dest) = s->pc +4, s->pc += imm);
  INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc));

  // INSTPAT("0000000 ????? ????? 111 ????? 01100 11", and    , R, R(dest) = src1 & src2);

// INSTPAT("0000001 ????? ????? 000 ????? 01100 11", mul    , R, R(dest) = src1 * src2);
// INSTPAT("0000001 ????? ????? 000 ????? 01110 11", mulw   , R, R(dest) = SEXT(src1 * src2, 32));

  /*N-type*/
  // INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc  , U, R(dest) = s->pc + imm);
  // INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, R(dest) = s->pc + 4; s->dnpc = (src1 + imm) & ~1);
  // INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal    , J, R(dest) = s->pc + 4; s->dnpc = s->pc + imm);
  // INSTPAT("??????? ????? ????? 011 ????? 01000 11", sd     , S, Mw(src1 + imm, 8, src2));

  // INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi   , I, R(dest) = src1 + imm);
  // INSTPAT("??????? ????? ????? 011 ????? 00000 11", ld     , I, R(dest) = Mr(src1 + imm, 8));
  // INSTPAT("??????? ????? ????? 011 ????? 01000 11", sd     , S, Mw(src1 + imm, 8, src2));
  // // INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, R(dest) = s->pc + 4; s->dnpc = (src1 + imm) & ~1);
  // // INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, NEMUTRAP(s->pc, R(10))); // R(10) is $a0
  // INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal    , J, R(dest) = s->pc + 4; s->dnpc = s->pc; s->dnpc += imm);
  // INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, s->dnpc = (src1 + imm) & ~(word_t)1; R(dest) = s->pc + 4);
  // INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc  , U, R(dest) = s->pc + imm);
  // INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc));

//   INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi   , I, R(dest) = src1 + imm);
//   INSTPAT("??????? ????? ????? 000 ????? 00110 11", addw   , I, R(dest) = SEXT(src1 + imm, 32));
//   INSTPAT("??????? ????? ????? 111 ????? 00100 11", andi   , I, R(dest) = src1 & imm);
//   INSTPAT("??????? ????? ????? 110 ????? 00100 11", ori    , I, R(dest) = src1 | imm);
//   INSTPAT("000000? ????? ????? 001 ????? 00100 11", slli   , I, R(dest) = src1 << (imm & 0x3f));
//   INSTPAT("000000? ????? ????? 001 ????? 00110 11", slliw  , I, R(dest) = SEXT(src1 << (imm & 0x3f), 32));
//   INSTPAT("010000? ????? ????? 101 ????? 00100 11", srai   , I, R(dest) = (sword_t)src1 >> (imm & 0x3f));
//   INSTPAT("010000? ????? ????? 101 ????? 00110 11", sraiw  , I, R(dest) = SEXT((int32_t)src1 >> (imm & 0x3f), 32));
//   INSTPAT("000000? ????? ????? 101 ????? 00100 11", srli   , I, R(dest) = src1 >> (imm & 0x3f));
//   INSTPAT("000000? ????? ????? 101 ????? 00110 11", srliw  , I, R(dest) = SEXT((uint32_t)src1 >> (imm & 0x3f), 32));
//   INSTPAT("??????? ????? ????? 010 ????? 00100 11", slti   , I, R(dest) = (sword_t)src1 < (sword_t)imm);
//   INSTPAT("??????? ????? ????? 011 ????? 00100 11", sltiu  , I, R(dest) = (word_t)src1 < (word_t)imm);
//   INSTPAT("??????? ????? ????? 100 ????? 00100 11", xori   , I, R(dest) = src1 ^ imm);

//   INSTPAT("??????? ????? ????? 000 ????? 00000 11", lb     , I, R(dest) = SEXT((uint8_t)Mr(src1 + imm, 1), 8));
//   INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu    , I, R(dest) = Mr(src1 + imm, 1));
//   INSTPAT("??????? ????? ????? 001 ????? 00000 11", lh     , I, R(dest) = SEXT((uint16_t)Mr(src1 + imm, 2), 16));
//   INSTPAT("??????? ????? ????? 101 ????? 00000 11", lhu    , I, R(dest) = Mr(src1 + imm, 2));
//   INSTPAT("??????? ????? ????? 010 ????? 00000 11", lw     , I, R(dest) = SEXT((uint32_t)Mr(src1 + imm, 4), 32));
//   INSTPAT("??????? ????? ????? 110 ????? 00000 11", lwu    , I, R(dest) = Mr(src1 + imm, 4));
//   INSTPAT("??????? ????? ????? 011 ????? 00000 11", ld     , I, R(dest) = Mr(src1 + imm, 8));

//   //ecall
  
//   INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, NEMUTRAP(s->pc, R(10))); // R(10) is $a0
//   INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc));

//   /*U-type*/
//   INSTPAT("??????? ????? ????? ??? ????? 01101 11", lui    , U, R(dest) = imm);
  
//   /*S-type*/
//   INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb     , S, Mw(src1 + imm, 1, (uint8_t)src2));
//   INSTPAT("??????? ????? ????? 001 ????? 01000 11", sh     , S, Mw(src1 + imm, 2, (uint16_t)src2));
//   INSTPAT("??????? ????? ????? 010 ????? 01000 11", sw     , S, Mw(src1 + imm, 4, (uint32_t)src2));

//  /*B-type*/
//   INSTPAT("??????? ????? ????? 000 ????? 11000 11", beq    , B, s->dnpc = (src1 == src2 ? s->pc + imm: s->dnpc));
//   INSTPAT("??????? ????? ????? 101 ????? 11000 11", bge    , B, s->dnpc = ((sword_t)src1 >= (sword_t)src2 ? s->pc + imm: s->dnpc));
//   INSTPAT("??????? ????? ????? 111 ????? 11000 11", bgeu   , B, s->dnpc = (src1 >= src2 ? s->pc + imm: s->dnpc));
//   INSTPAT("??????? ????? ????? 100 ????? 11000 11", blt    , B, s->dnpc = ((sword_t)src1 < (sword_t)src2 ? s->pc + imm: s->dnpc));
//   INSTPAT("??????? ????? ????? 110 ????? 11000 11", bltu   , B, s->dnpc = (src1 < src2 ? s->pc + imm: s->dnpc));
//   INSTPAT("??????? ????? ????? 001 ????? 11000 11", bne    , B, s->dnpc = (src1 != src2 ? s->pc + imm: s->dnpc));

  /*J-type*/
  
  /*RV64V*/
  // INSTPAT("??????? ????? ????? 000 ????? 00001 11", vld    , N, (vld(s)));
  // INSTPAT("??????? ????? ????? 101 ????? 00001 11", vld    , N, (vld(s)));
  // INSTPAT("??????? ????? ????? 110 ????? 00001 11", vld    , N, (vld(s)));
  // INSTPAT("??????? ????? ????? 111 ????? 00001 11", vld    , N, (vld(s)));
  
  // INSTPAT("??????? ????? ????? 000 ????? 01001 11", vsd    , N, (vsd(s)));
  // INSTPAT("??????? ????? ????? 101 ????? 01001 11", vsd    , N, (vsd(s)));
  // INSTPAT("??????? ????? ????? 110 ????? 01001 11", vsd    , N, (vsd(s)));
  // INSTPAT("??????? ????? ????? 111 ????? 01001 11", vsd    , N, (vsd(s)));

  // INSTPAT("001000? ????? ????? 000 ????? 10101 11", vgtm   , N, (vgtm(s)));
  // INSTPAT("001000? ????? ????? 011 ????? 10101 11", vgtm   , N, (vgtm(s)));
  // INSTPAT("001000? ????? ????? 100 ????? 10101 11", vgtm   , N, (vgtm(s))); 
  
  // INSTPAT("001100? ????? ????? 000 ????? 10101 11", guv   , N, (guv(s)));
  // INSTPAT("001101? ????? ????? 011 ????? 10101 11", guv   , N, (guv(s)));
  // INSTPAT("001101? ????? ????? 100 ????? 10101 11", guv   , N, (guv(s)));
  
  // INSTPAT("??????? ????? ????? ??? ????? 00010 11", rvmc  , N, (rvmc_decode_exec(s)));
 /*RV64V*/
  

  
  
  INSTPAT_END();

  R(0) = 0; // reset $zero to 0

  return 0;
}
// 什么是运行一条指令
// 首先是获得值
// 
int isa_exec_once(Decode *s) {
  s->isa.inst.val = inst_fetch(&s->snpc, 4);
  return decode_exec(s);
}
