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
#define CSR(i) *csr_register(i)


static vaddr_t *csr_register(word_t imm) {
  switch (imm)
  {
  case 0x341: return &(cpu.csr.mepc);
  case 0x342: return &(cpu.csr.mcause);
  case 0x300: return &(cpu.csr.mstatus);
  case 0x305: return &(cpu.csr.mtvec);
  default: panic("Unknown csr");
  }
}

enum {
  TYPE_I, TYPE_U, TYPE_S,
  TYPE_J,
  TYPE_N, // none
  TYPE_B,
  TYPE_R,
};
// 为什么这一堆是* ，这里的*不是decode_exec里边的值
// 还有就是*imm这些东西是decode_operand里边的
// src1是src2是地址，因为这里是局部变量不是环境变量，*是值，R函数是获得值，rs1是一个二进制的值对应的整数
// https://blog.csdn.net/weixin_48896613/article/details/127371045
#define src1R() do { *src1 = R(rs1); } while (0)
#define src2R() do { *src2 = R(rs2); } while (0)
#define immI() do { *imm = SEXT(BITS(i, 31, 20), 12); } while(0)
#define immU() do { *imm = SEXT(BITS(i, 31, 12), 20) << 12; } while(0)
#define immS() do { *imm = (SEXT(BITS(i, 31, 25), 7) << 5) | BITS(i, 11, 7); } while(0)
// #define immJ() do { *imm = (SEXT(BITS(i, 30, 21), 10) << 1 | BITS(i, 19, 12) << 12 | BITS(i,31,31) << 20 | BITS(i,20,20) << 11);} while (0)
#define immB() do { *imm = (SEXT(BITS(i, 31, 31), 1) << 12) | BITS(i, 7, 7) << 11 | BITS(i, 30, 25) << 5 | BITS(i, 11, 8) << 1; } while(0)
// #define immJ() do {  *imm =(SEXT(BITS(i, 30, 21),21) << 1) | (BITS(i, 31, 31) << 20) | (BITS(i, 19, 12) << 12) | (BITS(i, 20, 20) << 11)  ;} while(0)
#define immJ() do { *imm = (SEXT(BITS(i, 31, 31), 1) << 20) | BITS(i, 30, 21) << 1 \
                          | BITS(i, 20, 20) << 11 | BITS(i, 19, 12) << 12 ; } while(0)

// rd是个地址，rs1一个变量
//static void decode_operand(Decode *s, int *rd, word_t *src1, word_t *src2, word_t *imm, int type) {
// #define immJ() do { *imm = SEXT(( \
// (BITS(i, 31, 31) << 19) | \
// BITS(i, 30, 21) | \
// (BITS(i, 20, 20) << 10) | \
// (BITS(i, 19, 12) << 11) \
// ) << 1, 21); Log(ANSI_FG_CYAN "%#lx\n" ANSI_NONE, *imm); } while(0)
word_t isa_raise_intr(word_t NO, vaddr_t epc);
word_t isa_reg_str2val(const char *s, bool *success);
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
    case TYPE_R: src1R(); src2R();         break;
    // case TYPE_R: 
  }
}

static int decode_exec(Decode *s) {
  // int rd = 0;

  int dest = 0;

  word_t src1 = 0, src2 = 0, imm = 0;
  s->dnpc = s->snpc;
// 这是个获值
#define INSTPAT_INST(s) ((s)->isa.inst.val)
// #define MAYBE_FUNC_JAL(s) IFDEF(CONFIG_ITRACE, { \
//   if (dest == 1) { \
//     trace_func_call(s->pc, s->dnpc, false); \
//   } \
// })
// #define MAYBE_FUNC_JALR(s) IFDEF(CONFIG_ITRACE, { \
//     if (s->isa.inst.val == 0x00008067) { \
//       trace_func_ret(s->pc); \
//     } else if (dest == 1) { \
//       trace_func_call(s->pc, s->dnpc, false); \
//     } else if (dest == 0 && imm == 0) { \
//       trace_func_call(s->pc, s->dnpc, true); \
//     } \
//   })
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

// 使用了INSTPAT_MATCH，就会使用decode_operand
// decode_operand就会变成
//   uint32_t i = s->isa.inst.val;
//   int rs1 = BITS(i, 19, 15);
//   int rs2 = BITS(i, 24, 20);
//   int rd = BITS(i, 11,7);
//   *dest = rd;
//   switch (type) {
//     case TYPE_I: src1R();          immI(); break;
//     case TYPE_U:                   immU(); break;
//     case TYPE_S: src1R(); src2R(); immS(); break;
//     case TYPE_J: immJ(); break;
//     case TYPE_B: src1R(); src2R(); immB(); break;
//     // case TYPE_R: 
//   }
// }
#define INSTPAT_MATCH(s, name, type, ... /* execute body */ ) { \
  decode_operand(s, &dest, &src1, &src2, &imm, concat(TYPE_, type)); \
  __VA_ARGS__ ; \
}

