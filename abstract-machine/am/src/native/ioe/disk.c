#include <am.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define BLKSZ 512

static int disk_size = 0;
static FILE *fp = NULL;
// 这是磁盘的初始化
// 首先是getenv
// getenv ---搜索name对应的环境字符串，并返回相应值
// 未知操作涅
void __am_disk_init() {
  const char *diskimg = getenv("diskimg");
  // 存在就打开diskimg，然后状态是可读可写
  if (diskimg) {
    fp = fopen(diskimg, "r+");
    // 打开后，寻找这个文件，然后后续操作
    if (fp) {
      fseek(fp, 0, SEEK_END);
      // 返回当前流位置的偏移值，然后加上511就是所在的块了
      disk_size = (ftell(fp) + 511) / 512;
      rewind(fp);
    }
  }
}

// 磁盘的配置
// BLKSZ是512
// blkcnt是size
void __am_disk_config(AM_DISK_CONFIG_T *cfg) {
  cfg->present = (fp != NULL);
  cfg->blksz = BLKSZ;
  cfg->blkcnt = disk_size;
}


void __am_disk_status(AM_DISK_STATUS_T *stat) {
  stat->ready = 1;
}

// 这是确定下一次读写的位置，表示对fp，在io的blkno*BLKSZ处写入读入
void __am_disk_blkio(AM_DISK_BLKIO_T *io) {
  if (fp) {
    fseek(fp, io->blkno * BLKSZ, SEEK_SET);
    // 如果write是真，则fwriteblkcnt*BLKSZ个大小的一个记录，其写入buf中
    // 否则是写入
    int ret;
    if (io->write) ret = fwrite(io->buf, io->blkcnt * BLKSZ, 1, fp);
    else ret = fread(io->buf, io->blkcnt * BLKSZ, 1, fp);
    assert(ret == 1);
  }
}
