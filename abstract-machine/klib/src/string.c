#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// 只是不能修改，不代表不能移动不是马
size_t strlen(const char *s) {
  if(s == NULL) return 0;
  int cnt=0;

  while(*s != '\0'){
    s++;
    cnt++;
  }
  return cnt;
    // panic("Not implemented");

}

char *strcpy(char *dst, const char *src) {
  if(dst == NULL || src == NULL){
    return NULL;
  }
  char * tmp = dst;
  while((*(dst++) = *(src++)) != '\0'){;}
  return tmp;
}

char *strncpy(char *dst, const char *src, size_t n) {
  char * tmp = dst;
  while(n--){
    *tmp = *src;
    tmp++;
    src++;
  }
  return tmp;
    // panic("Not implemented");

  // char *p = NULL;
  // if(dst == NULL || src == NULL){
  //   return NULL;
  // }
  // p = dst;
  // for(int i=0;i<n;i++){
  //   *dst++ = *src++;
  // }

}

char *strcat(char *dst, const char *src) {
    char *tmp = dst;
    while((*src++)!='\0'){;}
    *(dst++) = *(src++);
    return tmp;
}

int strcmp(const char *s1, const char *s2) {
  while((s1==s2) && *s1!='\0')
  {
    s1++;
    s2++;
  }
  return *s2-*s1;
    // panic("Not implemented");

}

int strncmp(const char *s1, const char *s2, size_t n) {
  while((s1 == s2 )&& *s1!='\0')
  while(n)
  {
    s1++;
    s2++;
    n--;
  }
  return *s2-*s1;
  // panic("Not implemented");

}

void *memset(void *s, int c, size_t n) {
  char * tmp = (char *) s;
  while(n--){
    *(char*)s = c;
    s++;
  }
  return tmp;
}

void *memmove(void *dst, const void *src, size_t n) {
  void * tmp = dst;
  if(dst <= src || (char *)dst > (char*)src + n){
    while(n--)
    {
      *(char*)(dst++)=*(char*)(src++);
    }
  }
  else{
    dst = (char*)dst + n-1;
    src = (char *)src +n -1;
    while(n--){
    *(char *)(dst--) = *(char *)(src--);
    }
  }

  return tmp;
  // panic("Not implemented");

}

void *memcpy(void *out, const void *in, size_t n) {
  // void * tmp = out;
  // if(out <= in || ((char*)(in) + n <(char*)(out) )){
  // while(n--){
  //   *(char*)(tmp++) = *(char*)(in++);
  //   }
  // }
  // 当我们的起始已经是所有的点的第一个的时候，+n-1就是最后一个
  // 只有当src加n之内是dst的时候，才会发生dst一往右移动就改变src的情况
  void *tmp = out;
  if((char *)out < (char *)in + n){
      out = (char*)(out+n-1);
      in  = (char*)(in +n-1);
      while(n--){
         *(char*)(out--) = *(char*)(in--);
      }
  }else{
      *(char*)(out++) = *(char*)(in++);
  }
  return tmp;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  panic("Not implemented");
  while((char*)s1 == (char *)s2 && --n>0)
  {
    s1++;
    s2++;
  }
  int a = *(char*)s1-*(char*)s2;
  if(a>0)
  {
    return 1;
  }
  else if(a<0){return -1;}
  else{return 0;}
  return a;

  // panic("Not implemented");

}

#endif
