#include <am.h>

static uint64_t boot_time = 0;

#define CLINT_MMIO 0x2000000ul
#define TIME_BASE 0xbff8

// 这就是read时间
// 我们首先得到low为clint_mmio + 基准
// 然后hi也是，不过加上4
// 之后时间就是高位前32位，低为low
// 之后time处以10
static uint64_t read_time() {
  uint32_t lo = *(volatile uint32_t *)(CLINT_MMIO + TIME_BASE + 0);
  uint32_t hi = *(volatile uint32_t *)(CLINT_MMIO + TIME_BASE + 4);
  uint64_t time = ((uint64_t)hi << 32) | lo;
  return time / 10;
}
// 这就是am_timer的上达时间
void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uptime->us = read_time() - boot_time;
}

// 这是am_timer的初始化
// boot的时间即read的时间
void __am_timer_init() {
  boot_time = read_time();
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
