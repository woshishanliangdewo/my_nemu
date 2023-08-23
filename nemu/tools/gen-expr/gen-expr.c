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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
// unsigned int
"  printf(\"%%u\", result); "
"  return 0; "
"}";

static void gen_rand_expr() {
  switch(choose(3)){
    case(0): gen_num(); break;
    case(1): gen('('); gen_rand_expr(); gen(')'); break;
    default: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
  }
}

uint32_t choose(uint32_t n){
  srand((unsigned)(time(NULL)));
  sprintf(buf,"%d",(uint32_t)(rand()%n));
}

uint32_t gen_num(){
    sprintf(buf,"%d",choose(65536));
}

void gen_rand_op(){
  switch(choose(4)){
    case(0):
    sprintf(buf,"%c",'+');
    case(1):
    sprintf(buf,"%c",'-');
    case(2):
    sprintf(buf,"%c",'*');
    case(3):
    sprintf(buf,"%c",'/');
  }
}

void gen(char * c){
  sprintf(buf,"%c",c);
}



int main(int argc, char *argv[]) {
  // 根据我们的seed，生成一个srand随机数
  int seed = time(0);
  srand(seed);
  // loop就是循环次数
  int loop = 1;
  // 从argv【1】中得到
  // 默认argc为1，argv[0]为程序名称。
  // 如果输入一个参数，则argc为2，argv[0]为程序名称，argv[1]为输入的那个参数。
  // 以此可推出，多个输入参数的情况。
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();
// sprintf函数打印到字符串中（要注意字符串的长度要足够容纳打印的内容，否则会出现内存溢出）
//而printf函数打印输出到屏幕上。
//sprintf函数在我们完成其他数据类型转换成字符串类型的操作中应用广泛。
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);
// 执行 dos(windows系统) 或 shell(Linux/Unix系统) 命令，参数字符串command为命令名。
// 另，在windows系统下参数字符串不区分大小写。
// 说明：在windows系统中，system函数直接在控制台调用一个command命令。
// 在Linux/Unix系统中，system函数会调用fork函数产生子进程，由子进程来执行command命令，
// 命令执行完后随即返回原调用的进程。
    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;
// 利用system函数调用shell命令，只能获取到shell命令的返回值，
// 而不能获取shell命令的输出结果，那如果想获取输出结果怎么办呢？用popen函数可以实现。
// 函数说明：popen()会调用fork()产生子进程，然后从子进程中调用/bin/sh -c 来执行参数command 的指令。
// 参数type 可使用 "r"代表读取，"w"代表写入。依照此type 值，popen()会建立管道连到子进程的标准输出设备或标准输入设备，然后返回一个文件指针。
// 随后进程便可利用此文件指针来读取子进程的输出设备或是写入到子进程的标准输入设备中。
    fp = popen("/tmp/.expr", "r");
    
// assert的作用是现计算表达式 expression ，如果其值为假（即为0），那么它先向stderr打印一条出错信息，
// 然后通过调用 abort 来终止程序运行。
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}