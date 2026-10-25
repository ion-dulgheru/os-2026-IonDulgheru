# Lucrarea 6 – Sistemul de fișiere xv6

Dulgheru Ion, FAF-241. Raportul complet, cu capturi de ecran: `Raport_Lab6_SO.docx`. Codul este în `mystat.c`, `find.c` și `Makefile` (UPROGS: `_sleep`, `_args`, `_mystat`, `_find`).

## Partea 1 – Ce afișează xv6

1. Tipurile din `kernel/stat.h` sunt: 1 = T_DIR (director, de exemplu `.` și `a`), 2 = T_FILE (fișier, de exemplu README), 3 = T_DEVICE (dispozitiv, de exemplu console).
2. Rădăcina are inode-ul 1 (`. 1 1 1024` și `.. 1 1 1024`). `.` și `..` sunt identice, pentru că rădăcina nu are părinte, iar `..` indică spre ea însăși.
3. O intrare de director are `sizeof(struct dirent)` = 2 (`ushort inum`) + 14 (`char name[DIRSIZ]`) = 16 octeți, deci 32 / 16 = 2 intrări: `.` (inode 28, directorul `a`) și `..` (inode 1, rădăcina).

## Partea 2 – mystat

Ieșirile reale sunt în figura 3.3 din raport. Rezumat:

| Moment | Nume | Inode | Links | Size |
|---|---|---|---|---|
| înainte de ln | README | 2 | 1 | 2441 |
| după `ln README r2` | README, r2 | 2 | 2 | 2441 |
| după `rm README` | r2 | 2 | 1 | 2441 |

1. După `ln`, ambele nume au inode-ul 2 și links 2, deci sunt două nume pentru același fișier. După `rm README`, `r2` rămâne cu inode-ul 2 și links 1. Dimensiunea și conținutul nu s-au schimbat (`48 336 2441`), s-a schimbat doar contorul de legături.
2. În `sys_unlink` din `kernel/sysfile.c`, contorul scade la instrucțiunea `ip->nlink--;`. Blocurile se eliberează în `iput()` din `kernel/fs.c`: când nu mai există referințe și `ip->nlink == 0`, se apelează `itrunc(ip)`.
3. `struct dinode` are doar câmpurile `type`, `major`, `minor`, `nlink`, `size` și `addrs[NDIRECT + 1]`. Nu are proprietar, permisiuni sau timp.

Observații:
- Un director nou `a` are links 1, pentru că `.` nu se numără („No ip->nlink++ for '.': avoid cyclic ref count”), iar `..` din `a` mărește contorul părintelui.
- `mystat console` a dat „cannot stat console”, în timp ce `mystat ./console` a afișat `device, inode 27, links 1, size 0 bytes`. Cauza nu am identificat-o.

## Partea 3 – Limitele sistemului de fișiere

Constantele sunt BSIZE = 1024, NDIRECT = 12, NINDIRECT = 1024 / 4 = 256 și DIRSIZ = 14.

1. `ls` afișează `averyveryverylongname` ca `averyveryveryl`, adică numele e trunchiat la 14 caractere (DIRSIZ). De aceea `mkdir averyveryverylongother` dă „failed to create”: primele 14 caractere coincid.
2. F(max) = (NDIRECT + BSIZE / 4) · BSIZE = (12 + 256) · 1024 = 268 · 1024 = 274 432 octeți (268 KiB), adică MAXFILE = 268 de blocuri.
3. Linia `nmeta 47 (boot, super, log blocks 31, inode blocks 13, bitmap blocks 1) blocks 1953 total 2000` arată că discul are 2000 de blocuri: 31 pentru jurnal, 13 pentru inode-uri și 1953 pentru date. Metadatele ocupă 2 + 31 + 13 + 1 = 47 de blocuri.

## Partea 4 – find

Testul a fost făcut pe un disc curat (`make clean ; make qemu`), iar captura e figura 3.8 din raport. `find . r2` a găsit `./a/b/r2`, `./a/r2` și `./r2`, iar `find a b` a afișat `a/b`.

Stretch (`find . r2 -n`):

| Cale | Inode | Același fișier cu cat? |
|---|---|---|
| ./a/b/r2 | 30 | nu |
| ./a/r2 | 31 | nu |
| ./r2 | 3 | da |

`./r2` are inode-ul 3, același cu `cat`, deci e o legătură hard spre `cat`. Celelalte două sunt fișiere separate, create cu `echo`.
