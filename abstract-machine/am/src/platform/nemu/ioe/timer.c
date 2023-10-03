#include <am.h>
#include <nemu.h>

void __am_timer_init() {
}
// 这其实是寄存器的操作
void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uint32_t low = inl(RTC_ADDR);
  uint32_t high = inl(RTC_ADDR + 4 );
  uptime->us = (uint64_t)low + (((uint64_t)high)<<32);
}
// 所谓的timer_rtc不过是一堆时间罢了
void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
