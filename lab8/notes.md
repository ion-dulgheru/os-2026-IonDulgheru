# Lab 8 notes

## pingpong

1. Sunt necesare două pipe-uri, fiindcă un pipe este un singur flux de octeți, fără direcție. Cu un singur pipe, procesul care scrie și-ar putea citi propriul octet înapoi, înainte ca celălalt proces să-l preia.
2. Părintele așteaptă pentru că read() pe pipe-ul gol c2p blochează procesul până când copilul scrie răspunsul. wait(0) de la final așteaptă în plus terminarea copilului.
3. Copilul are PID-ul mai mare deoarece PID-urile sunt alocate crescător, iar copilul este creat după părinte. În rularea mea: copil 4, părinte 3.

## primes

1. La afișarea lui "prime 31" există 11 etape (câte una pentru fiecare prim de la 2 la 31) și generatorul (main), adică 12 procese ale programului. Dacă se numără și `sh` și `init`, sunt 14.
2. Cu limita 280 programul nu se termină complet. Ultimul prim afișat este 269 (a 57-a etapă, pid 62 în rularea cu getpid()), apoi nucleul oprește procesul următor: `usertrap(): unexpected scause 0xf pid=63`, `stval=0x2ff8` (store page fault). Nu a apărut niciun mesaj "pipe failed" sau "fork failed", deci pipe() și fork() nu au întors eroare. Nu am putut confirma niciuna dintre limitele NOFILE, NPROC sau NFILE: sunt cel mult 60 de procese (sub NPROC = 64), fiecare etapă ține cel mult 3 descriptori (sub NOFILE = 16), iar eroarea de la pipe() așteptată pentru NFILE nu a apărut. Ipoteză neconfirmată: depășirea stivei utilizatorului, deoarece fiecare copil continuă în apelul stage() al părintelui, cu o copie a stivei acestuia, iar stiva xv6 are o singură pagină.
3. Este un ciur deoarece fiecare etapă elimină multiplii primului ei număr. Rulează în paralel deoarece toate etapele există simultan și filtrează numerele pe măsură ce acestea ajung prin pipe, fără să aștepte ca etapa anterioară să se termine.
