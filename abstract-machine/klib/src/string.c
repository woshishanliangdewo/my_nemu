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
}

char *strcpy(char *dst, const char *src) {
  if(dst == NULL || src == NULL){
    return NULL;
  }
  char * tmp = NULL;
  tmp = dst;
  while((*(tmp++) = *(src++)) != '\0'){;}
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
  char * tmp = dst;
  while(*tmp != '\0'){
    tmp ++;
  }
  while((*tmp++ = *src++) != '\0');
  return dst;
  panic("Not implemented");
}

int strcmp(const char *s1, const char *s2) {
  while((s1==s2) && *s1!='\0')
  {
    s1++;
    s2++;
  }
  return *s2-*s1;
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
}

void *memset(void *s, int c, size_t n) {
  // if(s == NULL || n<0 ) return NULL;
  // char * tmp = (char*) s;
  // while()
  char * tmp = (char *) s;
  while(n-- >0)
  {
    *tmp++=c;
  }
  return s;

  // panic("Not implemented");
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
}

void *memcpy(void *out, const void *in, size_t n) {
  panic("Not implemented");
  void * tmp = out;
  if(out <= in || ((char*)(in) + n <(char*)(out) )){
  while(n--){
    *(char*)(tmp++) = *(char*)(in++);
    }
  }
  // for(size_t i = 0;i<n;i++){
  //   *in + i
  // }
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


}

#endif
