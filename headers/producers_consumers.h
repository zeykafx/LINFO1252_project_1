#pragma once
#include <stdbool.h>

#define BUFFER_SIZE 8
#define CYCLES 16


int *buffer;
int *consumed_buffer;


void producer_consumer(int n_prods, int n_cons, bool verbose);

void *producer(void *args);

void *consumer(void *args);


typedef struct prod_cons_args {
    bool verbose;
    int id;
}prod_cons_args_t;
