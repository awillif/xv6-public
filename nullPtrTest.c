#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char **argv) {
  uint* nullptr = (uint*) 0;
  printf(1, "%x %x\n", nullptr, *nullptr);
} 
