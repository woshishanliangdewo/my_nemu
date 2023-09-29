#include <am.h>
#include <klib-macros.h>

void __am_timer_init();
void __am_timer_rtc(AM_TIMER_RTC_T *);
void __am_timer_uptime(AM_TIMER_UPTIME_T *);

static void __am_timer_config(AM_TIMER_CONFIG_T *cfg) { cfg->present = true; cfg->has_rtc = true; }

// 这是lut，这是一个void的指针，然后
typedef void (*handler_t)(void *buf);
static void *lut[128] = {
  [AM_TIMER_CONFIG] = __am_timer_config,
  [AM_TIMER_RTC   ] = __am_timer_rtc,
  [AM_TIMER_UPTIME] = __am_timer_uptime,
};

// 这是fail， 我们会输出到达了不存在的寄存器
static void fail(void *buf) { panic("access nonexist register"); }
// 这是ioe的初始化
// 在lut的长度中， 我们将lut，lut包括计时器的配置，rtc和更新时间
// 然后初始化的时候会是输出不可达
// 然后我们结束了am_timer的初始化
bool ioe_init() {
  for (int i = 0; i < LENGTH(lut); i++)
    if (!lut[i]) lut[i] = fail;
  __am_timer_init();
  return true;
}

// 我们进行读和写的时候，都是操控一个reg，然后对这个函数传递参数buf
void ioe_read (int reg, void *buf) { ((handler_t)lut[reg])(buf); }
void ioe_write(int reg, void *buf) { ((handler_t)lut[reg])(buf); }
