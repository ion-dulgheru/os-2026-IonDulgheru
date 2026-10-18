# Lab 7 - Processes and the System-Call Interface (Linux)

## Part 1
1. Order: `pipe2([3,4])`, then two `clone` (children 12645 and 12646), then `dup2` and `execve` in each child, then `wait4` in the parent (bash, PID 12641). PID 12645 calls `dup2(4, 1)`: ls writes to descriptor 1, so it is redirected into the pipe write end. PID 12646 calls `dup2(3, 0)`: wc reads descriptor 0, so it is connected to the pipe read end.
2. `clone` creates the children (glibc `fork()` is a wrapper over `clone`).
3. `ls /usr/bin` makes only 3 `write` calls: with output redirected, stdout is fully buffered, so text is written in block-sized chunks, not line by line.

## Part 2
1. While the parent sleeps, the child shows STAT `Z` (`[zombie] <defunct>`). After the parent calls `waitpid`, the status is collected and the entry disappears.
2. Prediction: 8 (2^3). Real count: 8. Process tree: P has children A, B, C; A has children A1, A2; A1 has child A1a; B has child B1 (8 processes).
3. On a terminal stdout is line-buffered (`\n` flushes it). In a pipe it is fully buffered, so "before fork" stays in the buffer in process memory. `fork` copies that memory, so parent and child both print it when flushing at exit.

## Part 3
1. A successful `execvp` replaces code, data and stack of the process, so the old code no longer exists and the call never returns. The code after it runs only if exec failed.
2. 127 is the shell convention for "command not found". `bash -c nosuchprog; echo $?` prints 127 as well.
3. `pkill` sends signal 15, `SIGTERM` (`kill -l`: `15) SIGTERM`).

## Part 5
1. With the parent's `close(p[1])` commented out the program hangs. `wc` gets EOF only when all write ends of the pipe are closed. `/proc/<pid>/fd` showed the parent still holding fd 4 = `pipe:[65157]` (write) while wc had fd 0 = `pipe:[65157]` (read), so `read` never returned 0.
2. `dup2` changes the descriptor table of the calling process. If the shell redirected itself, it would lose its own stdout and would have to restore it after every command. In the child (between fork and exec) only the program about to run is affected.
