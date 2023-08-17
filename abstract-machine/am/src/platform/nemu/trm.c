#include <am.h>
#include <nemu.h>
//
extern char _heap_start;
// 三个点表示建议重构
int main(const char *args);
// Area区域，也就是说根据开始和结束唯一确定一个地址
// 单下划线说明是标准库的变量
// 双下划线说明是编译器的变量
Area heap = RANGE(&_heap_start, PMEM_END);
#ifndef MAINARGS
#define MAINARGS ""
#endif
// 初始化mainargs这个数组
static const char mainargs[] = MAINARGS;

// 
void putch(char ch) {
  outb(SERIAL_PORT, ch);
}
//
void halt(int code) {
  nemu_trap(code);

  // should not reach here
  while (1);
}

void _trm_init() {
  int ret = main(mainargs);
  halt(ret);
}
