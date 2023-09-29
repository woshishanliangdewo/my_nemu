#include <am.h>
#include <stdio.h>
#include <klib-macros.h>

void __am_platform_dummy();
void __am_exit_platform(int code);

// 初始化trm
// 进行平台的dummy（仿真）
void trm_init() {
  __am_platform_dummy();
}

// 这是put一个char
// 一次只会输出一个字符
void putch(char ch) {
  putchar(ch);
}

// 这就是halt一个code

void halt(int code) {
  const char *fmt = "Exit code = 40h\n";
  for (const char *p = fmt; *p; p++) {
    char ch = *p;
    if (ch == '0' || ch == '4') {
      // 只留下code>>(ch-'0')的后四位，然后看他是十六进制的那一个
      ch = "0123456789abcdef"[(code >> (ch - '0')) & 0xf];
    }
    putch(ch);
  }
  __am_exit_platform(code);
  putstr("Should not reach here!\n");
  while (1);
}

Area heap = {};
