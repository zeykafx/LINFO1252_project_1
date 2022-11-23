#include <stdio.h>
#include <stdlib.h>
#include "../headers/my_semaphore.h"

// semaphore implementation: ----------------------------

// initializes the semaphore struct (with the mutex and the value)
int semaphore_init(semaphore_t *semaphore, int init_value) {
    semaphore->mutex = mutex_init();
    semaphore->counter = init_value;

    return 0;
}

// cleans up the semaphore struct (only the mutex needs to be cleaned up)
int semaphore_destroy(semaphore_t *semaphore) {
    mutex_destroy(semaphore->mutex);
    return 0;
}

// decrements the semaphore value, and block the calling thread until the semaphore value is greater than 0
void semaphore_wait(semaphore_t *semaphore) {

    // only one thread at a time is going to be let through the mutex, and if a thread locked the mutex but the counter is 0
    // it will spin until the counter is incremented, any thread that call semaphore_wait during this will be waiting on
    // the lock instead.

    lock_test_and_test_and_set(semaphore->mutex);

    if (semaphore->counter > 0) {
        (semaphore->counter)--;
    } else {
        // the calling thread is blocked while counter is 0
        while (semaphore->counter == 0) {}

        (semaphore->counter)--;
    }


    unlock(semaphore->mutex);
}

// increments the semaphore value, unblocks any thread waiting for the counter to go up.
void semaphore_post(semaphore_t *semaphore) {
    (semaphore->counter)++;

    // we can also increment counter using xadd which is the atomic addition
//    uint res;
//    asm volatile ("lock; xadd %0, %1"
//            : "=r"(res), "+m"(semaphore->counter)
//            : "0"(1)
//            : "memory"
//            );
//    return res;
}


// test program: ----------------------------
// small program used to test if the semaphores above work correctly, they are used to guarantee mutual exclusion,
// sort of like a binary semaphore

void test_semaphore(int n_threads, bool verbose) {
    pthread_t threads[n_threads];

    test_semaphore_threads_arguments_t **threads_args = malloc(n_threads * sizeof(test_semaphore_threads_arguments_t *));
    if (threads_args == NULL) {
        perror("Failed to malloc args buffer");
        exit(EXIT_FAILURE);
    }
    volatile int *counter = malloc(sizeof(int));

    *counter = 0;

    semaphore_t semaphore;

    semaphore_init(&semaphore, 1);

    for (int i = 0; i < n_threads; ++i) {

        threads_args[i] = malloc(sizeof(test_semaphore_threads_arguments_t));

        threads_args[i]->counter = counter;
        threads_args[i]->semaphore = &semaphore;
        threads_args[i]->verbose = verbose;
        threads_args[i]->id = i;

        int err = pthread_create(&threads[i], NULL, test_semaphore_thread_func, (void *) threads_args[i]);
        if (err != 0) {
            perror("Failed to init semaphore test threads");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < n_threads; ++i) {
        int err = pthread_join(threads[i], NULL);
        if (err != 0) {
            perror("Failed to join semaphore test threads");
            exit(EXIT_FAILURE);
        }
        free(threads_args[i]);
    }
    free(threads_args);

    if (verbose) {
        printf("Counter : %d, expected: %d\n", *counter, (10000 * n_threads));
        printf("Finished running the semaphore test program\n");
    }

    free((int *) counter);

    semaphore_destroy(&semaphore);
}

void *test_semaphore_thread_func(void *arg) {
    test_semaphore_threads_arguments_t *args = (test_semaphore_threads_arguments_t *) arg;

    for (int i = 0; i < 10000; ++i) {
        semaphore_wait(args->semaphore);
        if (args->verbose) {
            printf("Thread #%d got past the sem and is incrementing counter\n", args->id);
        }
        (*args->counter)++;
//        *args->counter = *args->counter + 1;
        semaphore_post(args->semaphore);
    }
    pthread_exit(NULL);
}
