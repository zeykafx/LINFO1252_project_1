#pragma once
#include <pthread.h>
#include <stdbool.h>
#include "../headers/test_and_set_lock.h"


#define CYCLES_PHILOSOPHERS 100000


typedef struct philosophers_args {
    int number_of_philosophers;
    int id;
    mutex_t **baguette;
    pthread_mutex_t *pthread_baguette;
    bool verbose;
    bool using_pthread_sync;
} philosophers_args_t;


void philosophers(int n_philosophers, bool verbose, bool using_pthread_sync);

void *philosopher(void *arg);
