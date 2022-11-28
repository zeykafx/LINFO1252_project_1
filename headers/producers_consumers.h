#pragma once

#include <stdbool.h>

#define BUFFER_SIZE 8
#define CYCLES 8192
#define BUSY_WORK_CYCLES 10000


typedef struct prod_cons_args {
    bool verbose;
    int id;
    bool using_pthread_sync;
} prod_cons_args_t;

void producer_consumer(int n_prods, int n_cons, bool verbose, bool using_pthread_sync);

void *producer(void *args);

void *consumer(void *args);
