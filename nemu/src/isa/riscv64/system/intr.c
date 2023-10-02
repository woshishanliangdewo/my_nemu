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

#include <isa.h>
typedef struct{
  word_t mtvec;
  word_t mepc;
  word_t mstatus;
  word_t mcause;
}riscv64_CSR;

typedef struct{
  riscv64_CSR* csr;
  word_t gpr[32];
  vaddr_t pc;
}riscv64_CPU_STATE;

riscv64_CPU_STATE riscv64_cpu;

// 为了实现自陷,我需要实现这个函数
word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  riscv64_cpu.csr->mcause = NO;
  riscv64_cpu.csr->mepc = epc;
  return riscv64_cpu.csr->mtvec;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
