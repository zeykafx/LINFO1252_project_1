#pragma once
#include <stdbool.h>

#define TEST_SET_THREADS_CYCLE 6400

void test_and_set_lock(bool verbose, int n_threads, int n_tatas_threads, bool is_simple_tas);

int test_and_set(int *lock);

void lock(int *lock);
void unlock(int *lock);

void lock_test_and_test_and_set(int *lock);

void *thread_func(void *args);

typedef struct test_and_set_lock_threads_args {
    int *lock;
    int n_threads;
    bool verbose;
    int id;
    int n_tatas_threads;
    bool is_simple_tas;
} test_and_set_lock_threads_args_t;
