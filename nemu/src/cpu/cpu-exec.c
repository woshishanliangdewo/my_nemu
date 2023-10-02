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

#include <cpu/cpu.h>
#include <cpu/decode.h>
#include <cpu/difftest.h>
#include <locale.h>

/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INST_TO_PRINT 10
#define NR_WP 32
CPU_state cpu = {};
uint64_t g_nr_guest_inst = 0;
static uint64_t g_timer = 0; // unit: us
static bool g_print_step = false;

void device_update();
// 跟踪并且进行差分测试
// 一个解码和一个动态pc
static void trace_and_difftest(Decode *_this, vaddr_t dnpc) {
#ifdef CONFIG_ITRACE_COND
  if (ITRACE_COND) { log_write("%s\n", _this->logbuf); }
#endif
// 这里就是了，g_print_step，这是一个bool，而且初始化为false
  if (g_print_step) { IFDEF(CONFIG_ITRACE, puts(_this->logbuf)); }
  IFDEF(CONFIG_DIFFTEST, difftest_step(_this->pc, dnpc));
  // int i = check_wp();
  // if(i != -1)
  // {
  //   nemu_state.state = NEMU_STOP;
  //   printf("你触发了监控点%d",i);
  //   return ;
  // }
  
}
static void exec_once(Decode *s, vaddr_t pc) {
  s->pc = pc;
  s->snpc = pc;
  // 执行命令
  isa_exec_once(s);
  // 指令改变
  cpu.pc = s->dnpc;
  // 经过上一条代码，snpc将会变为pc+1，因为他记录的是一共运行了几步了
  // dnpc是实际下一条要运行什么，赋值给pc了
#ifdef CONFIG_ITRACE
     
  char *p = s->logbuf;
  p += snprintf(p, sizeof(s->logbuf), FMT_WORD ":", s->pc);
  int ilen = s->snpc - s->pc;
  int i;
  uint8_t *inst = (uint8_t *)&s->isa.inst.val;
  for (i = ilen - 1; i >= 0; i --) {
    p += snprintf(p, 4, " %02x", inst[i]);
  }
  
  int ilen_max = MUXDEF(CONFIG_ISA_x86, 8, 4);
  int space_len = ilen_max - ilen;
  if (space_len < 0) space_len = 0;
  space_len = space_len * 3 + 1;
  memset(p, ' ', space_len);
  p += space_len;

#ifndef CONFIG_ISA_loongarch32r
  void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
  disassemble(p, s->logbuf + sizeof(s->logbuf) - p,
      MUXDEF(CONFIG_ISA_x86, s->snpc, s->pc), (uint8_t *)&s->isa.inst.val, ilen);
#else
  p[0] = '\0'; // the upstream llvm does not support loongarch32r
#endif
#endif
}
// 所谓执行就是在这里
// 一共是n步，在这n步里，我们先执行一次pc的地址，若是，则g_nr_guest_inst（我暂且猜测是指令执行数）加一，
// 同时对pc的值进行trace， 同时执行之后我们看
// 状态是否是运行态，如果是的话，就停止，
// #define IFDEF(macro, ...) MUXDEF(macro, __KEEP, __IGNORE)(__VA_ARGS__)
// n如果一直小于0，他就会一直循环运作
// 然后这里的state一旦是running中，那么就在进行一次device_update
// 在设备的更新时，我们首先获得了一个last时间
// 然后我们有获得了一个now，也就是现在的时间戳
// 如果每一次的时间戳小于（1000000是一个微妙，转化为秒）
// 然后TIMER_HZ表示每秒钟的更新次数（或者说每秒钟的帧数）
// 得出的结果就是每次更新应该的时间差，可以防止过于频繁的更新
// 于是此时如果更新成功了，那么我们就可以更新vga屏幕了
static void execute(uint64_t n) {
  Decode s;
  for (;n > 0; n --) {
    // 执行一次
    exec_once(&s, cpu.pc);
    // 记录客户指令的计数器
    g_nr_guest_inst ++;

    trace_and_difftest(&s, cpu.pc);
    if (nemu_state.state != NEMU_RUNNING) break;
    IFDEF(CONFIG_DEVICE, device_update());
  }
}
// 这就是statistic，也就是静态的，这个函数的作用是？
// 
static void statistic() {
  IFNDEF(CONFIG_TARGET_AM, setlocale(LC_NUMERIC, ""));
#define NUMBERIC_FMT MUXDEF(CONFIG_TARGET_AM, "%", "%'") PRIu64
  Log("host time spent = " NUMBERIC_FMT " us", g_timer);
  Log("total guest instructions = " NUMBERIC_FMT, g_nr_guest_inst);
  if (g_timer > 0) Log("simulation frequency = " NUMBERIC_FMT " inst/s", g_nr_guest_inst * 1000000 / g_timer);
  else Log("Finish running in less than 1 us and can not calculate the simulation frequency");
}

void assert_fail_msg() {
  isa_reg_display();
  statistic();
}

/* Simulate how the CPU works. */
// 如果n现在比最大指令数小，就可以运行，怎么运行呢？ 我们监视他的状态， 如果是end或者abort，就先不做什么，否则设置成running
// 然后我们获得时间，执行n步，得到结束时间，从而得到时间辍。之后我们根据状态判断，运行态则停止，end或者abort则输出日至， quit态则
// 执行statistic?
void cpu_exec(uint64_t n) {
  // 查看状态
  g_print_step = (n < MAX_INST_TO_PRINT);
  switch (nemu_state.state) {
    case NEMU_END: case NEMU_ABORT:
      printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
      return;
    default: nemu_state.state = NEMU_RUNNING;
  }

  uint64_t timer_start = get_time();
  // 执行指令
  execute(n);

  uint64_t timer_end = get_time();
  g_timer += timer_end - timer_start;

  switch (nemu_state.state) {
    case NEMU_RUNNING: nemu_state.state = NEMU_STOP; break;

    case NEMU_END: case NEMU_ABORT:
      Log("nemu: %s at pc = " FMT_WORD,
          (nemu_state.state == NEMU_ABORT ? ANSI_FMT("ABORT", ANSI_FG_RED) :
           (nemu_state.halt_ret == 0 ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) :
            ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED))),
          nemu_state.halt_pc);
      // fall through
    case NEMU_QUIT: statistic();
  }
}
