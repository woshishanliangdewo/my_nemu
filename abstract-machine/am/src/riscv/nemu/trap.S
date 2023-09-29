#define concat_temp(x, y) x ## y
#define concat(x, y) concat_temp(x, y)
#define MAP(c, f) c(f)

#if __riscv_xlen == 32
#define LOAD  lw
#define STORE sw
#define XLEN  4
#else
#define LOAD  ld
#define STORE sd
#define XLEN  8
#endif

#define REGS(f) \
      f( 1)       f( 3) f( 4) f( 5) f( 6) f( 7) f( 8) f( 9) \
f(10) f(11) f(12) f(13) f(14) f(15) f(16) f(17) f(18) f(19) \
f(20) f(21) f(22) f(23) f(24) f(25) f(26) f(27) f(28) f(29) \
f(30) f(31)

#define PUSH(n) STORE concat(x, n), (n * XLEN)(sp);
#define POP(n)  LOAD  concat(x, n), (n * XLEN)(sp);

#define CONTEXT_SIZE  ((32 + 3 + 1) * XLEN)
#define OFFSET_SP     ( 2 * XLEN)
#define OFFSET_CAUSE  (32 * XLEN)
#define OFFSET_STATUS (33 * XLEN)
#define OFFSET_EPC    (34 * XLEN)

; 这就是__am_asm_trap
; 首先将sp减去上下文大小
; 然后我们csrr三个存储器
; 然后store
.align 3
.globl __am_asm_trap
__am_asm_trap:
; xlen是4，然后32+3+1=36
; 36*xlen就是context的size
  addi sp, sp, -CONTEXT_SIZE

; 这是regs(push) ==> push(1) push(3)==>sd x1, (1*xlen)(sp)
  MAP(REGS, PUSH)
; 读csr中的值到rd中
  csrr t0, mcause
  csrr t1, mstatus
  csrr t2, mepc
; 通过sd将三个寄存器值放入sp的OFFSET中
  STORE t0, OFFSET_CAUSE(sp)
  STORE t1, OFFSET_STATUS(sp)
  STORE t2, OFFSET_EPC(sp)
; 将mstatus的第十七位值为1
 # 也就是mprv这个标志位，这一位标志的是被中断或者异常处理的程序是否可以访问本地存储器
  # set mstatus.MPRV to pass difftest
  li a0, (1 << 17)
  # 将a0和t1的值进行或并将结果放到t1中
  or t1, t1, a0
  # 将t1的值写道mstatus中
  csrw mstatus, t1

  mv a0, sp
  # 调用__am_irq_handle程序
  # 并将堆栈指针作为参数传递
  # 将堆栈指针的值作为参数传递给a0
  jal __am_irq_handle
; 恢复初始值
  LOAD t1, OFFSET_STATUS(sp)
  LOAD t2, OFFSET_EPC(sp)
  # 将这些值写回去
  csrw mstatus, t1
  csrw mepc, t2
  # REGS(pop) ==> pop( 1) pop(3) ==> ld x1,(1*xlen)(sp)
  MAP(REGS, POP)
  # 堆栈指针继续向上移动CONTEXT_SIZE的大小
  addi sp, sp, CONTEXT_SIZE
  # 推出中断函数，恢复程序执行
  mret
