#pragma once
#include <stdbool.h>

#define BUFFER_SIZE 8
#define CYCLES 8192
//#define CYCLES 16
#define BUSY_WORK_CYCLES 10000

int *buffer;
int *consumed_buffer;

typedef struct prod_cons_args {
    bool verbose;
    int id;
}prod_cons_args_t;

void producer_consumer(int n_prods, int n_cons, bool verbose);

void *producer(void *args);

void *consumer(void *args);
