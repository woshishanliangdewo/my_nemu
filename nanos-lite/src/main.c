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
  init_ramdisk();

// 如果有cte，我们就会初始化irq
#ifdef HAS_CTE
  init_irq();
#endif
// 这应该是文件系统
  init_fs();
// 这应该是一个进程块
  init_proc();

  Log("Finish initialization");
// 如果有hascte，那就yield
#ifdef HAS_CTE
  yield();
#endif

  panic("Should not reach here");
}
