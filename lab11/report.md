# Lab 11 - The Race and the Semaphore (Linux)

## Part 1 - See the race

Capturi: `screenshots/1_race.png`, `screenshots/4_full_run_time_order.png`.
`./race` a dat 1239941, 1049786, 1095444, niciodată 2000000.

1. `counter++` nu este un singur pas. CPU face trei pași:
   1. **load**: citește `counter` din memorie într-un registru;
   2. **add**: adună 1 în registru;
   3. **store**: scrie registrul înapoi în `counter`.
2. Presupunem `counter = 5`. Firul A citește 5 și firul B citește tot 5, înainte ca A să apuce să scrie. A scrie 6, iar B scrie și el 6. Două adunări au avut loc, dar valoarea a crescut o singură dată: o adunare s-a pierdut. Se repetă de multe ori într-un milion de iterații, de aceea rezultatul e mult sub 2000000.
3. Ordinea în care planificatorul întrerupe și alternează cele două fire nu este deterministă. Depinde de momentul exact al întreruperilor, de încărcarea sistemului și de nucleul pe care rulează fiecare fir. Numărul de adunări pierdute diferă deci la fiecare rulare.

## Part 2 - Fix it: mutex, then semaphore

Capturi: `2_mutex.png`, `3_sem.png`, `4_full_run_time_order.png`, `5_sem_init2.png`.

1. Da. `mutex` și `sem`, rulate de câte 3 ori, au dat de fiecare dată `counter = 2000000`. Doar un fir poate fi în secțiunea critică (între lock/unlock, respectiv wait/post), deci nicio adunare nu se pierde.
2. Cu `sem_init(&s, 0, 2)` rezultatele sunt greșite: 1508636, 1496323, 1761999. Semaforul are 2 unități, deci ambele fire pot trece `sem_wait` simultan și intră amândouă în secțiunea critică. Nu mai este excludere mutuală și apare din nou race-ul din Part 1. Pentru un lock, semaforul trebuie să pornească de la 1.
3. Varianta cu lock e mai lentă (`time`: race 0.009 s, mutex 0.210 s, sem 0.241 s). Fiecare incrementare face lock și unlock, iar când un fir găsește lock-ul ocupat trebuie să aștepte și, uneori, să intre în nucleu (sys time 0.2 s) ca să fie blocat și trezit. Varianta fără protecție doar incrementează, dar este greșită. Semaforul e puțin mai lent decât mutex-ul, pentru că este un mecanism mai general.

## Part 3 - A semaphore as a signal

Capturi: `4_full_run_time_order.png` (start la 0), `6_order_init1.png` (start la 1).

1. Cu `sem_init(&ready, 0, 1)` ieșirea este:
   ```
   B: waiting for the message...
   B: I read ""
   A: message written
   ```
   B citește un mesaj gol. Semaforul pornește cu valoarea 1, deci `sem_wait` din B trece imediat, fără să aștepte, și B citește `message` înainte ca A (care doarme o secundă) să-l scrie.
2. Un semafor care pornește de la 0 este o ușă închisă: cine face `sem_wait` așteaptă până când altcineva face `sem_post`, adică până când evenimentul așteptat s-a întâmplat.

## Part 4 - A semaphore as a counter: the parking lot

Capturi: `7_parking_places3.png`, `8_parking_places1.png`.

1. Valoarea maximă a lui `inside` este 3. Linia `sem_wait(&places);` o garantează: semaforul pornește de la `PLACES` = 3 și scade la fiecare mașină care intră. A patra mașină găsește 0 și așteaptă până când una pleacă și face `sem_post(&places)`.
2. Cu `PLACES 1`, `inside` nu depășește 1: intră câte o mașină pe rând, iar cele 6 mașini se succed. Semaforul devine un lock (mutex) binar, ca în `sem.c` din Part 2.
