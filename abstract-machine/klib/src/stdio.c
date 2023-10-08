#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  // int i;
  // char buf[256];
  // va_list args;
  // int i = vsprintf(buf,fmt,args);
  // write(buf,i);
  // return i;
  va_list ap;
  char tmp[256];
  va_start(ap,fmt);
  vsprintf(tmp,fmt,ap);
  char * p = tmp;
  while (*p) {
    putch(*p);
    p++;
  }
  return 0;
}
char *itoa(int value, char *string, int radix){
    int i = 0;
    char tmp[33];
    if(value == 0){
        string[i] = '0';
        return string;
    }else if(value < 0){
        value = ~(value-1);
    }
    // 100 16
    // 6   4
    int result = value/radix;
    int left = value%radix;
    tmp[i++] = left;
    do{
        // 6  0
        left = result%radix;
        result = result/radix;
        tmp[i++] = left;
    }while(result != 0);
    int j = 0;
    while(i--){
        string[i] = tmp[j];
        j++;
    }
    return string;
}
// char *itoa(int value, char *string, int radix){
//     char str[33] = {0};
//     int value1 = value;
//     int i = 0;
//     if(value < 0){
//       value1 = -value;
//       value1 = ~value1;
//       value1 += 1;
//     }else if(value > 0){
//       int num = value1%radix;
//       str[i] = (num > 9)?(num-10 + 'a'):(num + '0');
//       value1 = value1/radix;
//       i++;
//     }else if(value == 0){
//       str[i] = '0';
//       i++;
//     }
//     i--;
//     int j= 0;
//     while(i--){
//       string[i] = str[j];
//       j++; 
//     }
//   return string;
// }

// fmt是很长的字符串
// 其中有一个是%s
// 此时的s是后边的不定长字符串
int vsprintf(char *out, const char *fmt, va_list ap) {
  // va_start(ap,fmt);
  // char buf[256];
  // while(*fmt != '\0'){
  //   if(*fmt != '%'){
  //     *out++ = *fmt++;
  //     continue;
  //   }else{
  //     fmt++;
  //     switch(*fmt){
  //       case 's':
  //         char * args = va_arg(ap,char *);
  //         strcpy(out,args);
  //         out+=sizeof(args);
  //         // 有他马的王了
  //         break;
  //       case 'd':
  //         int num = va_arg(ap,int);
  //         out+=itoa(num,out,10);
  //         break;
  //     }
  //   }
  // }
  // panic("Not implemented");
  char * x= out;
  while(*fmt != '\0'){
      if(*fmt != '%'){
        *out = *fmt;
        out++;
        fmt++;
        continue;
      }else{
        fmt++;
        switch(*fmt){
          case 's':{
            char * s = va_arg(ap,char*);
            strcpy(out, s);
            out+=sizeof(s);  
            break; 
          } 
          case 'd':{
            int num = va_arg(ap,int);
            out+=sizeof(itoa(num,out,10));
            break;
          }
        }
      continue;
      }
  }
  return out-x+1;
}

// int atoi(char* s, int base){
//     int sum = 0;
//     while((*s)!='\0'){
//         if(*s<'0' || *s> '9')
//         return -1;
//         int num = s-'0';
//         sum = sum*base+num;
//         s++;
//     }
//     return sum;
// }


// 在va系列的操作中，接下来我会进行按序排列
// 首先va_arg是我们的可变长参数
// 这个参数包括亮点，一个是指向的指针va_list类型，另一个则是变量类型
// 我们自己定义一个va_list，然后通过va_start让他指向第一个可变，
// 然后这个va_start是开始，告诉我们谁之后都是可变参数了
// va_arg让我们在取出指针值的时候，让va向下一个移动
// va_end则是让指针消失，也就是销毁
// 下面是对上边的自实现
// typedef char * va_list
// typedef va_start(args, fmt) args = (va_list)(&fmt);
// typedef va_end(list) list = (va_list)0;
int sprintf(char *out, const char *fmt, ...) {
    va_list args;
    va_start(args,fmt);
    int i = vsprintf(out,fmt,args);
    return i;
}
  // va_list args;
  // va_start(args, fmt);
  // char ch;
  // while(ch = *(fmt++)){
  //     if(ch == '%'){
  //       ch = *(fmt++);
  //       if(ch == 's'){
  //         char* tmp = va_arg(args,char*);
  //         strcpy(out, tmp);
  //         out = out + strlen(tmp);
  //       }else if(ch == 'd'){
  //         int name = va_arg(args,int);
  //         out = out + itoa(name,out,10);
  //       }
  //     }else{
  //       *out = ch;
  //       out ++;
  //     }
  // }



int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
