#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/philosophes.h"

void philosophers(int n_philosophers, bool verbose, bool using_pthread_sync) {
    if (n_philosophers < 2) {
        fprintf(stderr, "Cannot run the philosophers problem with only one philosopher (thread)!\n");
        exit(EXIT_FAILURE);
    }

    if (using_pthread_sync) {
        printf("Running the philosophers problem using pthread sync\n");
    }

    pthread_t phil[n_philosophers];

        // array of pthread mutexes
        pthread_mutex_t *pthread_baguette = malloc(sizeof(pthread_mutex_t) * n_philosophers);
        if (pthread_baguette == NULL) {
            perror("Failed to mutex buffer for philosophers");
            exit(EXIT_FAILURE);
        }

        // own mutex array
        mutex_t *baguette[n_philosophers];


    // for each philosopher, we create a mutex, a thread, and we start the thread
    for (int i = 0; i < n_philosophers; ++i) {
        int err = pthread_mutex_init(&pthread_baguette[i], NULL);
        if (err != 0) {
            perror("Failed to init philosophers mutex");
            exit(EXIT_FAILURE);
        }
        baguette[i] = mutex_init();
    }
    
    philosophers_args_t **args_buffer = malloc(n_philosophers * sizeof(philosophers_args_t *));
    if (args_buffer == NULL) {
        perror("Failed to init args buffer for philosophers");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n_philosophers; ++i) {
        args_buffer[i] = malloc(sizeof(philosophers_args_t));
        args_buffer[i]->number_of_philosophers = n_philosophers;
        args_buffer[i]->id = i;
        args_buffer[i]->baguette = baguette;
        args_buffer[i]->pthread_baguette = pthread_baguette;
        args_buffer[i]->verbose = verbose;
        args_buffer[i]->using_pthread_sync = using_pthread_sync;

        int err = pthread_create(&phil[i], NULL, philosopher, (void *) args_buffer[i]);
        if (err != 0) {
            perror("Failed to create philosophers thread");
            exit(EXIT_FAILURE);
        }
    }

    // joining all the philosophers, this will block and wait for them to finish
    for (int i = 0; i < n_philosophers; i++) {
        int err = pthread_join(phil[i], NULL);
        if (err != 0) {
            perror("Failed to join philosopher thread");
            exit(EXIT_FAILURE);
        }
    }

    // clean up
    for (int i = 0; i < n_philosophers; ++i) {
        int err = pthread_mutex_destroy(&pthread_baguette[i]);
        if (err != 0) {
            perror("Failed to destroy philosophers mutex");
            exit(EXIT_FAILURE);
        }
        mutex_destroy(baguette[i]);
        free(args_buffer[i]);
    }
    free(args_buffer);
    free(pthread_baguette);

    if (verbose) {
        printf("Finished running the Philosophers problem\n");
    }
}

void *philosopher(void *arg) {
    philosophers_args_t *args = (philosophers_args_t *) arg;

    int left = args->id;
    int right = (left + 1) % args->number_of_philosophers;

    if (args->verbose) {
        printf("left: %d, right: %d\n", left, right);
    }

    for (int i = 0; i < CYCLES_PHILOSOPHERS; ++i) {

        // thinking...
        if (args->verbose) {
            printf("philosopher %d is thinking\n", args->id);
        }

        if (left < right) {
            if (args->using_pthread_sync) {
                pthread_mutex_lock(&args->pthread_baguette[left]);
                pthread_mutex_lock(&args->pthread_baguette[right]);
            } else {
                lock_test_and_test_and_set(args->baguette[left]);
                lock_test_and_test_and_set(args->baguette[right]);
            }
        } else {
            if (args->using_pthread_sync) {
                pthread_mutex_lock(&args->pthread_baguette[right]);
                pthread_mutex_lock(&args->pthread_baguette[left]);
            } else {
                lock_test_and_test_and_set(args->baguette[right]);
                lock_test_and_test_and_set(args->baguette[left]);
            }
        }

        // eating ...
        if (args->verbose) {
            printf("philosopher %d is eating\n", args->id);
        }

        if (args->using_pthread_sync) {
            pthread_mutex_unlock(&args->pthread_baguette[left]);
            pthread_mutex_unlock(&args->pthread_baguette[right]);
        } else {
            unlock(args->baguette[left]);
            unlock(args->baguette[right]);
        }

    }

    pthread_exit(NULL);
}
