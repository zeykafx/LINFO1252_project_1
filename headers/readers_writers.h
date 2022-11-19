#pragma once

#include <stdbool.h>

#define WRITER_CYCLES 640
#define READER_CYCLES 2560

void reader_writer(int n_reader, int n_writer, bool verbose);

void *writer(void *args);

void *reader(void *args);

typedef struct reader_writer_args {
    bool verbose;
    int id;
} reader_writer_args_t;
