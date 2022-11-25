#pragma once

#include <stdbool.h>

#define TEST_SET_THREADS_CYCLE 6400

void test_and_set_lock(bool verbose, int n_threads, int n_tatas_threads, bool is_simple_tas);

typedef struct mutex {
    volatile int *lock;
} mutex_t;

mutex_t *mutex_init(void);

void mutex_destroy(mutex_t *mutex);

int test_and_set(mutex_t *mutex, int value);

void lock(mutex_t *mutex);

void unlock(mutex_t *mutex);

void lock_test_and_test_and_set(mutex_t *mutex);

void *thread_func(void *args);

typedef struct test_and_set_lock_threads_args {
    mutex_t *mutex;
    int n_threads;
    bool verbose;
    int id;
    int n_tatas_threads;
    bool is_simple_tas;
} test_and_set_lock_threads_args_t;
