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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

#define NR_REGEX ARRLEN(rules)

enum {
  TK_NOTYPE = 256, TK_DEC, TK_NEG
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {
  {"[0-9]+", TK_DEC},     // dec
  {" +", TK_NOTYPE},      // spaces
  {"\\*", '*'},           // mul
  {"/", '/'},             // div
  {"\\(", '('},           // bra1
  {"\\)", ')'},           // bra2
  {"-", '-'},             // sub
  {"\\+", '+'},           // plusd
};

static regex_t re[NR_REGEX] = {};

void init_regex() {
  int i;
  char error_msg[128];
  int ret;
  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    // 把我们自己订的规则rules存入re数组
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

// 一共多少种规则
// regex_t是我们的匹配规则的数组

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */

// Token是我们的各种类型的匹配
typedef struct token {
  int type;
  char str[32];
} Token;
//在gcc手册中找到了有关的解释：
//unused：This attribute, attached to a function, means that the function is meant to be
//         possibly unused. GCC will not produce a warning for this function.

// 表示该函数或变量可能不使用，这个属性可以避免编译器产生警告信息。
// ===============================================================================
// used： This attribute, attached to a function, means that code must be emitted for the
//        function even if it appears that the function is not referenced. This is useful,
//        for example, when the function is referenced only in inline assembly.

// 向编译器说明这段代码有用，即使在没有用到的情况下编译器也不会警告！

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;
// 这里是make_token，我们首先定义了初始位置，然后得到我们的pmatch，以及我们token的数量
static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;
  nr_token = 0;
  while (e[position] != '\0') {
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        // 把字符串逐个识别成token，存到pmatch
        char *substr_start = e + position;
        // 把token对应的起始字符串地址存入substr_start
        int substr_len = pmatch.rm_eo;
        // 把token长度存入substr_len
        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;
        switch (rules[i].token_type) {
          case '+':
            tokens[nr_token].type = rules[i].token_type;
          case '-':
          case ')':
          case '(':
          case '/':
          case '*':
          case TK_DEC:
          	tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token++].str, substr_start, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            // 匹配token，把它们存入数组tokens
            break;
          case TK_NOTYPE:
            break;
        }
        break;
      }
    }
    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  return true;
}


int check_parentheses(Token* start, Token* end) {
  int sign = 0;
  int count = 0;
  if (start->type!='(' || end->type!=')' ) {
    return false;
  }
  for(Token* sym = start; sym<end; sym++) {
    if(sym->type == '(') {
      count++;
    }else if(sym->type ==')') {
      count--;
    }
    if(count==0) {
      sign=1;
    }
  }
  if(count==1&&sign==0) {
    return true;
  }
  if(count==1&&sign==1) {
    return false;
  }
  panic("Error expression");
}





