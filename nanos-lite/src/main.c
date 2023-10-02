#include <common.h>

void init_mm(void);
void init_device(void);
void init_ramdisk(void);
void init_irq(void);
void init_fs(void);
void init_proc(void);

// 首先我们打印logo
// 然后我们输出信息以及时间
int main() {
  extern const char logo[];
  printf("%s", logo);
  Log("'Hello World!' from Nanos-lite");
  Log("Build time: %s, %s", __TIME__, __DATE__);

// 初始化mm（malloc memory)
  init_mm();
// 这是外部设备的初始化
  init_device();

// 这是ramdisk
// 这里只是将地址起始进行了输出
  init_ramdisk();

// 如果有cte，我们就会初始化irq
#ifdef HAS_CTE
// 这个只是调用了cte_init
// 然后在cte_init中
// 这里是将我们要执行的函数名放入mtvec中
// 然后我们初始化user_handler
// 而这个函数就是__am_asm_trap
// 至于在__am_asm_trap中我们作了什么呢
// 答案是我们保存了各个寄存器，然后进行了一些操作
// 然后又恢复了寄存器
  init_irq();
#endif
// 这应该是文件系统)(未实现)
  init_fs();
// 这应该是一个进程块
// ？？？
  init_proc();

  Log("Finish initialization");
// 如果有hascte，那就yield
// 在yield的时候，我们会li 7,-1
// 然后ecall，ecall的作用是
// 将中断保存
// 然后将pc放到sepc中
// 并且将模式切换到s
// 然后ecall到mtvec的位置
// 由于mtvec此时是__am_asm_trap
// 所以进入了汇编
// 汇编中我们总的来说干的事情很简单，就是
// 保存所有寄存器的值，将三个csr保存到t0-t2中
// 然后将三个t保存
// 设置mstatus的值
// 保存了所有的寄存器后，我们执行__am_irq_handle
// 所以这玩意是在汇编语言中定义的
// 这玩意会先看看event和context是什么
// 然后调用do_event(ev,c)
// 然后我们会根据event作出不同操作
// 然后将寄存器恢复
#ifdef HAS_CTE
  yield();
#endif

  panic("Should not reach here");
}
