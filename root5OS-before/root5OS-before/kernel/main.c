#include "include/types.h"
#include "include/param.h"
#include "include/memlayout.h"
#include "include/riscv.h"
#include "include/defs.h"
#include "include/sbi.h"

volatile static int boot_hart = -1;
extern void _entry();
void start_harts();

// start() jumps here in supervisor mode on all CPUs.
void
main()
{
  if(boot_hart == -1){
    boot_hart = cpuid();
    consoleinit();
    printfinit();
    printf("\n");
    printf("xv6 kernel is booting\n");
    printf("\n");
    kinit();         // physical page allocator
    kvminit();       // create kernel page table
    kvminithart();   // turn on paging
    procinit();      // process table
    trapinit();      // trap vectors
    trapinithart();  // install kernel trap vector
    plicinit();      // set up interrupt controller
    plicinithart();  // ask PLIC for device interrupts
    binit();         // buffer cache
    iinit();         // inode table
    fileinit();      // file table
    virtio_disk_init(); // emulated hard disk
    userinit();      // first user process
    //started = 1;
    __sync_synchronize();
    //启动其他核
    start_harts();
  } else {
    // while(started == 0)
    //   ;
    __sync_synchronize();
    printf("hart %d starting\n", cpuid());
    kvminithart();    // turn on paging
    trapinithart();   // install kernel trap vector
    plicinithart();   // ask PLIC for device interrupts
  }

  set_next_trigger();
  scheduler();        
}

void 
start_harts(){
  for(int i = 0;i < NCPU; i++ ){
    if(sbi_hart_get_status(i) == SBI_HSM_STATE_STOPPED){
      sbi_hart_start(i,(uint64)_entry,0);
    }
  }
}