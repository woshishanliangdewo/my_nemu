#include <am.h>
#include <SDL2/SDL.h>
#include <fenv.h>

// 根据我们的模式，我们选择长宽
//#define MODE_800x600
#ifdef MODE_800x600
# define W    800
# define H    600
#else
# define W    400
# define H    300
#endif
// 这是FPS
#define FPS   60

// 这是RGBA的各自的言马，都是256
#define RMASK 0x00ff0000
#define GMASK 0x0000ff00
#define BMASK 0x000000ff
#define AMASK 0x00000000

// SDL的window和surface
static SDL_Window *window = NULL;
static SDL_Surface *surface = NULL;

// 这是text的同步
static Uint32 texture_sync(Uint32 interval, void *param) {
// 这是SDL的bit
  SDL_BlitScaled(surface, NULL, SDL_GetWindowSurface(window), NULL);
  SDL_UpdateWindowSurface(window);
  return interval;
}

// 这是gpu的init
// 初始化的是video和timer
void __am_gpu_init() {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
// 然后创建一个显示用的窗口
// 两个位定义，然后将w和h分别变成对应的大小，即窗口大小
  window = SDL_CreateWindow("Native Application",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
#ifdef MODE_800x600
      W, H,
#else
      W * 2, H * 2,
#endif
      SDL_WINDOW_OPENGL);
// 这创建的是RGB的surface，其中第一个只是为了兼容，然后是长宽和位深度
// 以及一个像素点所占字节数，然后是rgba的言马，也就是说有2^32种颜色，也就是说
// rgba各自8中
  surface = SDL_CreateRGBSurface(SDL_SWSURFACE, W, H, 32,
      RMASK, GMASK, BMASK, AMASK);
// 定时执行用户定义的回调函数
// 返回一个定时器的id，参数是间隔时间，回调函数以及参数
  SDL_AddTimer(1000 / FPS, texture_sync, NULL);
}

// 这是gpu的设置
// 首先是一个cpu配置的cfg设置为一个
// 结构体
// 结构体的present，height，width，vmemsz等都在这里设置
void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = W, .height = H,
    .vmemsz = 0
  };
}

// 这是gpu的状态
// 我们确定已经是raedy了
void __am_gpu_status(AM_GPU_STATUS_T *stat) {
  stat->ready = true;
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;
  if (w == 0 || h == 0) return;
  feclearexcept(-1);
  // 使用内存像素创建一个surface
  SDL_Surface *s = SDL_CreateRGBSurfaceFrom(ctl->pixels, w, h, 32, w * sizeof(uint32_t),
      RMASK, GMASK, BMASK, AMASK);
      //sdl_rect是指一个矩阵的大小，使用s这个surface上画图
      // 话一个矩阵，参数是rect的
      // 然后将他释放
  SDL_Rect rect = { .x = x, .y = y };
  SDL_BlitSurface(s, NULL, surface, &rect);
  SDL_FreeSurface(s);
}
