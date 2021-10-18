#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "pstat.h"
#include "spinlock.h"

struct processTable {
    struct spinlock lock;
    struct proc proc[NPROC];
};
extern struct processTable ptable;

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
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

int
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

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_settickets(void)
{
  int tickets;

  if(argint(0, &tickets) < 0 || tickets < 1)
    return -1;
  
  myproc()->tickets = tickets;
  return 0;
}

int
sys_getpinfo(void)
{
  struct proc *p;
  struct pstat *procInfo;

    // Get the process info pointer from user space in which information will be
    // stored
    if (argptr(0, (char **)(&procInfo), sizeof(procInfo)) < 0)
        return -1;

    // Acquire the process table lock
    acquire(&ptable.lock);
    int index = 0;

    // Get the process information and store it
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
        if (p->state == UNUSED) {
          procInfo->inuse[index] = 0;
          index++;
          continue;
        }

        procInfo->pid[index] = p->pid;
        procInfo->ticks[index] = p->ticks;
        procInfo->tickets[index] = p->tickets;
        procInfo->inuse[index] = 1;
        index++;
    }
    // Release the process table lock
    release(&ptable.lock);
    return 0;
}
