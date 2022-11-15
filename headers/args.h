#pragma once
#include <stdbool.h>

typedef struct options {
    int number_philosophers;
    int number_producers;
    int number_consumers;
    int number_readers;
    int number_writers;

    bool verbose;
} options_t;

int options_parser(int argc, char *argv[], options_t *options);