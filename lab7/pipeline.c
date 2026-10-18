#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main(void)
{
    int p[2];
    if (pipe(p) < 0) { perror("pipe"); return 1; }

    pid_t a = fork();
    if (a < 0) { perror("fork"); return 1; }
    if (a == 0) {                 /* ls -l /etc -> scrie in pipe */
        dup2(p[1], 1);
        close(p[0]); close(p[1]);
        execlp("ls", "ls", "-l", "/etc", (char *)NULL);
        perror("ls"); exit(127);
    }

    pid_t b = fork();
    if (b < 0) { perror("fork"); return 1; }
    if (b == 0) {                 /* wc -l <- citeste din pipe */
        dup2(p[0], 0);
        close(p[0]); close(p[1]);
        execlp("wc", "wc", "-l", (char *)NULL);
        perror("wc"); exit(127);
    }

    close(p[0]);
    close(p[1]);                  /* ESENTIAL: fara el, wc nu primeste EOF */
    waitpid(a, NULL, 0);
    waitpid(b, NULL, 0);
    return 0;
}
