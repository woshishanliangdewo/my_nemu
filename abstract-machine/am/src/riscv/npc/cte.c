#include <am.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;
// 这就是irq的处理函数
// 首先是一个事件ev
// 然后我们看上下文的mcause
// 之后另事件为error，并使用user_handler(ev,c)
Context* __am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      default: ev.event = EVENT_ERROR; break;
      
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  return NULL;
}
// 由于我们已经有了地址，所以我们这里的操作十分简单
// 将中断保存下来，然后ecall到mtvec的部分
void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
