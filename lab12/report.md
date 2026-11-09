# Lab 12 - Producer, Consumer and the Other Tools (Linux)

## Part 1 - Producer and consumer
Capturi: `screenshots/1_prodcons.png`, `screenshots/2_prodcons_swap.png`.

1. Producătorul este oprit de `sem_wait(&empty)`: semaforul pornește de la 4 și scade la fiecare `put`; la 0 producătorul așteaptă până când consumatorul face `sem_post(&empty)`.
2. La început așteaptă consumatorul, la `sem_wait(&full)`, pentru că `full` pornește de la 0 și nu e nimic de luat.
3. Cu `lock` înainte de `sem_wait(&empty)`, când buffer-ul e plin producătorul așteaptă un loc liber ținând mutex-ul. Consumatorul are nevoie de același mutex ca să ia un element, deci așteaptă producătorul, iar producătorul așteaptă consumatorul: deadlock. Programul se oprește după `put 4` (`timeout` îl oprește, exit code 124).

## Part 2 - The condition variable
Captură: `screenshots/3_condvar_while_if.png` (primele 3 linii `condvar`, ultimele 3 `condvar_if`).

1. Variabila întreagă `ready` ține minte că mesajul e gata; variabila de condiție doar trezește firul care doarme.
2. Da, cu `if` merge și aici, pentru că e un singur writer, un singur reader și un singur semnal.
3. Se scrie `while` pentru că un fir se poate trezi și găsi totuși `ready == 0`: (a) trezire falsă (spurious wakeup), permisă de sistem fără niciun semnal; (b) cu mai multe fire, altul se poate trezi primul, ia datele și pune condiția la loc pe fals înainte ca acest fir să ia mutex-ul.

## Part 3 - Readers and writers
Captură: `screenshots/4_readers.png`.

1. Liniile `if (++readers == 1) sem_wait(&room);` și `if (--readers == 0) sem_post(&room);`: primul reader blochează camera pentru writeri, iar ultimul o eliberează.
2. `readers` este o variabilă comună modificată de mai multe fire; fără `rlock`, `++readers`/`--readers` ar avea același race ca `counter++` din Lab 11 (adunări pierdute, contor greșit, cameră blocată pentru totdeauna sau deschisă cu un reader înăuntru).
3. Dacă readerii vin fără pauză, `readers` nu ajunge niciodată la 0, `room` nu se eliberează și writer 2 așteaptă la nesfârșit: starvation.

## Part 4 - Deadlock
Capturi: `5_deadlock_orig.png`, `6_deadlock_fixed.png`, `7_deadlock_nosleep.png`.

1. `one` ține A și așteaptă B; `two` ține B și așteaptă A. Cerc: one → B → two → A → one.
2. Nu se blochează mereu: din 10 rulări, 9 au dat exit code 124 și una s-a terminat normal (exit code 0), pentru că uneori primul fir ia ambele mutex-uri înainte să înceapă al doilea. E mai rău: un program care se blochează mereu e găsit și reparat imediat, unul care se blochează doar uneori trece testele și cedează mai târziu, iar eroarea e greu de reprodus.
3. Dacă toate firele iau lock-urile în aceeași ordine, nu se poate forma un cerc de așteptare: un fir care așteaptă un lock e așteptat doar de fire aflate mai târziu în ordine, deci cineva poate mereu continua.
