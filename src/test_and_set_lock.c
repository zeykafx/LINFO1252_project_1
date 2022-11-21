#include "../headers/test_and_set_lock.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "../headers/producers_consumers.h"


void lock(int *lock) {
    int value = 1;
    do {
        asm volatile ("xchgl %0, %1" : : "a"(value), "m"(*lock)); // a means value goes into eax first, m means that lock is read from memory
    } while (value - (*lock) == 0); // loop while value isn't 1, meaning lock was 0, so another thread had the lock

}

void unlock(int *lock) {
    int value = 0;
    asm("xchgl %0, %1" : : "a"(value), "m"(*lock));
}


void test_and_set_lock(bool verbose, int n_threads) {
    int *lock = malloc(sizeof(int));
    if (lock == NULL) {
        perror("Failed to init lock variable");
        exit(EXIT_FAILURE);
    }


    pthread_t threads[n_threads];

    test_and_set_lock_threads_args_t **args_buffer = malloc(n_threads * sizeof(test_and_set_lock_threads_args_t*));
        if (args_buffer == NULL) {
        perror("Failed to args buffer");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n_threads;++i) {
        args_buffer[i] = malloc(sizeof(test_and_set_lock_threads_args_t));
        if (args_buffer[i] == NULL) {
            perror("Failed to init args");
            exit(EXIT_FAILURE);
        }

        args_buffer[i]->id = i;
        args_buffer[i]->lock = lock;
        args_buffer[i]->n_threads = n_threads;
        args_buffer[i]->verbose = verbose;

        int err = pthread_create(&threads[i], NULL, thread_func, (void *) args_buffer[i]);
        if (err != 0) {
            perror("Failed to init test_and_set lock threads");
            exit(EXIT_FAILURE);
        }
    }

    
    for (int i = 0; i < n_threads;++i) {
        int err = pthread_join(threads[i], NULL);
        if (err != 0) {
            perror("Failed to join threads");
            exit(EXIT_FAILURE);
        }

        free(args_buffer[i]);
    }
    free(args_buffer);

    free(lock);

    if (verbose) {
        printf("Finished running the test_and_set lock test program\n");
    }
}

void *thread_func(void *arg) {
    test_and_set_lock_threads_args_t *args = (test_and_set_lock_threads_args_t *) arg;
    
    for (int i = 0; i < TEST_SET_THREADS_CYCLE/args->n_threads; ++i) {
        lock(args->lock);

            // simulate busy work
            for (int _ = 0; _ < BUSY_WORK_CYCLES; _++) {}

        unlock(args->lock);
    }

    if (args->verbose) {
        printf("thread #%d finished working\n", args->id);
    }

    pthread_exit(NULL);
}