## Info sistem
whoami -> ion
uname -a -> Linux ion-vitius 7.0.0-34-generic #34-Ubuntu SMP PREEMPT_DYNAMIC Wed Sep 2 14:29:37 UTC 2026 x86_64 GNU/Linux
uptime -> 16:35:53 up 5 min, 1 user, load average: 1.54, 1.40, 0.69

## Part 1: Files and directories
- Owner/grup fisiere create: ion / ion
- Ce inseamna cele 10 caractere dupa chmod 600: -rw------- ; tipul fisier obisnuit (-) + trei triplete rwx pentru owner/grup/altii; la 600 doar owner-ul are read+write, grupul si ceilalti nu au niciun drept
- Doua directoare din / si ce contin: etc - fisiere de configurare ale sistemului ; home - directoarele personale ale utilizatorilor
- Output interesant: ls -l note.txt inainte -> -rw-r--r--  ; dupa chmod 600 -> -rw-------
## Part 2: Processes
PID proces 1 și ce este: 1 — systemd
Nr. procese pe sistem: 342
"State:" din /proc/<pid>/status: S (sleeping)
Output interesant: ex. rândul ion 9133 5796 0 16:50 pts/1 00:00:00 sleep 300 din ps -ef | grep sleep

## Part 3: Memory
RAM totală/liberă: 14Gi total, 7.6Gi liberă (din free -h) — sau, din /proc/meminfo: MemTotal: 15472968 kB ≈ 15GB, MemFree: 8020004 kB ≈ 8GB
Ce e swap și cât e configurat: swap = spațiu pe disc folosit ca extensie a RAM-ului, când memoria fizică se umple; configurat 4.0Gi, folosit 0B (nu a fost nevoie de el)
VmRSS pentru sleep: 8036 kB (~8MB) — surprinzător de mult pentru un proces care „nu face nimic", dar tot foarte puțin comparat cu un proces normal (browser, IDE etc.)
Output interesant: 
              total        used        free      shared  buff/cache   available
Mem:           14Gi        3.9Gi       7.6Gi       697Mi        4.0Gi        10Gi
Swap:         4.0Gi          0B        4.0Gi
## Part 4: Devices and storage
Pe ce device e montat /: /dev/sda1 (ext4, 218.9G)
O intrare din /dev și ce reprezintă: console — fișierul special ce reprezintă consola sistemului (acolo ies mesajele kernelului/boot-ului)
Ce înseamnă "everything is a file": discurile (/dev/sda1), procesele (/proc/<pid>), terminalul (console) — toate sunt accesibile prin căi de fișier obișnuite, citibile cu cat/ls, fără comenzi speciale diferite pentru fiecare tip de resursă
Output interesant: 
sda      8:0   0  223.6G  0 disk
├─sda1   8:1   0  218.9G  0 part /

## Sinteza finala
Sistemul de operare gestioneaza patru resurse: fisierele (vazute cu ls -l),
procesele (vazute cu ps aux), memoria (vazuta cu free -h) si dispozitivele/
stocarea (vazute cu lsblk). Fiecare resursa e expusa uniform prin fisiere
sau comenzi simple, ceea ce confirma rolul OS-ului de manager central
intre programe si hardware.
