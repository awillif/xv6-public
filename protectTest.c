#include "types.h"
#include "stat.h"
#include "user.h"
#include "mmu.h"

int
main(int argc, char *argv[])
{
  int parent = getpid();

  // Create an address to protect
  int *val = (int *) sbrk(0);

  // Make sure the process memory size is at least a page
  sbrk(PGSIZE);

  // Protect the address
  mprotect(val, 1);

  if (fork() == 0) {
    munprotect(val, 1);  
    *val = 5;
    printf(1, "TEST PASSED: unprotected value did not cause trap\n");
    exit();

  } else {
    wait();
  }

  if(fork() == 0) {
    sleep(5);
    *val = 5;
    printf(1, "TEST FALIED: protected value did not cause trap\n");
    kill(parent);
    exit();

  // Parent: wait for child
  } else {
    wait();
  }

  printf(1, "TEST PASSED: protected value caused trap\n");
  exit();

}
