# Lab 3 – The Command Line: Shell, Pipes and Text Filters

Dulgheru Ion, FAF-241. Full report with screenshots: `Lab3_DulgheruIon_FAF241.docx`.

## Part 1 – Moving fast and finding help

Commands: `type cd ; type ls ; type echo ; which ls`, `echo $PATH | tr ':' '\n'`, `nosuchprog ; echo "exit code: $?"`, `man ls`, `ls --help | head`, `man -k "list directory"`, `help cd`.

Observed: `cd` and `echo` are shell builtins, `ls` is an alias of `ls --color=auto`, and `which ls` gives `/usr/bin/ls`. `nosuchprog` prints "command not found" with exit code 127. `ls --help` and `man -k` (rust-ls, rust-dir, rust-vdir) show that `ls` on this system comes from uutils coreutils (Rust), not GNU coreutils.

1. `cd` and `echo` are built-ins, while `ls` is an external program (the alias only adds colours). `cd` must be a built-in because an external program runs in a child process, and a child can change only its own working directory. The working directory of the shell would stay the same.
2. PATH has 16 entries, of which only 12 are distinct: `/home/ion/.local/bin` appears three times, and `/snap/bin` and `/home/ion/.dotnet/tools` appear twice each. `ls` is in `/usr/bin`.
3. A command that is not found returns exit code 127.
4. `-t` sorts by modification time, newest first (line 142 of `man ls`: "Sort by modification time (the 'mtime' in the inode), newest first").

## Part 2 – What the shell does to a command line

Test folder `~/os-lab3/play` with `a.c b.c file1.txt file2.txt notes.txt "alfa beta.txt"`.

| Command | Prediction | Real output | Note |
|---|---|---|---|
| `echo *.c` | a.c b.c | a.c b.c | matches |
| `echo file?.txt` | file1.txt file2.txt | file1.txt file2.txt | `?` matches one character |
| `echo [a-b]*` | a.c alfa beta.txt b.c | a.c alfa beta.txt b.c | every name starting with a or b |
| `echo *.xyz` | \*.xyz | \*.xyz | no match, so the pattern stays literal |
| `ls alfa beta.txt` | two errors | cannot access 'alfa' and 'beta.txt' | the space splits the name in two |
| `ls "alfa beta.txt"` | alfa beta.txt | 'alfa beta.txt' | ls quotes a name that has a space |
| `echo '$HOME' "$HOME" \$HOME` | $HOME /home/ion $HOME | $HOME /home/ion $HOME | only double quotes expand |
| `echo {1..5} ; echo $((7 * 6))` | 1 2 3 4 5 and 42 | 1 2 3 4 5 and 42 | brace and arithmetic expansion |
| `echo "today is $(date +%A), I am $(whoami)"` | today is Monday, I am ion | today is Monday, I am ion | command substitution |
| `touch log{1..3}.txt && ls log*` | log1.txt log2.txt log3.txt | log1.txt  log2.txt  log3.txt | created first, then listed |

1. The predictions, the real outputs and the notes are in the table above.
2. The file names were produced by the shell. It expands `*.c` to `a.c b.c` before `echo` starts, and `echo` only prints the arguments it receives. That is also why `echo *.xyz` prints the pattern itself.
3. The shell split the line at the space, so `ls` received two arguments, `alfa` and `beta.txt`, and neither exists. There are two fixes: `ls "alfa beta.txt"` or `ls alfa\ beta.txt`.
4. Single quotes keep everything literal (`'$HOME'` stays `$HOME`). Double quotes stop word splitting and globbing but still expand variables and `$(...)` (`"$HOME"` becomes `/home/ion`). The backslash protects the next character.

## Part 3 – Redirection, chaining and jobs

Outputs: `wc -l out.txt` gives 246 lines. `err.txt` has the message for `/nope`. `notes.md` contains `first` and `second`. `wc -l < access.log` gives 500. `ls /nope 2> /dev/null || echo "failed with code $?"` gives `failed with code 2`. `sleep 100 &` followed by `jobs ; kill %1 ; jobs` started the job and then stopped it.

1. The listing of `/etc` (246 lines) went to `out.txt`, and the message "cannot access '/nope'" went to `err.txt`.
2. With `> all.txt 2>&1`, stdout is redirected to the file first, and then stderr goes where stdout now points, so both streams end up in the file. With `2>&1 > all.txt`, stderr is first copied to the current stdout (the terminal) and only then is stdout redirected. So the error appeared on the screen and the file held only the listing.
3. With `<`, the shell opens the file and gives it to `wc` on stdin, so `wc` never learns the file name. With `wc -l access.log`, the name arrives as an argument, so it is printed.
4. `;` runs the commands one after the other whatever happens. `&&` runs the second only if the first succeeded (exit code 0). `||` runs the second only if the first failed (non-zero exit code).

## Part 4 – One-liners (`oneliners.sh`)

| Q | Answer |
|---|---|
| Q1 | 100 failed requests |
| Q2 | 4 pages: /page0, /page1, /page2, /page3 |
| Q3 | 125 requests for each page (4 × 125 = 500) |
| Q4 | user3 and user5, 15 failed requests each (head -3 also shows user6 with 14) |
| Q5 | user3 made 72 requests, 15 of them failed |
| Q6 | 10:10 user0, 10:15 user5, 10:20 user3 |
| Q7 | 41 × /usr/sbin/nologin, 5 × /bin/false, 3 × /usr/bin/false, 2 × /bin/bash, 1 × /bin/sync |
| Q8 | 245 and 246 lines |

Self-check: the Q3 counts add up to 500. The failures per user (five users with 14, two with 15) add up to 100, the same as Q1.

Q8: `ls -l` prints one extra first line, `total N` (the disk blocks used by the listed files), so its line count is exactly one more than that of `ls`.

Stretch: `grep -E ' 10:[0-9]0 user[0-9] FAIL ' access.log | wc -l` gives 50. A request fails when its line number i is a multiple of 5, and the minute ends in 0 when i is a multiple of 10, so N = 500 / 10 = 50.

## Part 5 – `report.sh`

Output: `500 requests, 100 failed`, then the failures of user0–user6: 14, 14, 14, 15, 14, 15, 14. `./report.sh nope.log` prints `no such file: nope.log` with exit code 1. Before `chmod +x`, the script failed with "Permission denied".

1. `#!/bin/bash` tells the kernel which interpreter runs the file. Without `chmod +x` the file has no execute permission, so `./report.sh` gives "Permission denied".
2. The current directory is not in PATH, so `report.sh` alone would be searched only in the PATH directories and give "command not found".
3. `>&2` sends the message to stderr, so it does not mix with the normal output when that output is redirected or piped. `exit 1` returns a non-zero status, so the caller can detect the failure with `$?`, `&&` or `||`.
