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

#include "sdb.h"

#define NR_WP 32

// 如果这是一个结构体，那么设置为一个数组有什么意义呢
// 很显然，大家都知道，如果一个东西是一个数组的话，
// 他的特点就是下标决定了一切，怎么会与next指针有关呢
// 可是与此同时呢，如果与next指针无关，tmd
// next指针又有什么用呢？
typedef struct watchpoint {
  // 监视点的序号
  int NO;
  // 下一个的指针，这不就是链表么
  struct watchpoint *next;
  char * expr ;
  int old;
  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
// 头指针，尾指针
// 其中head用于组织使用中的监视点结构
// free_用于组织空闲的监视点结构
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
// 唯一的，也是最大的问题是，怎么才能让一个正向的链表有这种功能呢
// 有一种好方法，你还真别说
// 让链表反过来，哈哈哈
// 应该分开，数组是数组，为了计算机虚拟化
// 链表是链表，与数组无关了
// https://blog.csdn.net/qq_45655405/article/details/108941025
WP* new_wp(){
  printf("yes");
   WP * wp = free_;
   WP * tmp = head;
   if(free_->next != NULL)
   {
    free_ = free_->next;
   }else {
    panic("监视点已经使用完了");
   }
   if(head == NULL){
    head = wp;
    wp->next = NULL;
   }else {
      while(tmp->next != NULL){
        tmp = tmp->next;
      }
        tmp->next = wp;
        wp->next = NULL;
    }
    return wp;
   }

void free_wp(WP* wp){
  WP* temp =wp;
  if(wp == head)
  {
    head = wp->next;
  }
  else {
    while(temp->next!= wp){
      temp = temp->next;
    }
    temp->next = wp->next;
  }
  wp->next = free_->next;
  free_->next = wp;
}

void create_new_wp(char *expression,int value){
  printf("wat           \n");
    WP* wp = new_wp();

    // wp->expr = expression;
    strcpy(wp->expr, expression);

    wp->old = value;
    printf("你已经成功添加了哦\n");
}

void delete_wp(int no){
    WP * wp = &wp_pool[no];
    free_wp(wp);
    printf("成功删除监视点\n");
}

int check_wp(){
  for(int i=0;i<NR_WP;i++)
  {
    int new = expr(wp_pool[i].expr,false);
    if(new != wp_pool->old){
      return i;
    }
  }
  return -1;
}

void wp_show(){
  WP * tmp = head;
  if(!tmp){
    printf("没有监视点\n");
    return;
  }
  while(tmp){
    printf("%-8d%-8s\n", tmp->NO, tmp->expr);
    tmp = tmp->next;
  }
  
}

