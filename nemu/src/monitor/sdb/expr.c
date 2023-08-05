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

enum {
  TK_NOTYPE = 256, TK_EQ, TK_DEC

  /* TODO: Add more token types */

};
// rule是我们的规则定义，包括我们的regex规则和我们的token的类型
static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {"[0-9]",TK_DEC},
  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},
  {"\\*", '*'},
  {"/", '/'},
  {"\\(", '('},
  {"\\)", ')'},
  {"==", TK_EQ},        // equal
};
// 一共多少种规则
#define NR_REGEX ARRLEN(rules)
// regex_t是我们的匹配规则的数组
static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    // 把所有的rules都进行如下操作
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}
// Token是我们的各种类型的匹配
typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;
// 这里是make_token，我们首先定义了初始位置，然后得到我们的pmatch，以及我们token的数量
static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;
  // 只要我们的字符串不为空，我们就进行下列操作
  while (e[position] != '\0') {
    /* Try all rules one by one. */
    // 对于所有的regex进行尝试
    // 现在是第i个re
    // pmatch.rm_so == 0 保证的是我们只会进行一次编译过程
    for (i = 0; i < NR_REGEX; i ++) {
      // 根据我们上边得到的re，对e+position处的字符串进行处理（因为前边已经处理完了）， 然后regmatch_t结构体的长度是1，这个regmatch_t的结构体
      // 是我们的pmatch，而pmatch是上边定义的， eflags为0
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        // 这里根据我们上述的操作，我们得到了字串的开始指针
        char *substr_start = e + position;
        // 同样根据上述操作，将rm_eo作为了结束的标志，长度就是pmatch.rm_eo，这是所有子串的标志，并不是
        // 所谓的token匹配部分的，阿sir谨记哦;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        // position加的是长度，也就是说我们将开始加上长度就是下一个开始
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
           case '+':
           case '-':
           case '*':
           case '/':
           case '(' :
           case ')' :
           case TK_NOTYPE:
            break;
           case TK_DEC:
          	tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token++].str, substr_start, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            // 匹配token，把它们存入数组tokens
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


word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  if (make_token(e)) {
    //  *success = 
  }
  return 0;
}
