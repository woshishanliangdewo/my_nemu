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
  panic("Not implemented");
}

int strcmp(const char *s1, const char *s2) {
  
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
}

int memcmp(const void *s1, const void *s2, size_t n) {
  panic("Not implemented");
}

#endif
