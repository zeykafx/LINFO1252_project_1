//
// Created by corentin on 11/14/22.
//
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "../headers/philosophes.h"

void mange(int id) {
    // printf("philosophers %d is eating\n", id);
}

typedef struct philosophers_args {
    int number_of_philosophers;
    int id;
    pthread_mutex_t *baguette;
} philosophers_args_t;

void *philosopher(void *arg) {
    philosophers_args_t *args = (philosophers_args_t *) arg;
    int left = args->id;
    int right = (left + 1) % args->number_of_philosophers;

    printf("left: %d, right: %d\n", left, right);
    for (int i = 0; i < CYCLES; ++i) {
        // thinking...
        if (left < right) {
            pthread_mutex_lock(&args->baguette[left]);
            pthread_mutex_lock(&args->baguette[right]);
        } else {
            pthread_mutex_lock(&args->baguette[right]);
            pthread_mutex_lock(&args->baguette[left]);
        }
        mange(args->id);
        pthread_mutex_unlock(&args->baguette[left]);
        pthread_mutex_unlock(&args->baguette[right]);
    }
    return (NULL);
}



int philosophers(int n_philosophers) {
    pthread_mutex_t baguette[n_philosophers];

    philosophers_args_t **args_buffer =  calloc(n_philosophers, n_philosophers * sizeof(philosophers_args_t *));
    if (args_buffer == NULL) {
        perror("Failed to init args buffer for philosophers");
        exit(EXIT_FAILURE);
    }

    pthread_t phil[n_philosophers];

    for (int i = 0; i < n_philosophers; ++i) {
        int err = pthread_mutex_init(&baguette[i], NULL);
        if(err != 0) {
            perror("Failed to init philosophers mutex");
        }

        args_buffer[i] = malloc(sizeof(philosophers_args_t));
        args_buffer[i]->number_of_philosophers = n_philosophers;
        args_buffer[i]->id = i;
        args_buffer[i]->baguette = baguette;

        err = pthread_create(&phil[i], NULL, philosopher, (void *) args_buffer[i]);
        if (err != 0) {
            perror("Failed to create philosophers thread");
        }
    }


    for (int i = 0; i < n_philosophers; i++) {
      int err = pthread_join(phil[i], NULL);
      if(err!=0) {
        perror("Failed to join philosopher thread");
      }
   }


    for (int i = 0; i < n_philosophers; ++i) {
        int err = pthread_mutex_destroy( &baguette[i]);
        if(err != 0) {
            perror("Failed to destroy philosophers mutex");
        }
        free(args_buffer[i]);
    }
    free(args_buffer);

    return 69;
}
