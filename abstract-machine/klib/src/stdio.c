#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}
// 在va系列的操作中，接下来我会进行按序排列
// 首先va_arg是我们的可变长参数
// 这个参数包括亮点，一个是指向的指针va_list类型，另一个则是变量类型
// 我们自己定义一个va_list，然后通过va_start让他指向第一个可变，
// 然后这个va_start是开始，告诉我们谁之后都是可变参数了
// va_arg让我们在取出指针值的时候，让va向下一个移动
// va_end则是让指针消失，也就是销毁
int sprintf(char *out, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  char ch;
  while(ch = *(fmt++)){
    if(ch =='%'){
      ch = *(fmt++);
      if(ch == 's'){
        char* name = va_arg(args,char *);
        strcpy(out,name);
        out = out+strlen(name);
      }else if(ch == 'd'){
        int name = va_arg(args,int);
        out = out+itoa(name,out,10);
      }
    }else{
      *out = ch;
      out++;
    }
  }
  panic("Not implemented");
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