// 这里是指令开始了
// 其中的变量是name
// 指令的开始  __instpat_end = &&__instpat_end_name
// 指令的结束  __instpat_end_name:
// 这里就是    __instpat_end = &&__instpat_end_;
//            __instpat_end_: ;
  INSTPAT_START();
// 这里是
// uint64_t key, mask, shift; \
//   pattern_decode(pattern, STRLEN(pattern), &key, &mask, &shift); \
//   if ((((uint64_t)INSTPAT_INST(s) >> shift) & mask) == key) { \
//     INSTPAT_MATCH(s, ##__VA_ARGS__); \
//     goto *(__instpat_end); \
//   } \
// } while (0)

// 核心部分替换：
// pattern_decode(pattern, STRLEN(pattern), &key, &mask, &shift);
// if ((((uint64_t)INSTPAT_INST(s) >> shift) & mask) == key) {
// INSTPAT_MATCH(s, ##__VA_ARGS__); 
//     goto *(__instpat_end); 
//  }
// } while (0)

// 会变为
// uint64_t __key = 0, __mask = 0, __shift = 0;
// #define macro(i) \
//   if ((i) >= len) goto finish; \
//   else { \
//     char c = str[i]; \
//     if (c != ' ') { \
//       Assert(c == '0' || c == '1' || c == '?', \
//           "invalid character '%c' in pattern string", c); \
//       __key  = (__key  << 1) | (c == '1' ? 1 : 0); \
//       __mask = (__mask << 1) | (c == '?' ? 0 : 1); \
//       __shift = (c == '?' ? __shift + 1 : 0); \
//     } \
//   }

// 以及
// 
  // INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc  , U, R(dest) = s->pc + imm);
  // INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu    , I, R(dest) = Mr(src1 + imm, 1));
  // INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb     , S, Mw(src1 + imm, 1, src2));
  // INSTPAT("0000000 ????? ????? 000 ????? 01100 11", add    , R, R(dest) = src1 + src2);
  // INSTPAT("??????? ????? ????? 000 ????? 00110 11", addiw  , I, R(dest) = src1 + imm);
  // INSTPAT("0100000 ????? ????? 000 ????? 01100 11", sub    , R, R(dest) = src1 - src2);
  // INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, NEMUTRAP(s->pc, R(10))); // R(10) is $a0
  // INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi   , I, R(dest) = src1 + imm);
  // INSTPAT("??????? ????? ????? 001 ????? 11100 11", csrrw  , I, R(dest) = CSR(imm);CSR(imm) = src1);
  // INSTPAT("??????? ????? ????? 110 ????? 11000 11", bltu   , B, if(src1<src2) s->dnpc= s->pc +imm);
  // INSTPAT("0000000 ????? ????? 101 ????? 00110 11", srliw  , I, R(dest) = BITS(src1,31,0) >> imm);
  // // INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal    , J, R(dest) = s->pc +4, s->dnpc = s->pc + imm);
  // // INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal    , J, R(dest) = s->pc +4, s->dnpc = s->pc + imm,printf("\n");printf("0x%x\n",imm);printf("0x%x\n",s->pc););
  // INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal    , J, R(dest) = s->snpc; s->dnpc = s->pc + imm);    // 要更新动态pc
  // // INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, R(dest)= s->pc+4,s->pc =(src1+imm)&~(word_t)1);
  // INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, R(dest) = s->snpc; s->dnpc = src1 + imm);  // 也即ret
  // // INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, s->dnpc =(src1+imm)&~(word_t)1, R(dest)= s->pc+4);
  // INSTPAT("??????? ????? ????? 011 ????? 01000 11", sd     , S, Mw(src1 + imm, 8, src2));
  // INSTPAT("??????? ????? ????? 010 ????? 00000 11", lw     , I, R(dest) = Mr(src1 + imm, 4));
  // INSTPAT("??????? ????? ????? 011 ????? 00000 11", ld     , I, R(dest) = Mr(src1 + imm, 8));
  // INSTPAT("??????? ????? ????? 000 ????? 11000 11", beq    , B, if (src1 == src2) s->dnpc = s->pc + imm);
  // INSTPAT("000000? ????? ????? 001 ????? 00100 11", slli   , I, R(dest) = src1<<imm);
  // INSTPAT("??????? ????? ????? 111 ????? 00100 11", andi   , I, R(dest) = src1&imm);
  // INSTPAT("0000000 00000 00000 000 00000 11100 11", ecall  , I, bool success; s->dnpc = isa_raise_intr(isa_reg_str2val("a7",&success),s->pc));
  // // INSTPAT("0100000 ????? ????? 000 ????? 01110 11", subw   , R, R(dest) = (src1-src2)&(0x0000000011111111));
  // INSTPAT("??????? ????? ????? 010 ????? 01000 11", sw     , S, Mw(src1 + imm, 4 , src2));
  // INSTPAT("??????? ????? ????? 001 ????? 11000 11", bne    , B, if(src1!=src2) s->dnpc = s->pc + imm);
  // INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc));
