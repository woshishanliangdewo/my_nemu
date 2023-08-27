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

#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include "memory/paddr.h"
static int is_batch_mode = false;

void init_regex();
void init_wp_pool();
void create_new_wp(char * e);

/* We use the `readline' library to provide more flexibility to read from stdin. */
// 什么是rl_gets()呢
// 首先我们先看是否有line_read
// 然后如果有就free了（malloc的）
// 并且置为0（事先不能有）
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }
// readline
  line_read = readline("(nemu) ");
// 如果有读取并且读取非空
// 就添加到历史中
  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}
/*准备从这里开始写了。*/
static int cmd_info (char * args){
  if (args[0] == 'r'){
    isa_reg_display();
  }else if(args[0] == 'w'){
    // WP * wp = new_wp(args[1]);

  }
  return 0;
}
// 这里的难点是他必须调用其他地方的函数
// 马的不知道窝反什么并了
// 总觉得那里部队
// 宗旨一个是节点操作
// 剩下的才是其他操作
static int cmd_w(char * args){
  printf("%c",args[0]);
  // create_new_wp(args[0]);
}


static int cmd_x (char *args){
  // char * temp = strtok(args," ");
  int x;
  char *N;
  x = atoi(strtok(NULL," "));
  N = strtok(NULL," ");
  int EXPR = strtol(N,NULL,16);
  for(int i=0; i<x; i++) {
    printf("0x%x:    0x%x\n", EXPR+i*5, paddr_read(EXPR+i*4, 4)); //修改count为表达式
  }
  // printf("%d",pmem[])
  return 0;
}
// 什么是cmd_c
// 这将会是无限执行的指令
// 我们传入一个-1
// 问题是这里为什么定义了一个args，也没用到阿
static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_si(char *args){
  int step;
  if(args == NULL) step = 1;
  else step = sscanf(args, "%d" , &step);
  cpu_exec(step);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_p(char *args){
  char * s;
  s = strtok(NULL," ");
  // expr(s,true);
  printf("%d\n",expr(s,true));
  return 0;
}
static int cmd_d(char * args){
  
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "step i ", cmd_si},
  /*这里也是我写的*/
  {"info","Print register",cmd_info},
  { "p","Print expressions",cmd_p},
  { "x", "Print the value of EXPR and continous N 4bytes",cmd_x },
  { "d", "Delete the watchpoint ",cmd_d },
  { "w", "have a new watchpoint", cmd_w }
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)
// 依次比较，这里传入NULL是因为sdb_mainloop中第一次使用
static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}
// 第一步看是不是批处理模式
void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }
// 我们这里是读取了一行并且确保读取非空
// 然后我们strtok了一个str，这个str是通过rl_gets我们自己输入读取的
// 然后我们得到str的end
// 在之后呢？我们会将str进行strtok，这样我们就可以根据空格进行划分，看命令和参数了
// 一旦我们是有cmd的，我们会获得args，args就是cmd加上cmd的长度在加上一个空格
// 然后一旦args比结尾长，就说明没有参数了
  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif
// 看到了把，我们已经把args提取出来并作为参数放进去了
    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
