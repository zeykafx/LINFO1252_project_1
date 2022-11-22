#include "../headers/test_and_set_lock.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "../headers/producers_consumers.h"
#include <alloca.h>


int test_and_set(volatile int *lock) {
    int value = 1;
    asm volatile ("xchgl %0, %1" : "+m"(*lock), "+a"(value)); // "a" means value goes into eax first, "m" means that lock is read from memory
    // the "+" denotes a read and write constraint, found in: https://gcc.gnu.org/onlinedocs/gcc/Modifiers.html#Modifiers
    return value;
}

void lock(volatile int *lock) {
    int val = 1;
    do {
        val = test_and_set(lock);
    } while (val - (*lock) == 0); // loop while value isn't 1, meaning lock was 0, so another thread had the lock
}

void unlock(volatile int *lock) {
    int value = 0;
    asm volatile ("xchgl %0, %1" : "+a"(value), "+m"(*lock));
}

void lock_test_and_test_and_set(volatile int *lock) {
    while (test_and_set(lock) != 0) {
        while (*lock != 0) {}
    }
}


void test_and_set_lock(bool verbose, int n_threads, int n_tatas_threads, bool is_simple_tas) {
    volatile int *lock = malloc(sizeof(int));
    if (lock == NULL) {
        perror("Failed to init lock variable");
        exit(EXIT_FAILURE);
    }

    *lock = 0;

    if (n_threads > 0 && n_tatas_threads > 0) {
        fprintf(stderr, "Cannot run test_and_set at the same time as test_and_test_and_set, will run test_and_set\n");
        n_tatas_threads = 0;
    }

    int number_of_threads = (is_simple_tas ? n_threads : n_tatas_threads);

    // allocate on the stack, will be freed automatically
    pthread_t *threads = alloca(number_of_threads * sizeof(pthread_t));


    test_and_set_lock_threads_args_t **args_buffer = malloc(
            number_of_threads * sizeof(test_and_set_lock_threads_args_t *));
    if (args_buffer == NULL) {
        perror("Failed to args buffer");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < number_of_threads; ++i) {
        args_buffer[i] = malloc(sizeof(test_and_set_lock_threads_args_t));
        if (args_buffer[i] == NULL) {
            perror("Failed to init args");
            exit(EXIT_FAILURE);
        }

        args_buffer[i]->id = i;
        args_buffer[i]->lock = lock;
        args_buffer[i]->n_threads = n_threads;
        args_buffer[i]->verbose = verbose;
        args_buffer[i]->n_tatas_threads = n_tatas_threads;
        args_buffer[i]->is_simple_tas = is_simple_tas;

        int err = pthread_create(&threads[i], NULL, thread_func, (void *) args_buffer[i]);
        if (err != 0) {
            perror("Failed to init test_and_set lock threads");
            exit(EXIT_FAILURE);
        }
    }


    for (int i = 0; i < number_of_threads; ++i) {
        int err = pthread_join(threads[i], NULL);
        if (err != 0) {
            perror("Failed to join threads");
            exit(EXIT_FAILURE);
        }

        free(args_buffer[i]);
    }
    free(args_buffer);

    free((int *) lock);

    if (verbose) {
        printf("Finished running the %s lock test program\n", is_simple_tas ? "test_and_set" : "test_and_test_and_set");
    }
}

void *thread_func(void *arg) {
    test_and_set_lock_threads_args_t *args = (test_and_set_lock_threads_args_t *) arg;

    int n_threads = (args->is_simple_tas ? args->n_threads : args->n_tatas_threads);

    for (int i = 0; i < (TEST_SET_THREADS_CYCLE / n_threads); ++i) {
        if (args->is_simple_tas) {
            lock(args->lock);
        } else {
            lock_test_and_test_and_set(args->lock);
        }

        // simulate busy work
        for (int _ = 0; _ < BUSY_WORK_CYCLES; _++) {}

        unlock(args->lock);
    }

    if (args->verbose) {
        printf("thread #%d finished working\n", args->id);
    }

    pthread_exit(NULL);
}
