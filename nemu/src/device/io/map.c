/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <memory/host.h>
#include <memory/vaddr.h>
#include <device/map.h>
// 最大的IO空间
#define IO_SPACE_MAX (2 * 1024 * 1024)

static uint8_t *io_space = NULL;
static uint8_t *p_space = NULL;

// 什么是new_space
// 首先是一个指针p；
// 然后是
uint8_t* new_space(int size) {
  uint8_t *p = p_space;
  // page aligned;
  // 1ul<<12，这是通过代码实现将内存尺寸对其的效果
  size = (size + (PAGE_SIZE - 1)) & ~PAGE_MASK;
  // p_space加的是size
  p_space += size;
  assert(p_space - io_space < IO_SPACE_MAX);
  return p;
}

static void check_bound(IOMap *map, paddr_t addr) {
  if (map == NULL) {
    Assert(map != NULL, "address (" FMT_PADDR ") is out of bound at pc = " FMT_WORD, addr, cpu.pc);
  } else {
    Assert(addr <= map->high && addr >= map->low,
        "address (" FMT_PADDR ") is out of bound {%s} [" FMT_PADDR ", " FMT_PADDR "] at pc = " FMT_WORD,
        addr, map->name, map->low, map->high, cpu.pc);
  }
}
// 这是反转callback
// 什么意思呢，这么说吧，我们有一个
static void invoke_callback(io_callback_t c, paddr_t offset, int len, bool is_write) {
  if (c != NULL) { c(offset, len, is_write); }
}

void init_map() {
  // 最大的IO空间
  // 
  io_space = malloc(IO_SPACE_MAX);
  assert(io_space);
  p_space = io_space;
}
// 如果长度位于1到8之间
// 检查边界
// 地址减去映射的最低值（地址是物理地址）
// 这是一个IOMap的callback 
// offset是一个地址减去map的low
word_t map_read(paddr_t addr, int len, IOMap *map) {
  assert(len >= 1 && len <= 8);
  check_bound(map, addr);
  paddr_t offset = addr - map->low;
  // 最后会调用的函数是map->callback(offset，len，false)
  invoke_callback(map->callback, offset, len, false); // prepare data to read
  // host会read，所以会使用
  // 什么是map->space呢，然后加上offset，offset是addr减去map->low
  // 然后将map->spac加上offset
  // 看来space是起始空间
  word_t ret = host_read(map->space + offset, len);
  return ret;
}
// 检查边界
// 
void map_write(paddr_t addr, int len, word_t data, IOMap *map) {
  assert(len >= 1 && len <= 8);
  check_bound(map, addr);
  paddr_t offset = addr - map->low;
  host_write(map->space + offset, len, data);
  invoke_callback(map->callback, offset, len, true);
}
