#pragma once
#include <stdbool.h>

#define TEST_SET_THREADS_CYCLE 6400

void test_and_set_lock(bool verbose, int n_threads);

void lock(int *lock);
void unlock(int *lock);

void *thread_func(void *args);

typedef struct test_and_set_lock_threads_args {
    int *lock;
    int n_threads;
    bool verbose;
    int id;
} test_and_set_lock_threads_args_t;