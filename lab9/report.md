# Lab 9 - Watching System Calls with strace

Tool: strace 6.19 (`screenshots/1_strace_version.png`).

## Part 1 - the calls of a real command

Command: `strace -f -e trace=openat,read,write,close ls -l`
(`screenshots/2_ls_trace.png`)

Count with `strace -f -c ls -l` (`screenshots/3_ls_count.png`): one simple
`ls -l` makes about **289 system calls** in total (9 of them fail). The most
frequent are `close` (35), `getdents64` (34), `mmap` (32), `openat` (32),
`fstat` (30) and `read` (22). Most are start-up work: loading the dynamic
linker and libc, reading `/etc/passwd`, `/etc/group` and the timezone files,
and `getdents64`/`statx` to read the directory.

**Observe: which system call does `ls` use to print, and to which descriptor?**

`ls` prints with the **`write`** system call, to descriptor **1** (stdout, the
screen):

    write(1, "total 328\n-rw-rw-r-- 1 ion ion 3"..., 312) = 312

## Part 2 - a program that calls the kernel directly

`syscalls.c` (`screenshots/4_syscalls_c.png`) compiled with
`gcc -Wall -Wextra -o syscalls syscalls.c`, no warnings. Output
(`screenshots/5_run_syscalls.png`):

    open returned descriptor 3
    write sent 14 bytes
    read got 14 bytes: hello, kernel

`strace -e trace=openat,write,read,close ./syscalls`
(`screenshots/6_strace_syscalls.png`) shows my C calls as real system calls:

    openat(AT_FDCWD, "note.txt", O_WRONLY|O_CREAT|O_TRUNC, 0644) = 3
    write(3, "hello, kernel\n", 14)         = 14
    close(3)                                = 0
    openat(AT_FDCWD, "note.txt", O_RDONLY)  = 3
    read(3, "hello, kernel\n", 63)          = 14
    close(3)                                = 0

Notes:
- C's `open()` became `openat(AT_FDCWD, ...)`, the modern Linux call;
  `AT_FDCWD` means "relative to the current directory".
- `read` asked for 63 bytes (`sizeof(buf) - 1`) but got 14, the file size.
- The `write(1, ...)` lines between them are my `printf`s going to the screen.
- The first lines (`/etc/ld.so.cache`, `libc.so.6`) are the setup: the
  program loads the C library before `main` runs.

**Observe: why is the first descriptor 3 and not 0, 1 or 2?**

Every process starts with three descriptors already open, inherited from the
shell: **0 = stdin** (keyboard), **1 = stdout** (screen), **2 = stderr**
(screen, for errors). The kernel always gives a new file the **lowest free
descriptor number**; 0, 1, 2 are taken, so the first new file gets **3**.
After `close(3)` the number 3 is free again, which is why the second `openat`
also returns 3.
