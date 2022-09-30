#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64 numfreemem();
uint64 numfreeproc();


uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Add a sys_trace() function
// output pattern-> PID: sys_$name(arg0) -> return_value
// demo example [ 3: sys_read(3) -> 1023 ]

uint64
sys_trace(void)
{
  int mask;
  // char st[32];
  // argstr(1, st, 32);
  if(argint(0, &mask) < 0) // argint here to get the first argument
    return -1;

  myproc()->mask = mask;  
  return 0;
}


uint64
sys_sysinfo(void)
{
/*
 1  struct sysinfo {
 2    uint64 freemem;   // amount of free memory (bytes)
 3    uint64 nproc;     // number of process 
 4    uint64 freefd;    // number of free file descriptor
 5  };
*/
  uint64 user; // user sysinfo结构体的指针
  struct sysinfo info;

  struct proc *p = myproc();

  if(argaddr(0, &user) < 0)
    return -1;
 // 接下来就是填结构体
  info.freemem  = numfreemem();
  info.nproc = numfreeproc();

 
  
  // 把填好的结构体传到用户空间
  if(copyout(p->pagetable, user, (char *)&info, sizeof(info)) < 0)
    return -1;

  return 0;
}
