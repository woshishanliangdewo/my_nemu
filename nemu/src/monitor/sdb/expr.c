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

static int DEREF = 1;


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
// 这里需要注意的问题是我们的position是什么？
// 在这个循环中，重要的是循环的过程，首先position=0，所以一开始的时候，e+position就是e
// 那么什么是e呢，e就是我们进行make_token的时候，同时也是传入的字符串的开始
// 之后呢，pmatch.rm_so会变为0，
// 然后我们会使用通过e+position得到我们的子串的开始？
// 最开始的时候子串就是原本的字符串
// 然后我们得到长度，因为是eo-ro而且ro为0
// 之后position加上长度，就是下一个子串的起始的偏移了
// 然后e+position就是起始位置
// 其中nr_token就是我们的每次记录下来一个token后，token就加1
// tokens是我们用来记录的，rules是我们一开始定义的



// 这里还是很重要的，我必须把我的理解认认真真的写出来
// 首先我们要知道的是为什么要有pmatch.rm_so == 0，这就是说，因为rm_so是我们的起始地址
// 如果我们的字串是+456的话，那么我们的rm_so是1，rm_eo是4，而len是3，这是因为
// 我们匹配123+456的时候，第一次会把123匹配出来，剩下的就是+456了，所以会这样
// 正因此我们使用了pmatch.rm_so为0的联合平匹配法，这保证了我们在到了+456的时候会因为+456
// 不是rm_so为0而转而选择下一个匹配规则，最后到了\\+，于是匹配了+，如此往复
// 最后的结果就可以是正确的了，终于明白了
// 还有，定义了regcomp，一定tmd要用，不然会说地址越界了，tmd曹了，一直报错我说呢，因为只定义没使用，曹
// Token是我们的各种类型的匹配
int count = 0;
static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;
  nr_token = 0;
  // 正因为第一次的break了，所以我们才把position设置为全局变量，这样他就可以随意糟蹋了
  // switch case这个东西太恐怖了，wc
  // 什么情况，如果我们的case不写break的话，会有很重要的事情，
  // switch...case的三个规则：
  //（1）既无成功匹配，又无default子句，那么swtich语句块什么也不做；
  //（2）无成功匹配，但有default，那么swtich语句块做default语句块的事；
  //（3）有成功匹配，没有break，那么成功匹配后，一直执行，直到遇到break。

  // 通俗一点说，就是：
  // 我只找我想要的case，其他的都不管（直接注释掉这个case前面的所有语句）。
  // 找到我想要的case后，我就不再看其他case了（直接注释掉这个case后面的其他"case"关键字）。
  // 找到我想要的case后，我只管执行后面代码，直到遇到break后跳出switch语句块。

  // 因为我们是一个str，所以我们储存的是一长串的字符串，正因此，我们每次的结果相同
  // 因为str[0]相同，可是别的东西不同阿！
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
            strncpy(tokens[nr_token++].str, substr_start, substr_len);
            break;     
          case '-':
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token++].str, substr_start, substr_len);
            break;     
          case ')':
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token++].str, substr_start, substr_len);         
            break;     
          case '(':
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token++].str, substr_start, substr_len);         
            break;     
          case '/':
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token++].str, substr_start, substr_len);
             break;     
         case '*':
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token++].str, substr_start, substr_len);    
            break;     
          case TK_DEC:
          	tokens[nr_token].type = rules[i].token_type;
            // 用%c不行，因为大于界限了，用%s也不行，因为enum不是字符串
            strncpy(tokens[nr_token++].str, substr_start, substr_len);
            // 匹配token，把它们存入数组tokens
            break;
          case TK_NOTYPE:
            break;
        }
        // 这个break是for循环的，也就是说识别到了一个的话就会停止然后进行下一个步骤的循环
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
// 总结一下，在循环中的break语句,无论有多少层for循环，
// break语句永远只跳出自己所在那一层循环，即写在那一层循环就跳出那一层循环。
// 也就是说如果有两层循环的话，很有意思的一点是这个循环会进行很多次，但是
// 内部的循环只会进行一次

bool check_parentheses(int p,int q){
    if(tokens[p].type != '(' || tokens[q].type != ')'){
      return false;
    }
    int i = p,j = q;
    int lcount = 0, rcount = 0;
      while(i<j){
        if(tokens[i].type == '(' && tokens[j].type== ')'){
          lcount+=1,rcount+=1;
          i++;
          j--; 
        }else if(tokens[i].type != '(' && tokens[j].type== ')' ){
          rcount+=1;
          j--;
        }else if(tokens[i].type == '(' && tokens[j].type != ')'){
          lcount+=1;
          i++;
        }
      }
        if(lcount == rcount){
          return true;
        }else{
          return false;
        }
      
}


int max(int a,int b){
  return a>b? a:b;
}

int eval(int p, int q)
{
  printf("%d\n",p>q);
  if(p>q){
    // printf("It looks like that this expression is wrong");
    return ;
  }
  // // else {
  // //   printf("123");
  // // }
  // else if(p == q){
  //   return atoi(tokens[p].str);
  // } else if (check_parentheses(p, q) == true) {
  //   /* The expression is surrounded by a matched pair of parentheses.
  //    * If that is the case, just throw away the parentheses.
  //    */
  //   printf("p<q");
  //   return eval(p + 1, q - 1);
  // }
  // else if(p<q){
  //   printf("hh");
  //   bool flag = false;
  //   int op = -1;
  //   int i=0;
  //   for(int i=p;i<=q;i++){
  //       if(!false && tokens[i].type == TK_DEC){
  //         i++;
  //       }
  //       if(tokens[i].type == '('){
  //         while(tokens[i++].type != ')');
  //         op = i;
  //       }
  //       if(!flag && (tokens[i].type == '+' || tokens[i].type == '-')){
  //         flag = true;
  //         op = max(op,i);
  //       }
  //       if(!flag && tokens[i].type == '*' || tokens[i].type == '/'){
  //         flag = true;
  //         op = max(op,i);
  //       }
        
  //   }
  //   printf("%d",op);
  //   printf("%s",tokens[op]);
  //   // int val1 = eval(p, op - 1);
  //   // int val2 = eval(op + 1, q);

  //   // switch (tokens[op].type) {
  //   //   case '+': return val1 + val2;
  //   //   case '-': return val1 - val2;
  //   //   case '*': return val1 * val2;
  //   //   case '/': return val1 / val2;
  //   //   default: assert(0);
  //   /* We should do more things here. */
    
  // // }
  // }
}

int expr(char * e,bool *success){
int i;
if (!make_token(e)) {
  *success = false;
  return 0;
}
// /* TODO: Implement code to evaluate the expression. */

// for (i = 0; i < nr_token; i ++) {
//   if (tokens[i].type == '*' && (i == 0 || tokens[i - 1].type == '-' || \
//                                           tokens[i - 1].type == '+' || \
//                                           tokens[i - 1].type == '*' || \
//                                           tokens[i - 1].type == '/' || \
//                                           tokens[i - 1].type == '(')) 
//   {
//     tokens[i].type = DEREF;
//   }
// }
  Log("%d",tokens[1].str);
  Log("yes1");
  return eval(0, nr_token);

}



