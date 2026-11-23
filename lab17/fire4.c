/* fire4.c - Part 2: four threads, same unprotected counter. */
#include <stdio.h>
#include <pthread.h>

#define N 1000000
#define T 4
long counter = 0;

void *work(void *arg) {
    long id = (long)arg;
    printf("thread %ld sees counter = %ld\n", id, counter);
    for (int i = 0; i < N; i++)
        counter++;
    return NULL;
}

int main(void) {
    pthread_t t[T];
    for (long i = 0; i < T; i++)
        pthread_create(&t[i], NULL, work, (void *)(i + 1));
    for (int i = 0; i < T; i++)
        pthread_join(t[i], NULL);
    printf("expected: %d\n", T * N);
    printf("counter: %ld    (lost updates: the race)\n", counter);
    return 0;
}
