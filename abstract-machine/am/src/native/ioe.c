#include <am.h>
#include <klib-macros.h>

bool __am_has_ioe = false;
static bool ioe_init_done = false;

void __am_timer_init();
void __am_gpu_init();
void __am_input_init();
void __am_audio_init();
void __am_disk_init();
void __am_input_config(AM_INPUT_CONFIG_T *);
void __am_timer_config(AM_TIMER_CONFIG_T *);
void __am_timer_rtc(AM_TIMER_RTC_T *);
void __am_timer_uptime(AM_TIMER_UPTIME_T *);
void __am_input_keybrd(AM_INPUT_KEYBRD_T *);
void __am_gpu_config(AM_GPU_CONFIG_T *);
void __am_gpu_status(AM_GPU_STATUS_T *);
void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *);
void __am_audio_config(AM_AUDIO_CONFIG_T *);
void __am_audio_ctrl(AM_AUDIO_CTRL_T *);
void __am_audio_status(AM_AUDIO_STATUS_T *);
void __am_audio_play(AM_AUDIO_PLAY_T *);
void __am_disk_config(AM_DISK_CONFIG_T *cfg);
void __am_disk_status(AM_DISK_STATUS_T *stat);
void __am_disk_blkio(AM_DISK_BLKIO_T *io);
static void __am_uart_config(AM_UART_CONFIG_T *cfg)   { cfg->present = false; }
static void __am_net_config (AM_NET_CONFIG_T *cfg)    { cfg->present = false; }

typedef void (*handler_t)(void *buf);
static void *lut[128] = {
  [AM_TIMER_CONFIG] = __am_timer_config,
  [AM_TIMER_RTC   ] = __am_timer_rtc,
  [AM_TIMER_UPTIME] = __am_timer_uptime,
  [AM_INPUT_CONFIG] = __am_input_config,
  [AM_INPUT_KEYBRD] = __am_input_keybrd,
  [AM_GPU_CONFIG  ] = __am_gpu_config,
  [AM_GPU_FBDRAW  ] = __am_gpu_fbdraw,
  [AM_GPU_STATUS  ] = __am_gpu_status,
  [AM_UART_CONFIG ] = __am_uart_config,
  [AM_AUDIO_CONFIG] = __am_audio_config,
  [AM_AUDIO_CTRL  ] = __am_audio_ctrl,
  [AM_AUDIO_STATUS] = __am_audio_status,
  [AM_AUDIO_PLAY  ] = __am_audio_play,
  [AM_DISK_CONFIG ] = __am_disk_config,
  [AM_DISK_STATUS ] = __am_disk_status,
  [AM_DISK_BLKIO  ] = __am_disk_blkio,
  [AM_NET_CONFIG  ] = __am_net_config,
};
// ioe的初始化
// 判断了两种情况后，就将__am_has_ioe设置为true了
// 是否分配了cpu
// 是否已经初始化过了
// 将__am_has_ioe变为true
// 然后返回
bool ioe_init() {
  
  panic_on(cpu_current() != 0, "call ioe_init() in other CPUs");
  panic_on(ioe_init_done, "double-initialization");
  __am_has_ioe = true;
  return true;
}

static void fail(void *buf) { panic("access nonexist register"); }
// 这就是am的ioe的初始化
// 首先是lut
// 然后是几个不一样的函数，第一个是timer的init
// 这是将boot_time赋值，并且获得了当前的时间
// 这是通过gettimeofday实现的，目前这个结构体中只是存放了时间
// 单位分别是秒和微妙

void __am_ioe_init() {
  for (int i = 0; i < LENGTH(lut); i++)
    if (!lut[i]) lut[i] = fail;
  __am_timer_init();
// 然后是__am_gpu_init，这是通过将ideo和timer分别创建了sdl后
// 又分别创建了窗口，名为Native Application，并且屏幕位置为定义，
// 同时将W和H写死，作用是窗口的大小
// 然后创建了一个surface，是一个区域，设置了w，h以及深度，也就是说
// 有2^32种颜色，并且有四个言马
  __am_gpu_init();
// 这里是input的初始化
// 这是用来处理事件的，也就是处理所谓的按键按钮的
// 等待并获取下一个事件，阻塞程序执行，直到有事件发生
// 若是对列为空，则阻塞直到有事件了
// 有事件就将事件弹出并且返回一个非零值
  __am_input_init();
  __am_audio_init();
  __am_disk_init();
  ioe_init_done = true;
}

static void do_io(int reg, void *buf) {
  if (!ioe_init_done) {
    __am_ioe_init();
  }
  ((handler_t)lut[reg])(buf);
}

void ioe_read (int reg, void *buf) { do_io(reg, buf); }
void ioe_write(int reg, void *buf) { do_io(reg, buf); }
