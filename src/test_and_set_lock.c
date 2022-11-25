#include "../headers/test_and_set_lock.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "../headers/producers_consumers.h"
#include <alloca.h>


// test-and-set & test-and-test-and set lock implementation: ----------------------

mutex_t *mutex_init(void) {
    mutex_t *mutex = malloc(sizeof(mutex_t));
    if (mutex == NULL) {
        perror("Failed to init mutex struct");
        exit(EXIT_FAILURE);
    }
    mutex->lock = malloc(sizeof(int));
    if (mutex->lock == NULL) {
        perror("Failed to init lock variable");
        exit(EXIT_FAILURE);
    }
    *mutex->lock = 0;
    return mutex;
}

void mutex_destroy(mutex_t *mutex) {
    free((int *) mutex->lock);
    free(mutex);
}

int test_and_set(mutex_t *mutex) {
    int value = 1;
    // the "lock" is just there to make sure that it will indeed be an atomic operation
    asm volatile (
            "lock; xchgl %0, %1"
            : "+m"(*mutex->lock), "+a"(value)
            :
            : "memory"
            );
    // "a" means value goes into eax first, "m" means that lock is read from memory
    // the "+" denotes a read and write constraint, found in: https://gcc.gnu.org/onlinedocs/gcc/Modifiers.html#Modifiers
    return value;
}

void lock(mutex_t *mutex) {
//    int val = 1;
//    do {
//        val = test_and_set(mutex);
//    } while (val - (*mutex->lock) == 0); // loop while value isn't 1, meaning lock was 0, so another thread had the lock

    // spin until test_and_set returns 1, meaning we got the lock
    while (test_and_set(mutex) != 0) {
//        asm volatile ("nop");
    }

}

void unlock(mutex_t *mutex) {
    int value = 0;
    asm volatile ("lock; xchgl %0, %1" : "+a"(value), "+m"(*mutex->lock): : "memory"); // sets the lock variable back to 0
}

void lock_test_and_test_and_set(mutex_t *mutex) {
    while (test_and_set(mutex) != 0) {
        while (*mutex->lock != 0) {
            asm volatile ("nop");
        }
    }
}


// test program: ----------------------

volatile int lock_dump = 0;

void test_and_set_lock(bool verbose, int n_threads, int n_tatas_threads, bool is_simple_tas) {
    mutex_t *mutex = mutex_init();

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
        perror("Failed to malloc args buffer");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < number_of_threads; ++i) {
        args_buffer[i] = malloc(sizeof(test_and_set_lock_threads_args_t));
        if (args_buffer[i] == NULL) {
            perror("Failed to init args");
            exit(EXIT_FAILURE);
        }

        args_buffer[i]->id = i;
        args_buffer[i]->mutex = mutex;
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

    mutex_destroy(mutex);

    if (verbose) {
        printf("Finished running the %s lock test program\n", is_simple_tas ? "test_and_set" : "test_and_test_and_set");
    }
}

void *thread_func(void *arg) {
    test_and_set_lock_threads_args_t *args = (test_and_set_lock_threads_args_t *) arg;

    int n_threads = (args->is_simple_tas ? args->n_threads : args->n_tatas_threads);

    for (int i = 0; i < (TEST_SET_THREADS_CYCLE / n_threads); ++i) {
        if (args->is_simple_tas) {
            lock(args->mutex);
        } else {
            lock_test_and_test_and_set(args->mutex);
        }

        // simulate busy work
        for (int _ = 0; _ < BUSY_WORK_CYCLES; _++) {
            lock_dump++;
        }

        unlock(args->mutex);
    }

    if (args->verbose) {
        printf("thread #%d finished working\n", args->id);
    }

    pthread_exit(NULL);
}
