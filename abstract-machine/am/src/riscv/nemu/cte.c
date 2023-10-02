#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;
// word_t isa_raise_intr(word_t NO, vaddr_t epc);

// 这里是说后便如果调用了这个
// 那么就会调用一个do_event(ev,c)的函数
// ev是事件0， c是我们的Context，如果c的mcause是存在的
// 事件就会变为error
Context* __am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      default: ev.event = EVENT_ERROR; break;
      case -1:
        ev.event = EVENT_YIELD; break;
    }
    c = user_handler(ev, c);
    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);
// 这是cte的初始化
// 其中他将mtvec中写入值
// 然后user_handler 就是事件的handler
// 这里写入的是异常的入口地址

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  // 写的是这个函数的地址
  // mtvec中有了一个中断处理函数的地址
  // 将通用寄存器的值写入CSR中
  // 这是一个函数的地址
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));
  
  // register event handler
  user_handler = handler;
  // 因为这样调用了，所以其实是会调用一个函数叫做do_event()
  

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  return NULL;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
