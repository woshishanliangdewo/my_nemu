#include <am.h>
#include <nemu.h>
// 0x8000
// 0000 0000 0000 0000 1000 0000 0000 0000
// 获取按键状态，屏蔽掉其他的可能状态,按照MSDN上说低位should ignore。
#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t kc = inl(KBD_ADDR);
  kbd->keydown = kc & KEYDOWN_MASK ? true : false;
  kbd->keycode = kc & ~KEYDOWN_MASK;
}
