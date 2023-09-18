#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  panic("Not implemented");
}

char *strcpy(char *dst, const char *src) {

  // char * p = NULL;
  // if(dst == NULL || src == NULL){
  //   return NULL;
  // }
  // p = dst;
  // while((*dst++ = *src++)!= '\0')
  // return p;
  panic("Not implemented");
}

char *strncpy(char *dst, const char *src, size_t n) {
  // char *p = NULL;
  // if(dst == NULL || src == NULL){
  //   return NULL;
  // }
  // p = dst;
  // for(int i=0;i<n;i++){
  //   *dst++ = *src++;
  // }

  panic("Not implemented");
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
  // char * tmp1 = s1;
  // char * tmp2 = s2;
  // for(tmp1,tmp2; *tmp1 != '\0' && *tmp2 != '\0';tmp1 ++,tmp2++){
  //   if(*tmp1 == '\0' && *tmp2!='\0') return -1;
  //   else if(*tmp2 == '\0' && *tmp1!='\0') return 1;
  //   else if(*tmp1 == *tmp2) {;}
  //   else if(*tmp1 != *tmp2) {
  //     if(*tmp1 <*tmp2 ) return -1;
  //     else if(*tmp1 > *tmp2) return 1;
  //   }
  //   return 0;
  // }

  panic("Not implemented");
}

int strncmp(const char *s1, const char *s2, size_t n) {
  panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
  panic("Not implemented");
}

void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  panic("Not implemented");
  
  // for(size_t i = 0;i<n;i++){
  //   *in + i
  // }
}

int memcmp(const void *s1, const void *s2, size_t n) {
  panic("Not implemented");

}

#endif
