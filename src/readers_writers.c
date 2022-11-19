#include "../headers/readers_writers.h"
#include "producers_consumers.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t mutex_readcount, mutex_writercount;
pthread_mutex_t readtry;
sem_t rsem, wsem;

sem_t db;
pthread_mutex_t mutex;

volatile int readcount, writecount = 0; // nombre de readers et de writers actif

void reader_writer(int n_reader, int n_writer, bool verbose) {
    pthread_t readers[n_reader], writers[n_writer];

    int err = pthread_mutex_init(&mutex, NULL);
    if (err != 0) {
        perror("Failed to init reader writer mutex");
        exit(EXIT_FAILURE);
    }
    err = pthread_mutex_init(&mutex_readcount, NULL);
    if (err != 0) {
        perror("Failed to init reader writer mutex");
        exit(EXIT_FAILURE);
    }
    err = pthread_mutex_init(&mutex_writercount, NULL);
    if (err != 0) {
        perror("Failed to init reader writer mutex");
        exit(EXIT_FAILURE);
    }

    sem_init(&db, 0, 1);
    sem_init(&rsem, 0, 1);
    sem_init(&wsem, 0, 1);

    // Writers: init and start -------
    reader_writer_args_t **writer_args = malloc(n_writer * sizeof(reader_writer_args_t *));
    if (writer_args == NULL) {
        perror("Failed to init writer args buffer");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n_writer; ++i) {
        writer_args[i] = malloc(sizeof(reader_writer_args_t));
        if (writer_args[i] == NULL) {
            perror("Failed to init writer args");
            exit(EXIT_FAILURE);
        }

        writer_args[i]->verbose = verbose;
        writer_args[i]->id = i;

        pthread_create(&writers[i], NULL, writer, (void *) writer_args[i]);
    }

    // Readers: init and start -------
    reader_writer_args_t **reader_args = malloc(n_reader * sizeof(reader_writer_args_t *));
    if (reader_args == NULL) {
        perror("Failed to init reader args buffer");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n_reader; ++i) {
        reader_args[i] = malloc(sizeof(reader_writer_args_t));
        if (reader_args[i] == NULL) {
            perror("Failed to init reader args");
            exit(EXIT_FAILURE);
        }

        reader_args[i]->verbose = verbose;
        reader_args[i]->id = i;


        pthread_create(&readers[i], NULL, reader, (void *) reader_args[i]);
    }

    // writer: join and clean up
    for (int i = 0; i < n_writer; ++i) {
        int error = pthread_join(writers[i], NULL);
        if (error != 0) {
            perror("Failed to join writer thread");
            exit(EXIT_FAILURE);
        }
        free(writer_args[i]);
    }
    free(writer_args);

    if (verbose) {
        printf("Joined all writers\n");
    }

    // reader: join and clean up
    for (int i = 0; i < n_reader; ++i) {
        int error = pthread_join(readers[i], NULL);
        if (error != 0) {
            perror("Failed to join reader thread");
            exit(EXIT_FAILURE);
        }
        free(reader_args[i]);
    }
    free(reader_args);

    if (verbose) {
        printf("Joined all readers\n");
    }

    // Clean up
    err = pthread_mutex_destroy(&mutex);
    if (err != 0) {
        perror("Failed to destroy reader writer mutex");
        exit(EXIT_FAILURE);
    }
    err = pthread_mutex_destroy(&mutex_readcount);
    if (err != 0) {
        perror("Failed to destroy reader writer mutex");
        exit(EXIT_FAILURE);
    }
    err = pthread_mutex_destroy(&mutex_writercount);
    if (err != 0) {
        perror("Failed to destroy reader writer mutex");
        exit(EXIT_FAILURE);
    }

    sem_destroy(&db);
    sem_destroy(&wsem);
    sem_destroy(&rsem);

    printf("Finished running the reader writer problem\n");
}


void *reader(void *args) {
    reader_writer_args_t *arguments = (reader_writer_args_t *)args;

    for (int i = 0; i < WRITER_CYCLES; ++i) {
        pthread_mutex_lock(&readtry);

        sem_wait(&rsem);

        pthread_mutex_lock(&mutex_readcount);
            // section critique
            readcount++;
            if (readcount == 1) {
                if (arguments->verbose) {
                    printf("Reader #%d is the first reader, waiting on wsem\n", arguments->id);
                }
                sem_wait(&wsem);
            }
        pthread_mutex_unlock(&mutex_readcount);

        sem_post(&rsem);

        pthread_mutex_unlock(&readtry);

        if (arguments->verbose) {
            printf("Reader #%d is reading\n", arguments->id);
        }
        // simulate busy work - read database
        for (int _ = 0; _ < BUSY_WORK_CYCLES; _++) {}


        pthread_mutex_lock(&mutex_readcount);
            // section critique
            readcount--;
            if (readcount == 0) {
                if (arguments->verbose) {
                    printf("Reader #%d was the last reader\n", arguments->id);
                }
                sem_post(&wsem);
            }
        pthread_mutex_unlock(&mutex_readcount);

        if (arguments->verbose) {
            printf("Reader #%d is processing data read\n", arguments->id);
        }
        // simulate busy work - process data
        for (int _ = 0; _ < BUSY_WORK_CYCLES; _++) {}
    }

    pthread_exit(NULL);
}


void *writer(void *args) {
    reader_writer_args_t *arguments = (reader_writer_args_t *)args;

    for (int i = 0; i < READER_CYCLES; ++i) {

        if (arguments->verbose) {
            printf("Writer #%d is preparing data\n", arguments->id);
        }
        // simulate busy work - prepare data
        for (int _ = 0; _ < BUSY_WORK_CYCLES; _++) {}

        pthread_mutex_lock(&mutex_writercount);
            writecount++;
            if (writecount == 1) {
                if (arguments->verbose) {
                    printf("Writer #%d is the first writer\n", arguments->id);
                }
                sem_wait(&rsem);
            }
        pthread_mutex_unlock(&mutex_writercount);

        sem_wait(&wsem);
        // section critique, un seul writer à la fois

        if (arguments->verbose) {
            printf("Writer #%d is writing data\n", arguments->id);
        }
        // simulate busy work - write data
        for (int _ = 0; _ < BUSY_WORK_CYCLES; _++) {}

        sem_post(&wsem);

        pthread_mutex_lock(&mutex_writercount);
            writecount--;
            if (writecount == 0) {
                if (arguments->verbose) {
                    printf("Writer #%d was the last writer\n", arguments->id);
                }
                sem_post(&rsem);
            }
        pthread_mutex_unlock(&mutex_writercount);
    }

    pthread_exit(NULL);
}
