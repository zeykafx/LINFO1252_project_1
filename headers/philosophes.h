#pragma once
#include <pthread.h>
#include <stdbool.h>


#define CYCLES_PHILOSOPHERS 10


typedef struct philosophers_args {
    int number_of_philosophers;
    int id;
    pthread_mutex_t *baguette;
    bool verbose;
} philosophers_args_t;


void philosophers(int n_philosophers, bool verbose);
