#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc != 2){
    fprintf(2, "usage: sleep <ticks>\n");
    exit(1);
  }
  // syscall-ul se numeste "pause" in aceasta copie de xv6
  pause(atoi(argv[1]));
  exit(0);
}
