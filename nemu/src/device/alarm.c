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
#include <device/alarm.h>
#include <sys/time.h>
#include <signal.h>

#define MAX_HANDLER 8

//将alram的handler设置为数组
static alarm_handler_t handler[MAX_HANDLER] = {};
static int idx = 0;

// 添加一个idx，使用handler进行储存，然后储存结果为h
void add_alarm_handle(alarm_handler_t h) {
  assert(idx < MAX_HANDLER);
  handler[idx ++] = h;
}

// 这是alarm的一个函数，这是函数指针
static void alarm_sig_handler(int signum) {
  int i;
  for (i = 0; i < idx; i ++) {
    handler[i]();
  }
}

// 然后我们初始化alarm
// 首先是结构体sigaction
// 然后是初始化结构体
// 然后我们让ret位sigaction
// 然后我们有一个itimerval

void init_alarm() {
  struct sigaction s;
  memset(&s, 0, sizeof(s));
  s.sa_handler = alarm_sig_handler;
  int ret = sigaction(SIGVTALRM, &s, NULL);
  Assert(ret == 0, "Can not set signal handler");

  struct itimerval it = {};
  it.it_value.tv_sec = 0;
  it.it_value.tv_usec = 1000000 / TIMER_HZ;
  it.it_interval = it.it_value;
  ret = setitimer(ITIMER_VIRTUAL, &it, NULL);
  Assert(ret == 0, "Can not set timer");
}
