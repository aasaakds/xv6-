#include "include/types.h"
#include "include/param.h"
#include "include/memlayout.h"
#include "include/riscv.h"
#include "include/defs.h"

void main();
// void timerinit();

// entry.S needs one stack per CPU.
__attribute__ ((aligned (16))) char stack0[4096 * NCPU];

// a scratch area per CPU for machine-mode timer interrupts.
uint64 timer_scratch[NCPU][5];

// assembly code in kernelvec.S for machine-mode timer interrupt.
extern void timervec();

// entry.S jumps here in machine mode on stack0.
void
start()
{

  // disable paging for now.
  // 此时是在 M 态
  w_satp(0);

  w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);
  // 进入 main.c 之前就变成了 S 态
  main();
}

