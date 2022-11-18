#pragma once
#include <pthread.c>
#include <stdio.h>
#include <stdlib.h>

#define N 10
#define BUFFER_SIZE 8

int *buffer;

void producer(void);

void consumer(void);