# Lab 17 - Threads Share Memory, and Race (Linux)

## Part 1 - Shared and racing
Capturi: `screenshots/1_fire.png` (3 rulări `./fire`).

1. Faptul că thread 2 afișează la început un `counter` diferit de 0 (ex. `4451`) dovedește că cele două fire folosesc **aceeași memorie**: variabila globală `counter` este una singură, iar firul 2 vede imediat modificările făcute de firul 1, care deja numărase. (În unele rulări thread 2 vede 0, pentru că pornește înainte ca firul 1 să apuce să incrementeze; depinde de planificare.) Două procese, în schimb, ar avea fiecare copia lui.
2. `counter++` nu este o singură operație, ci trei pași: (1) citește `counter` din memorie într-un registru, (2) adună 1 în registru, (3) scrie registrul înapoi în memorie. Dacă ambele fire citesc aceeași valoare (de ex. 100) înainte ca vreunul să scrie, ambele scriu 101 și o adunare se pierde. Fiecare suprapunere pierde o incrementare, iar suprapunerile depind de momentul în care sistemul întrerupe/planifică firele pe nuclee, deci rezultatul final este mai mic decât 2000000 și diferit la fiecare rulare (în rulările mele: 1006352, 1151315, 1183811).

## Part 2 - Make it worse, then think
Capturi: `screenshots/2_fire4.png` (`fire4.c`, 4 fire).

Cu 4 fire valoarea așteptată este 4000000, dar am obținut 1564998, 1027661, 1335267. Pierderea crește în valoare absolută (între 2,4 și 3 milioane de adunări pierdute, față de ~1 milion la 2 fire), pentru că mai multe fire se suprapun pe aceeași variabilă; rezultatul final rămâne în jur de 1-1,6 milioane, aproape cât la 2 fire, deoarece variabila e disputată între nuclee și cele mai multe incrementări se suprascriu.

Propunere pe hârtie: protejăm `counter++` cu un mutex (`pthread_mutex_lock` înainte, `unlock` după), ca să fie executat de un singur fir odată.

Observe: **O singură idee ar opri adunările pierdute: excluderea mutuală, adică transformarea lui `counter++` într-o secțiune critică (sau operație atomică) pe care un singur fir o poate executa la un moment dat; asta construim săptămâna viitoare.**

## Extra (verificarea ideii din Part 2)
Capturi: `screenshots/3_fire_mutex.png`, `screenshots/4_counter_asm.png`.

1. `fire_mutex.c` este `fire4.c` cu `counter++` între `pthread_mutex_lock` și `pthread_mutex_unlock`. În 3 rulări cu 4 fire rezultatul a fost de fiecare dată exact `4000000`: ideea (excludere mutuală) oprește adunările pierdute.
2. Cei trei pași ai lui `counter++` se văd în assembly (`gcc -S`, fără optimizare): `movq counter(%rip), %rax` (citește), `addq $1, %rax` (adună), `movq %rax, counter(%rip)` (scrie). Un fir poate fi întrerupt între oricare două dintre aceste instrucțiuni, iar celălalt fir citește atunci valoarea veche.
