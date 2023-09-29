#include <common.h>
// do_event是一个上下文的指针，将事件和上下文传入
// 然后我们根据event的事件编号，看后续操作
static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}
// 这就是初始化irq
// 简单来说，我们cte_init了
void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
