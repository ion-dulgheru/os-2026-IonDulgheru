# Lab 10 notes

## Observe

1. În `trace 32 grep hello README` masca este 32 = 2^5, deci este activ doar bitul 5, adică `SYS_read`. Nucleul afișează un apel doar dacă bitul lui este activ: `(p->trace_mask >> num) & 1`. `open` are numărul 15 și `close` numărul 21, iar bitele lor nu sunt activate, deci nu apar. Apelurile se fac totuși; ele pur și simplu nu sunt afișate (cu masca 2147483647 apar `exec`, `open`, `read` și `close`).
2. Linia `np->trace_mask = p->trace_mask;` din `fork()` (kernel/proc.c). Fiecare copil moștenește masca părintelui, deci și procesele create de `forkforkfork` (pid 6, 7, 8, 9) afișează liniile `fork`. Fără ea ar afișa doar procesul care a apelat `trace`.
3. `echo hi` apelează `write` de două ori: o dată pentru `hi` (2 octeți, `-> 2`) și o dată pentru caracterul `\n` (1 octet, `-> 1`). Linia de trace se afișează după fiecare apel, imediat ce acesta revine, deci prima linie apare după `hi` și înainte de `\n`: `hi5: syscall write -> 2`, apoi linia nouă, apoi `5: syscall write -> 1`.
4. `open` este 15 și `close` este 21, deci masca este 2^15 + 2^21 = 32768 + 2097152 = **2129920**.

## Rezultate (rulate în xv6)

- `trace 32 grep hello README`: `read -> 1023`, `965`, `453`, `0` (1023 + 965 + 453 = 2441, dimensiunea lui README).
- `trace 2147483647 grep hello README`: `trace`, `exec`, `open`, 4 × `read`, `close`.
- `trace 65536 echo hi`: două apeluri `write`, `-> 2` și `-> 1`.
- `trace 2 usertests forkforkfork`: liniile `fork` de la mai mulți pid-uri, ultimele cu `-> -1` (tabela de procese plină), apoi `OK` și `ALL TESTS PASSED`.
