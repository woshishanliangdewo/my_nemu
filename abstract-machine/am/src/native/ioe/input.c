#include <am.h>
#include <SDL2/SDL.h>

//这是一个键盘按下的言马
#define KEYDOWN_MASK 0x8000

// 这是队列长度
#define KEY_QUEUE_LEN 1024
static int key_queue[KEY_QUEUE_LEN] = {};
static int key_f = 0, key_r = 0;
// 一个sdl的锁
static SDL_mutex *key_queue_lock = NULL;

#define XX(k) [SDL_SCANCODE_##k] = AM_KEY_##k,
static int keymap[256] = {
  AM_KEYS(XX)
};

static int event_thread(void *args) {
// 这是用来处理事件的，也就是处理所谓的按键按钮的
  SDL_Event event;
  while (1) {
// 等待并获取下一个事件，阻塞程序执行，直到有事件发生
// 若是对列为空，则阻塞直到有事件了
// 有事件就将事件弹出并且返回一个非零值
    SDL_WaitEvent(&event);
    // 根据事件分类
    switch (event.type) {
      case SDL_QUIT: halt(0);
      case SDL_KEYDOWN:
      case SDL_KEYUP: {
        SDL_Keysym k = event.key.keysym;
        int keydown = event.key.type == SDL_KEYDOWN;
        int scancode = k.scancode;
        if (keymap[scancode] != 0) {
          int am_code = keymap[scancode] | (keydown ? KEYDOWN_MASK : 0);
          SDL_LockMutex(key_queue_lock);
          key_queue[key_r] = am_code;
          key_r = (key_r + 1) % KEY_QUEUE_LEN;
          SDL_UnlockMutex(key_queue_lock);
          void __am_send_kbd_intr();
          __am_send_kbd_intr();
        }
        break;
      }
    }
  }
}

// init一个input
void __am_input_init() {
  // 创建一个互斥量，并初始化为解锁状态
  key_queue_lock = SDL_CreateMutex();
  // 然后我们创建一个进程，然后是一个函数是我们的进程，那就是event_thread
  SDL_CreateThread(event_thread, "event thread", NULL);
}

// 表示现在已经是input的配置了
void __am_input_config(AM_INPUT_CONFIG_T *cfg) {
  cfg->present = true;
}

// 现在是input的键盘案件
void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  int k = AM_KEY_NONE;

  SDL_LockMutex(key_queue_lock);
  if (key_f != key_r) {
    k = key_queue[key_f];
    key_f = (key_f + 1) % KEY_QUEUE_LEN;
  }
  SDL_UnlockMutex(key_queue_lock);

  kbd->keydown = (k & KEYDOWN_MASK ? true : false);
  kbd->keycode = k & ~KEYDOWN_MASK;
}
