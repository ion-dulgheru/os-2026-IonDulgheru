#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
int main(void)
{
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }
    if (pid == 0) {
        int fd = open("out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) { perror("open"); exit(1); }
        dup2(fd, 1);
        close(fd);
        execlp("ls", "ls", "-l", (char *)NULL);
        perror("execlp");
        exit(127);
    }
    waitpid(pid, NULL, 0);
    return 0;
}
