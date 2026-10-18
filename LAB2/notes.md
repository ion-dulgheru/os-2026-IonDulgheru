# Lab 2 - xv6 Notes

## Part 2: Use xv6 as Unix

**(1) Trei programe din xv6:**
- cat - afiseaza continutul unui fisier (sau al stdin-ului) la ecran
- echo - afiseaza argumentele primite, urmate de newline
- wc - numara linii, cuvinte si octeti dintr-un fisier (word count)

**(2) Ce trebuie sa existe pentru ca pipe-ul (|) sa functioneze:**
Doua lucruri: (a) procese separate - shell-ul face fork() pentru fiecare
comanda din pipeline, deci "ls" si "grep c" ruleaza ca doua procese
independente, nu ca o singura functie; (b) un mecanism de comunicare
inter-proces - syscall-ul pipe() (vazut in user.h: int pipe(int*)) creeaza
un canal cu doua capete; shell-ul leaga capatul de scriere al lui ls la
capatul de citire al lui grep, inlocuind stdout-ul unuia cu stdin-ul
celuilalt. Fara aceste doua piese (procese + pipe ca obiect de kernel),
"|" nu ar avea cum sa transfere date intre doua programe separate.

**(3) Comparatie xv6 shell vs Linux shell (Lab 1):**
Shell-ul xv6 (sh.c) e un subset minimal, de cateva sute de linii de cod,
care intelege comenzi simple, redirectari si pipe-uri de baza; shell-ul
Linux (bash) are zeci de mii de linii si adauga job control, istoric,
autocomplete, variabile de mediu, scripting complex - dar ambele se
bazeaza pe aceleasi primitive de kernel: fork, exec, pipe.

## Part 3: Read the source

**(1) Apeluri de sistem folosite de user/cat.c si ce cer kernelului:**
- read(fd, buf, n) - cere kernelului sa copieze pana la n octeti din
  fisierul/descriptorul fd in bufferul buf din memoria procesului
- write(fd, buf, n) - cere kernelului sa scrie n octeti din buf catre
  descriptorul fd (de obicei 1 = stdout)
cat.c ruleaza intr-o bucla: read() intr-un buffer, apoi write() acelasi
buffer la iesire, pana cand read() intoarce 0 (fisier terminat). Ambele
sunt declarate in user/user.h ca prototipuri de syscall, nu functii de
biblioteca obisnuite - fiecare apel produce un trap catre kernel/.

**(2) Unde e implementat sys_read:**
kernel/sysfile.c, linia 69 (sys_write e imediat dupa, la linia 83). Aici
kernelul valideaza descriptorul de fisier primit de la procesul user,
gaseste structura de fisier corespunzatoare si copiaza datele efectiv.

**(3) Diferenta dintre codul din kernel/ si codul din user/:**
kernel/ ruleaza in modul privilegiat (supervisor) al procesorului, cu
acces direct la hardware - gestioneaza memoria, scheduler-ul intre
procese, sistemul de fisiere si raspunde la syscall-uri. user/ contine
programe obisnuite (cat, ls, echo, shell-ul insusi) care ruleaza
neprivilegiat si nu pot atinge hardware-ul direct - orice actiune reala
(citit un fisier, alocat memorie, terminat procesul) trece printr-un
syscall, adica o cerere explicita catre kernel/.



## Comenzi rulate

### Part 2 (in xv6)
$ echo hello xv6
hello xv6

$ ls | grep c
cat            2 3 36728
echo           2 4 35592
wc             2 17 37728
sync           2 22 34944
console        3 23 0

$ wc README
48 336 2441 README

### Part 3 (in Ubuntu, in ~/xv6-riscv)
$ grep -n "sys_read\|sys_write" kernel/sysfile.c | head
69:sys_read(void)
83:sys_write(void)

$ cat user/user.h
(lista completa de syscall-uri: fork, exit, wait, pipe, write, read,
close, kill, exec, open, mknod, unlink, fstat, link, mkdir, chdir, dup,
getpid, pause, uptime, sync)


