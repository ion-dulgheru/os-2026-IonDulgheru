# Lucrarea 5 – Sistemul de fișiere în practică (Linux)

Dulgheru Ion, FAF-241. Raportul complet, cu capturi de ecran: `Raport_Lab5_SO.docx`. Toate comenzile au fost rulate ca utilizatorul `ion` (UID 1000), pe gazda `ion-vitius`.

## Partea 1 – Arborele și tipurile de fișiere

1. `/etc` este `d` (director), `/etc/passwd` este `-` (fișier obișnuit), `/bin` este `l` (legătură simbolică), `/dev/null` este `c` (dispozitiv caracter), iar `/tmp` este `d` (director cu bit sticky, `t`).
2. `/bin` este o legătură simbolică: `/bin -> usr/bin`.
3. Ca dispozitiv caracter am găsit `/dev/null` (și console, full, random), iar ca dispozitiv bloc `/dev/loop0` (b, major 7).
4. `du` a dat 268M pentru `/var/log` și 6,8G pentru `/usr`, deci `/usr` e de aproximativ 26 de ori mai mare (6,8 · 1024 / 268 ≈ 26). `file` a arătat că `/usr/bin/ls` este o legătură spre `../lib/cargo/bin/coreutils/ls`, adică uutils (Rust), nu GNU coreutils.

## Partea 2 – Inode-uri și legături

| Fișier | Inode | Legături | Mărime |
|---|---|---|---|
| hard.txt | 6032018 | 1 (după rm notes.txt) | 6 octeți |
| soft.txt | 6032019 | 1 | 8 octeți |

1. În `stat`, inode-ul este câmpul `Inode` (6032018), iar `Links` arată câte nume are fișierul (inițial 1, iar după `ln` devine 2).
2. După `rm notes.txt`, `cat hard.txt` a afișat `hello`, pentru că ambele nume indicau același inode. `rm` șterge un nume din director și scade contorul de legături. Datele se eliberează abia când contorul ajunge la 0 și niciun proces nu mai ține fișierul deschis.
3. După `rm hard.txt`, `cat soft.txt` a dat „No such file or directory”. Legătura simbolică există, dar ținta ei nu mai există (legătură atârnată). Are 8 octeți, adică exact lungimea textului `hard.txt`, pe care îl păstrează ca țintă.
4. Directorul `d` are inițial 2 legături: numele `d` din directorul curent și intrarea `.` din interiorul lui. După `mkdir d/x d/y` are 4, pentru că fiecare subdirector are o intrare `..` care indică spre `d`.
5. `one.txt` are 2 octeți, dar ocupă 4,0K, pentru că spațiul se alocă în blocuri întregi de 4096 octeți. `big.img`, creat cu `truncate`, e un fișier sparse: are 1,0G ca dimensiune logică, dar nu are niciun bloc alocat, așa că `du` arată 0.

## Partea 3 – Permisiuni

| Octal | rwx | Utilizare |
|---|---|---|
| 600 | rw------- | fișiere private, de exemplu o cheie SSH privată |
| 644 | rw-r--r-- | fișiere lizibile de toți, de exemplu /etc/passwd |
| 755 | rwxr-xr-x | programe și directoare publice, de exemplu /usr/bin |
| 700 | rwx------ | directoare private, de exemplu ~/.ssh |

2. Cu `chmod 000`, `cat s.txt` a dat „Permission denied”. Nucleul verifică doar setul de biți care se aplică procesului (aici cel al proprietarului, `---`), iar faptul că deții fișierul nu dă singur dreptul de citire. Proprietarul poate totuși schimba permisiunile, așa că `chmod u+r` a readus accesul.
3. umask este 0002. 666 AND NOT 002 = 664 (`rw-rw-r--`) pentru `new.txt`, iar 777 AND NOT 002 = 775 (`rwxrwxr-x`) pentru `newdir`, la fel ca în `ls -ld`.
4. Cu `u=r`, `ls box` a afișat `f`, dar `cat box/f` a dat Permission denied. Cu `u=x`, `ls box` a eșuat, dar `cat box/f` a afișat `hi`. Deci pe un director `r` permite listarea numelor, iar `x` permite traversarea, adică accesul la un element al cărui nume îl știi.
5. `/usr/bin/passwd` are `rwsr-xr-x`. Bitul `s` (setuid) face ca programul să ruleze cu drepturile lui root, iar passwd are nevoie de el ca să scrie în `/etc/shadow` (`rw-r-----`, root). `/tmp` are `rwxrwxrwt`. Bitul `t` (sticky) înseamnă că oricine poate crea fișiere acolo, dar fiecare poate șterge doar fișierele proprii. `cat /etc/shadow` a dat Permission denied.

## Partea 4 – Discuri și nucleul ca fișiere

1. Desenul discului:

```
sda        223,6G  disk
├─sda1     218,9G  part  /          ext4
└─sda2       1G    part  /boot/efi
nvme0n1    476,9G  disk
├─nvme0n1p1  260M  (nemontat)
├─nvme0n1p2   16M  (nemontat)
├─nvme0n1p3 426,1G (nemontat)
├─nvme0n1p4  915M  (nemontat)
└─nvme0n1p5  48,8G (nemontat)
loop*      snap-uri, doar citire (RO = 1)
```

   `mount` arată: `/dev/sda1 on / type ext4 (rw,relatime)`.
2. Pe `/` sunt folosiți 38G din 215G (19%) și 166G sunt liberi. Diferența dintre 218,9G (lsblk) și 215G (df) vine din metadatele ext4.
3. `/proc/cpuinfo` arată 12 procesoare logice. `/proc/meminfo` dă MemTotal 15472984 kB, adică aproximativ 14,76 GiB.
4. `/proc/self/status` a arătat `Name: head`. `/proc/self` indică procesul care citește fișierul, adică programul `head`, nu shell-ul. La fel, în `ls -l /proc/self/fd`, descriptorii 0, 1 și 2 indică `/dev/pts/0`, iar 3 indică directorul `/proc` deschis de `ls`.

## Stretch

(a) `file notes.txt` a dat „gzip compressed data”, deși extensia e .txt. Primii octeți, `1f 8b`, sunt numărul magic gzip, iar `08` înseamnă metoda deflate.

(b) `find / -xdev -perm -4000 -type f` a găsit 16 programe setuid: passwd, chsh, chfn, gpasswd, su, sudo.ws, /usr/lib/cargo/bin/su, /usr/lib/cargo/bin/sudo, mount, umount, fusermount3, ntfs-3g, pkexec, pppd, ssh-keysign și dbus-daemon-launch-helper. Lista e mai lungă decât cea de la curs. Fiecare program setuid de tip root e o posibilă țintă, de exemplu pkexec cu CVE-2021-4034.
