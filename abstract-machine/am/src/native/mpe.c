#include <stdatomic.h>
#include "platform.h"
// 未知的玩意

int __am_mpe_init = 0;
extern bool __am_has_ioe;
void __am_ioe_init();

// 初始化mpe
// 传入一个函数entry
// 
bool mpe_init(void (*entry)()) {
  __am_mpe_init = 1;

  int sync_pipe[2];
  assert(0 == pipe(sync_pipe));

  for (int i = 1; i < cpu_count(); i++) {
    if (fork() == 0) {
      char ch;
      assert(read(sync_pipe[0], &ch, 1) == 1);
      assert(ch == '+');
      close(sync_pipe[0]); close(sync_pipe[1]);

      thiscpu->cpuid = i;
      __am_init_timer_irq();
      entry();
    }
  }

  if (__am_has_ioe) {
    __am_ioe_init();
  }


  for (int i = 1; i < cpu_count(); i++) {
    assert(write(sync_pipe[1], "+", 1) == 1);
  }
  close(sync_pipe[0]); close(sync_pipe[1]);
  
  entry();
  panic("MP entry should not return\n");
}

// cpu的cont
// 返回一个ncpu
int cpu_count() {
  extern int __am_ncpu;
  return __am_ncpu;
}

// cpu的current
// 返回thiscpu
int cpu_current() {
  return thiscpu->cpuid;
}

// 原子的改变
// atomic_exchange
int atomic_xchg(int *addr, int newval) {
  return atomic_exchange((int *)addr, newval);
}
