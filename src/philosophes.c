#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "../headers/philosophes.h"

void philosophers(int n_philosophers, bool verbose) {
    if (n_philosophers < 2) {
        fprintf(stderr, "Cannot run the philosophers problem with only one philosopher (thread)!\n");
        exit(EXIT_FAILURE);
    }

    pthread_t phil[n_philosophers];
    pthread_mutex_t baguette[n_philosophers];

    philosophers_args_t **args_buffer = malloc(n_philosophers * sizeof(philosophers_args_t *));
    if (args_buffer == NULL) {
        perror("Failed to init args buffer for philosophers");
        exit(EXIT_FAILURE);
    }

    // for each philosopher, we create a mutex, a thread, and we start the thread
    for (int i = 0; i < n_philosophers; ++i) {
        int err = pthread_mutex_init(&baguette[i], NULL);
        if (err != 0) {
            perror("Failed to init philosophers mutex");
            exit(EXIT_FAILURE);
        }

        args_buffer[i] = malloc(sizeof(philosophers_args_t));
        args_buffer[i]->number_of_philosophers = n_philosophers;
        args_buffer[i]->id = i;
        args_buffer[i]->baguette = baguette;
        args_buffer[i]->verbose = verbose;

        err = pthread_create(&phil[i], NULL, philosopher, (void *) args_buffer[i]);
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
        int err = pthread_mutex_destroy(&baguette[i]);
        if (err != 0) {
            perror("Failed to destroy philosophers mutex");
            exit(EXIT_FAILURE);
        }
        free(args_buffer[i]);
    }
    free(args_buffer);

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
            pthread_mutex_lock(&args->baguette[left]);
            pthread_mutex_lock(&args->baguette[right]);
        } else {
            pthread_mutex_lock(&args->baguette[right]);
            pthread_mutex_lock(&args->baguette[left]);
        }

        // eating ...
        if (args->verbose) {
            printf("philosopher %d is eating\n", args->id);
        }

        pthread_mutex_unlock(&args->baguette[left]);
        pthread_mutex_unlock(&args->baguette[right]);
    }

    pthread_exit(NULL);
}
