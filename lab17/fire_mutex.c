/* fire_mutex.c - the fix: the same 4 threads, counter++ inside a mutex. */
#include <stdio.h>
#include <pthread.h>

#define N 1000000
#define T 4
long counter = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void *work(void *arg) {
    long id = (long)arg;
    printf("thread %ld sees counter = %ld\n", id, counter);
    for (int i = 0; i < N; i++) {
        pthread_mutex_lock(&m);
        counter++;
        pthread_mutex_unlock(&m);
    }
    return NULL;
}

int main(void) {
    pthread_t t[T];
    for (long i = 0; i < T; i++)
        pthread_create(&t[i], NULL, work, (void *)(i + 1));
    for (int i = 0; i < T; i++)
        pthread_join(t[i], NULL);
    printf("expected: %d\n", T * N);
    printf("counter: %ld    (with the mutex: no lost updates)\n", counter);
    return 0;
}