INSTPAT("0000000 ????? ????? 111 ????? 01100 11", and    , R, R(dest) = src1 & src2);
INSTPAT("??????? ????? ????? 111 ????? 00100 11", andi   , I, R(dest) = src1 & imm);
INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc  , U, R(dest) = s->pc + imm);
INSTPAT("0000000 ????? ????? 000 ????? 01100 11", add    , R, R(dest) = src1 + src2);
INSTPAT("0000000 ????? ????? 000 ????? 01110 11", addw   , R, R(dest) = SEXT(src1 + src2, 32));
INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi   , I, R(dest) = src1 + imm);
INSTPAT("??????? ????? ????? 000 ????? 00110 11", addiw  , I, R(dest) = SEXT(src1 + imm, 32));
INSTPAT("??????? ????? ????? 000 ????? 11000 11", beq    , B, if (src1 == src2) s->dnpc = s->pc + imm);
INSTPAT("??????? ????? ????? 001 ????? 11000 11", bne    , B, if (src1 != src2) s->dnpc = s->pc + imm);
INSTPAT("??????? ????? ????? 100 ????? 11000 11", blt    , B, if ((sword_t)src1 < (sword_t)src2) s->dnpc = s->pc + imm);
INSTPAT("??????? ????? ????? 110 ????? 11000 11", bltu   , B, if (src1 < src2) s->dnpc = s->pc + imm);
INSTPAT("??????? ????? ????? 111 ????? 11000 11", bgeu   , B, if (src1 >= src2) s->dnpc = s->pc + imm);
INSTPAT("??????? ????? ????? 101 ????? 11000 11", bge    , B, if ((sword_t)src1 >= (sword_t)src2) s->dnpc = s->pc + imm);
INSTPAT("0000001 ????? ????? 100 ????? 01110 11", divw   , R, R(dest) = SEXT(src1, 32) / SEXT(src2, 32));
INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal    , J, s->dnpc = s->pc; s->dnpc += imm; R(dest) = s->pc + 4);
INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, s->dnpc = (src1 + imm) & ~(word_t)1; R(dest) = s->pc + 4);
INSTPAT("??????? ????? ????? ??? ????? 01101 11", lui    , U, R(dest) = imm);
INSTPAT("??????? ????? ????? 000 ????? 00000 11", lb     , I, R(dest) = SEXT(Mr(src1 + imm, 1), 8));
INSTPAT("??????? ????? ????? 001 ????? 00000 11", lh     , I, R(dest) = SEXT(Mr(src1 + imm, 2), 16));
INSTPAT("??????? ????? ????? 010 ????? 00000 11", lw     , I, R(dest) = SEXT(Mr(src1 + imm, 4), 32));
INSTPAT("??????? ????? ????? 011 ????? 00000 11", ld     , I, R(dest) = Mr(src1 + imm, 8));
INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu    , I, R(dest) = Mr(src1 + imm, 1));
INSTPAT("??????? ????? ????? 101 ????? 00000 11", lhu    , I, R(dest) = Mr(src1 + imm, 2));
INSTPAT("??????? ????? ????? 110 ????? 00000 11", lwu    , I, R(dest) = Mr(src1 + imm, 4));
INSTPAT("0000001 ????? ????? 000 ????? 01100 11", mul    , R, R(dest) = src1 * src2);
INSTPAT("0000001 ????? ????? 000 ????? 01110 11", mulw   , R, R(dest) = SEXT(src1 * src2, 32));
INSTPAT("??????? ????? ????? 100 ????? 00100 11", xori   , I, R(dest) = src1 ^ imm);
INSTPAT("0000000 ????? ????? 110 ????? 01100 11", or     , R, R(dest) = src1 | src2);
INSTPAT("0000001 ????? ????? 110 ????? 01110 11", remw   , R, R(dest) = SEXT(src1, 32) % SEXT(src2, 32));
INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb     , S, Mw(src1 + imm, 1, src2));
INSTPAT("??????? ????? ????? 001 ????? 01000 11", sh     , S, Mw(src1 + imm, 2, src2));
INSTPAT("??????? ????? ????? 010 ????? 01000 11", sw     , S, Mw(src1 + imm, 4, src2));
INSTPAT("??????? ????? ????? 011 ????? 01000 11", sd     , S, Mw(src1 + imm, 8, src2));
INSTPAT("??????? ????? ????? 011 ????? 00100 11", sltiu  , I, R(dest) = src1 < imm );
INSTPAT("0000000 ????? ????? 011 ????? 01100 11", sltu   , R, R(dest) = src1 < src2);
// warn: "right shift a negative number"'s behavior(arithmetic or logical) depends on the compiler
INSTPAT("000000? ????? ????? 101 ????? 00110 11", srliw  , I, R(dest) = BITS(src1, 31, 0) >> BITS(imm, 4, 0));
INSTPAT("000000? ????? ????? 001 ????? 00100 11", slli   , I, R(dest) = src1 << BITS(imm, 5, 0));
INSTPAT("000000? ????? ????? 001 ????? 00110 11", slliw  , I, R(dest) = SEXT(src1 << BITS(imm, 4, 0), 32));
INSTPAT("0000000 ????? ????? 001 ????? 01110 11", sllw   , R, R(dest) = SEXT(src1 << BITS(src2, 4, 0), 32));
INSTPAT("000000? ????? ????? 101 ????? 00100 11", srli   , I, R(dest) = src1 >> BITS(imm, 5, 0)); 
INSTPAT("0000000 ????? ????? 101 ????? 01110 11", srlw   , R, R(dest) = SEXT(BITS(src1, 31, 0) >> BITS(src2, 4, 0), 32));
INSTPAT("0100000 ????? ????? 101 ????? 01110 11", sraw   , R, R(dest) = (sword_t)SEXT(src1, 32) >> BITS(src2, 4, 0));
INSTPAT("010000? ????? ????? 101 ????? 00100 11", srai   , I, R(dest) = (sword_t)src1 >> BITS(imm, 5, 0));
INSTPAT("010000? ????? ????? 101 ????? 00110 11", sraiw  , I, R(dest) = (sword_t)SEXT(src1, 32) >> BITS(imm, 4, 0));
INSTPAT("0000000 ????? ????? 010 ????? 01100 11", slt    , R, R(dest) = (sword_t)src1 < (sword_t)src2);
INSTPAT("0100000 ????? ????? 000 ????? 01100 11", sub    , R, R(dest) = src1 - src2);
INSTPAT("0100000 ????? ????? 000 ????? 01110 11", subw   , R, R(dest) = SEXT(src1 - src2, 32));
INSTPAT("0000000 ????? ????? 100 ????? 01100 11", xor    , R, R(dest) = src1 ^ src2);
// INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb     , S, Mw(/*src1 + imm*/0x8fffffff, 1, src2));


INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, NEMUTRAP(s->pc, R(10))); // R(10) is $a0
INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc));
  
  INSTPAT_END();

  R(0) = 0; // reset $zero to 0

  return 0;
}
// 什么是运行一条指令
// 首先是获得值
// 
void insertRb(word_t pc, u_int32_t inst);
int isa_exec_once(Decode *s) {
  s->isa.inst.val = inst_fetch(&s->snpc, 4);
  IFDEF(CONFIG_ITRACE, insertRb(s->pc, s->isa.inst.val));
  return decode_exec(s);
}
