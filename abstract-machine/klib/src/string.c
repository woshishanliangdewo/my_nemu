#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// 只是不能修改，不代表不能移动不是马
// strlen不包括最后的一个字符'\0'
size_t strlen(const char *s) {
  int size = 0;
  while(*s != '\0'){
    size++;
    s++;
  }
  return size;
}

// 如果空间不够大呢
char *strcpy(char *dst, const char *src) {
  if(dst == NULL || src == NULL){
    return NULL;
  }
  char * tmp = dst;
  if((*dst = *src) != '\0')
  {
    dst++;
    src++;
  }
  return tmp;
}

// ?是否会有重叠呢
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

// char *strcat(char *dest, const char *src) {
//     char *tmp = dest;

//     while (*dest != '\0') {
//         dest++;
//     }
//     while (*src) {
//         *dest++ = *src++;
//     }

//     *dest = '\0';

//     return tmp;
// }

// 如果while(a++ != x)
// 那么会到x后一个
// 如果while(a != x)
// 那么会到x正好
// 只要有加加，就是下一个
char *strcat(char *dst, const char *src) {
    char *tmp = dst;
    while((*dst)!='\0'){
      dst++ ;
    }

    while(*src){
        *dst++ = *src++;
    }

    *dst = '\0';
    return tmp;
}

// ？ 会有什么情况呢？
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
  void * tmp = s;
  while(n--){
    *(char*)s++ = c;
  }
  return tmp;
}

// ？？？
void *memmove(void *dst, const void *src, size_t n) {
  char* tmp = (char*)dst;
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
  // 当我们的起始已经是所有的点的第一个的时候，+n-1就是最后一个
  // 只有当src加n之内是dst的时候，才会发生dst一往右移动就改变src的情况
  // src+n是向右的字符串，所以要小心
  char *tmp =(char *)out;
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
  while(n--){
    if(*(char*)s1 == *(char*)s2){
      s1++;
      s2++;
    }else{
      return *(char*)s1-*(char*)s2;
    } 
  }
  return 0;
}
#endif




// #define UCHAR_MAX (0xffU)

// #define SS (sizeof(size_t))
// #define ALIGN (sizeof(size_t) - 1)
// #define ONES ((size_t)-1 / UCHAR_MAX)
// #define HIGHS (ONES * (UCHAR_MAX / 2 + 1))
// #define HASZERO(x) (((x)-ONES) & ~(x) & HIGHS)





// char *strncpy(char *d, const char *s, size_t n) {
//     typedef size_t __attribute__((__may_alias__)) word;
//     word *wd;
//     const word *ws;
//     if (((uintptr_t)s & ALIGN) == ((uintptr_t)d & ALIGN)) {
//         for (; ((uintptr_t)s & ALIGN) && n && (*d = *s); n--, s++, d++)
//             ;
//         if (!n || !*s) {
//             goto tail;
//         }
//         wd = (void *)d;
//         ws = (const void *)s;
//         for (; n >= sizeof(size_t) && !HASZERO(*ws); n -= sizeof(size_t), ws++, wd++) {
//             *wd = *ws;
//         }
//         d = (void *)wd;
//         s = (const void *)ws;
//     }
//     for (; n && (*d = *s); n--, s++, d++)
//         ;
// tail:
//     memset(d, 0, n);
//     return d;
// }


// int strcmp(const char *str1, const char *str2) {
//     while (*str1 && *str2) {
//         if (*str1 != *str2) {
//             return (*str1) - (*str2);
//         }
//         ++str1;
//         ++str2;
//     }
//     return (*str1) - (*str2);
// }

// int strncmp(const char *_l, const char *_r, size_t n) {
//     const unsigned char *l = (void *)_l, *r = (void *)_r;
//     if (!n--) {
//         return 0;
//     }
//     for (; *l && *r && n && *l == *r; l++, r++, n--)
//         ;
//     return *l - *r;
// }

// void *memset(void *dest, int val, size_t len) {
//     uint8_t *dst = (uint8_t *)dest;

//     for (; len != 0; len--) {
//         *dst++ = val;
//     }

//     return dest;
// }

// void *memmove(void *dst, const void *src, size_t n) {
//     char p_tmp[100];
//     memcpy(p_tmp, src, n);
//     memcpy(dst, p_tmp, n);
//     return dst;
// }

// void *memcpy(void *dest, const void *source, size_t len) {
//     uint64_t *dst = (uint64_t *)dest;
//     uint64_t *src = (uint64_t *)source;

//     for (; len >= 8;) {
//         *(uint64_t *)dst++ = *(const uint64_t *)src++;
//         len -= 8;
//     }
//     uint8_t *dst_left = (uint8_t *)dst;
//     uint8_t *src_left = (uint8_t *)src;
//     for (; len > 0; len--) {
//         *(uint8_t *)dst_left++ = *(const uint8_t *)src_left++;
//     }
//     return dest;
// }

// int memcmp(const void *ptr1, const void *ptr2, size_t num) {
//     for (int i = 0; i < num; ++i) {
//         if (((char *)ptr1)[i] != ((char *)ptr2)[i]) {
//             return ((char *)ptr1)[i] - ((char *)ptr2)[i];
//         }
//     }
//     return 0;
// }

// #endif