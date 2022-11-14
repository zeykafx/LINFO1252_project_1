//
// Created by corentin on 11/14/22.
//
#include <stdio.h>
#include <pthread.h>
#include "philosophes.h"


void mange(int id) {
    printf("philosophers %d is eating\n", id);
}

typedef struct philosophers_args {
    int number_of_philosophers;
    int id;
    pthread_mutex_t baguette[];
} philosophers_args_t;

void *philosopher(void *arg) {
    philosophers_args_t *args = (philosophers_args_t *) arg;
    int left = args->id;
    int right = (left + 1) % args->number_of_philosophers;
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
    return 69;
}
