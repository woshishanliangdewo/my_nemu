#ifndef LOONGARCH32R_H__
#define LOONGARCH32R_H__

#include <stdint.h>

static inline uint8_t  inb(uintptr_t addr) { return *(volatile uint8_t  *)addr; }
static inline uint16_t inw(uintptr_t addr) { return *(volatile uint16_t *)addr; }
// 什么是inl ，就是in一个long
// 换句话说，就是将一个addr地址对应的值进行返回
static inline uint32_t inl(uintptr_t addr) { return *(volatile uint32_t *)addr; }
// 什么是outb，这个是将addr赋值，值为data
// C语言中*(volatile unsigned int *)0x500的解释：
// 如下；
// (unsigned int *)0x500:将地址0x500强制转化为int型指针
// *(unsigned int *)0x500=0x10：对地址为0x500赋值为0x10
static inline void outb(uintptr_t addr, uint8_t  data) { *(volatile uint8_t  *)addr = data; }
static inline void outw(uintptr_t addr, uint16_t data) { *(volatile uint16_t *)addr = data; }
static inline void outl(uintptr_t addr, uint32_t data) { *(volatile uint32_t *)addr = data; }

#define PTE_V 0x1
#define PTE_D 0x2

// Page directory and page table constants
#define PTXSHFT   12      // Offset of PTX in a linear address
#define PDXSHFT   22      // Offset of PDX in a linear address

#define PDX(va)     (((uint32_t)(va) >> PDXSHFT) & 0x3ff)
#define PTX(va)     (((uint32_t)(va) >> PTXSHFT) & 0x3ff)

#endif
