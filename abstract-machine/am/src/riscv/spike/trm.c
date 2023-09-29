#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include "htif.h"

extern char _heap_start;
int main(const char *args);

extern char _pmem_start;
#define PMEM_SIZE (128 * 1024 * 1024)
#define PMEM_END ((uintptr_t)0x80000000 + PMEM_SIZE)
//heap是一个从heap_start开始的
// 然后这个前后这两个就是将两个地址分别赋值给了.start和.end
// 之后我们定义了mainargs
// 将mainargs赋值给mainargs数组
// 
Area heap = RANGE(&_heap_start, PMEM_END);
#ifndef MAINARGS
#define MAINARGS ""
#endif
static const char mainargs[] = MAINARGS;

void putch(char ch) {
  htif_console_putchar(ch);
}

void halt(int code) {
  printf("Exit with code = %d\n", code);
  htif_poweroff();

  // should not reach here
  while (1);
}

void _trm_init() {
  int ret = main(mainargs);
  halt(ret);
}
