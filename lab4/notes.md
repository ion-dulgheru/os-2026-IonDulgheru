# Lab 4 – The xv6 Shell from the Outside

Dulgheru Ion, FAF-241. Full report with screenshots: `Lab4_IonDulgheru_FAF-241.docx`.

## Part 1 – Fixing sleep

`grep -n "pause\|sleep" user/user.h` finds `int pause(int);` (line 25) and no `sleep`, so the call is now named `pause`. My `sleep.c` from Lab 2 already called `pause()`. `sleep 20` pauses and returns to the prompt, and `sleep` with no argument prints `usage: sleep <ticks>`.

## Part 2 – Bash versus the xv6 shell

| Command | Tests | bash | xv6 | Why |
|---|---|---|---|---|
| `echo hello > f ; cat < f` | redirection | hello | hello | both shells implement < and > |
| `ls \| grep READ` | pipe | README | README 2 2 2441 | both have pipes; xv6 ls also prints type, inode and size |
| `(echo a; echo b) \| wc` | grouping, list | 2 2 4 | 2 2 4 | both support ( ) and ; |
| `echo one ; echo two` | sequence | one / two | one / two | ; runs the commands in order |
| `echo bg &` | background | [1] 12134 and bg | bg | both implement & with fork and no wait |
| `ls *` | globbing | README f | ls: cannot open * | xv6 does not expand *, so ls gets the literal * |
| `echo $HOME` | variables | the home directory path | $HOME | xv6 has no variables |
| `echo "a b"` | quotes | a b | "a b" | xv6 does not process quotes |
| `mkdir d ; cd d ; ls ..` | PATH | README d f | exec ls failed (typed as separate lines) | xv6 has no PATH: ls is looked up in d |
| arrow up, Tab, Ctrl+R | history, completion | recall, complete, search | ^[[A is printed and exec fails; Tab inserts a tab | xv6 reads a plain line |

1. Both shells have redirection (<, >, >>), pipes (|), lists (;), grouping with ( ) and background jobs (&). Only bash has pattern expansion (* ? [ ] { }), variables and `$( )`, quote processing, a search path (PATH), history, completion and reverse search, `&&` and `||` with exit codes, job control and scripting.
2. xv6 calls `exec(argv[0], argv)` with the typed name and does not search any list of directories. After `cd d`, ls is looked up in d, which has no file ls, so the shell prints `exec ls failed`. The xv6 programs live in the root directory, so `/ls` (or `/ls ..`) works from inside d.
3. I would miss command history with completion most, because every command has to be typed again in full. The easiest to add is a search path: when exec fails, `runcmd` could retry with `/` in front of the name, which is a few lines of code.

## Part 3 – Who expands *

| Line | xv6 (`args`) | bash (`./args`, folder with a.c and b.c) |
|---|---|---|
| `args one              two         three` | argc = 4: one, two, three | argc = 4: one, two, three |
| `args *.c "x y"` | argc = 4: *.c, "x, y" | argc = 4: a.c, b.c, x y |
| `args $HOME` | argc = 2: $HOME | argc = 2: the home path |
| `args 'single quoted'` | argc = 3: 'single, quoted' | argc = 2: single quoted |

1. For `args *.c "x y"`, xv6 received 4 arguments: `args`, `*.c`, `"x` and `y"`. Linux also received 4: `./args`, `a.c`, `b.c` and `x y`.
2. The shell expands `*` and removes the quotes, not the program and not the kernel. The program only receives the finished argv, and the kernel copies that list into the new process during exec. The xv6 shell does neither, so the program saw `*.c` and the quote marks unchanged.
3. The extra spaces disappeared because both shells treat whitespace only as a separator between words, so the program received exactly one, two and three.

## Part 4 – Reading sh.c

`sh.c` has 499 lines. `#define` gives EXEC, REDIR, PIPE, LIST and BACK (lines 8–12) and MAXARGS 10. The cases are run in `runcmd` (lines 75–125), and cd is handled at lines 166–170.

1. The five kinds are EXEC (a plain word), REDIR (< > >>), PIPE (|), LIST (;) and BACK (&). Parentheses only group commands for the parser and are not a separate kind.
2. cd is handled in the main loop (lines 166–170): if the line starts with `cd `, the shell calls `chdir` itself. It cannot be a separate program, because the current directory belongs to the process: a child would change only its own directory and then exit.
3. sh.c has no code for `*` or `$`. The parser knows only `< | > & ; ( )`, and whitespace separates words. This explains Part 2: `ls *` and `echo $HOME` pass the text unchanged to the programs.
