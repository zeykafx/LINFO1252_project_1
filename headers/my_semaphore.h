#pragma once

#include <pthread.h>
#include "../headers/test_and_set_lock.h"
#include <stdbool.h>

typedef struct semaphore {
    volatile int counter;
    mutex_t *mutex;
} semaphore_t;


int semaphore_init(semaphore_t *semaphore, int init_value);

int semaphore_destroy(semaphore_t *semaphore);

void semaphore_wait(semaphore_t *semaphore);

void semaphore_post(semaphore_t *semaphore);


// test program: -----
typedef struct test_semaphore_threads_arguments {
    int *counter;
    semaphore_t *semaphore;
    bool verbose;
    int id;
} test_semaphore_threads_arguments_t;

void test_semaphore(int n_threads, bool verbose);

void *test_semaphore_thread_func(void *arg);
