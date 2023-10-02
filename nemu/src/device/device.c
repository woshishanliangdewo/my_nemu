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

#include <common.h>
#include <utils.h>
#include <device/alarm.h>
#ifndef CONFIG_TARGET_AM
#include <SDL2/SDL.h>
#endif

void init_map();
void init_serial();
void init_timer();
void init_vga();
void init_i8042();
void init_audio();
void init_disk();
void init_sdcard();
void init_alarm();

void send_key(uint8_t, bool);
void vga_update_screen();

// last是一个时间
// now是现在时间
// 如果所有的时间小于
// last变为now
// 根据get_time，我们会使用get_time_internal获得一个时间
// 然后又使用他获得一个now，now-boottime就是一个时间
void device_update() {
  static uint64_t last = 0;
  uint64_t now = get_time();
  if (now - last < 1000000 / TIMER_HZ) {
    return;
  }
  last = now;
// 如果有vga，那么就更新屏幕
// 更新屏幕的部分要我自己写
  IFDEF(CONFIG_HAS_VGA, vga_update_screen());

#ifndef CONFIG_TARGET_AM
  SDL_Event event;
  // 将队列头中的事件剖出来
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      // 如果sdl是退出了，那么状态就变为推出
      case SDL_QUIT:
        nemu_state.state = NEMU_QUIT;
        break;
  // 如果有键盘
  // 如果状态
#ifdef CONFIG_HAS_KEYBOARD
      // If a key was pressed 
      // 如果被按下了
      case SDL_KEYDOWN:
      // 直到被松开
      // 在被松开后，我们要看事件的
      case SDL_KEYUP: {
        // 第一个是SDL_Event
        // 第二个是事件中的键盘事件的值
        // 第三个是被按下的键盘的健
        // 第四个是对应的键盘的真实代码
        uint8_t k = event.key.keysym.scancode;
        bool is_keydown = (event.key.type == SDL_KEYDOWN);
        // 然后我们看一下是否按下，方法是看现在的key的类型是否是按下
        // 然后我们使用send_key
        // 这个send_key是一个检测函数，一旦检测到nemu在运行而且键盘对应的真实键盘码不是空格
        // 那么就将这个键盘的言马入队
        send_key(k, is_keydown);
        break;
      }
#endif
      default: break;
    }
  }
#endif
}

// 一直检查事件
void sdl_clear_event_queue() {
#ifndef CONFIG_TARGET_AM
  SDL_Event event;
  while (SDL_PollEvent(&event));
#endif
}

// 初始化设备
void init_device() {
  // 定义了config，我们就初始化ioe
  // 在初始化ioe的过程中，我们首先确保我们的cpu
  // 就是现在这个cpu，而且我们的ioe已经初始化过了
  // 之后我们就将__am_has_ioe变为true返回即可
  // 这里的方式是看ioe是否执行两次，防止反复执行
  IFDEF(CONFIG_TARGET_AM, ioe_init());
  // 这里是对映射的初始化
  // 简单来说我们开启了一个io_space，这是IO_SPACE_MAX大小的
  // 然后我们看是否成功初始化，最后我们将p_space指向我们开启的地址就可以了
  init_map();

  // 这里是串口的初始化，过程中我们先使用了new_space进行分配空间
  // 在new_space中我们首先分配了一个指针，然后我们通过
  // size = (size + (PAGE_SIZE - 1)) & ~PAGE_MASK操作
  // 1ul<<12，这是通过代码实现将内存尺寸对其的效果
  // 具体来说，PAGE_MASK是前边为1，后便12个0
  // 如果size的大小小于一个页面的大小，就得到一个页
  // 如果size的大小在一个页与两个页之间，就得到2个页，依次类推，分别是3个页，4个页等
  // 然后返回这个space是一个以页单位的空间，然后我们添加pio的映射
  // 过程中我们先确定我们的映射次数没有超过最大值
  // 然后我们的CONFIG_SERIAL_PORT是起始地址，然后我们的space是自定义的
  // 之后我们长度是8，回调函数是serial_io_handler
  // 于是我们将我们的maps数组中添加了一个映射，这是一个结构体，分别是最高地址
  // 最低地址，space和回调函数，在加加中表示多了一个map，并且这是一个数组
  IFDEF(CONFIG_HAS_SERIAL, init_serial());

  // 这是初始化timer
  // 首先分配了一个new_space
  // 方法同上，分配一个空间
  // 然后如果定义了port，就和上边一样的方法
  IFDEF(CONFIG_HAS_TIMER, init_timer());
  // 这里是初始化vga， 首先是new一个space
  // 然后屏幕的宽度左移16与宽度或，变成一个32位数字
  // 然后我们方法同上
  IFDEF(CONFIG_HAS_VGA, init_vga());
  // 这是初始化一个芯片，首先是分配一个4size的空间
  // 然后注意，每一次new_space的时候，都会出现
  // p_space加一个size的状况，因此p会一直增加
  // 然后每次p_space指向的都是当前所在的页，而不是初始页
  // 然后p是分配新的之前的页，p_space是分配后的页
  // 所以我们的页就是我们最新分配的这一段空间了
  // 之后我们初始化后分配了一个页的空间，而页的第一个字节就是空键盘
  // 之后是添加io了，方法是增加maps数组的一个元素，并且增加下标数
  IFDEF(CONFIG_HAS_KEYBOARD, init_i8042());

  IFDEF(CONFIG_HAS_AUDIO, init_audio());
  IFDEF(CONFIG_HAS_DISK, init_disk());
  IFDEF(CONFIG_HAS_SDCARD, init_sdcard());

  IFNDEF(CONFIG_TARGET_AM, init_alarm());
}
