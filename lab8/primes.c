#include "kernel/types.h"
#include "user/user.h"

void stage(int in) __attribute__((noreturn));

void
stage(int in)
{
  int p, n;
  int fd[2];

  if(read(in, &p, sizeof(p)) != sizeof(p)){
    close(in);
    exit(0);
  }
  printf("prime %d\n", p);

  if(pipe(fd) < 0){
    printf("primes: pipe failed after prime %d\n", p);
    close(in);
    exit(1);
  }
  int pid = fork();
  if(pid < 0){
    printf("primes: fork failed after prime %d\n", p);
    close(in);
    close(fd[0]);
    close(fd[1]);
    exit(1);
  }
  if(pid == 0){
    close(fd[1]);     // the child only reads
    close(in);        // the parent's input is not needed
    stage(fd[0]);
  }
  close(fd[0]);       // the parent only writes
  while(read(in, &n, sizeof(n)) == sizeof(n)){
    if(n % p != 0)
      write(fd[1], &n, sizeof(n));
  }
  close(in);
  close(fd[1]);       // signals EOF to the next stage
  wait(0);
  exit(0);
}

int
main(int argc, char *argv[])
{
  int fd[2];
  if(pipe(fd) < 0){
    fprintf(2, "primes: pipe failed\n");
    exit(1);
  }
  if(fork() == 0){
    close(fd[1]);
    stage(fd[0]);
  }
  close(fd[0]);
  for(int i = 2; i <= 35; i++)
    write(fd[1], &i, sizeof(i));
  close(fd[1]);
  wait(0);
  exit(0);
}
