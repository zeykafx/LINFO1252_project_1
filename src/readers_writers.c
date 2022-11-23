#include "../headers/readers_writers.h"
#include "../headers/producers_consumers.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include "../headers/my_semaphore.h"

// pthread mutex and semaphore, used when the -o is passed as argument to the program
pthread_mutex_t pthread_mutex_readcount, pthread_mutex_writercount;
pthread_mutex_t pthread_readtry;
sem_t pthread_rsem, pthread_wsem;
sem_t pthread_db;
pthread_mutex_t pthread_mutex;

// our mutex and semaphore, used by default
mutex_t *mutex_readcount, *mutex_writercount;
mutex_t *readtry;
semaphore_t rsem, wsem;

semaphore_t db;
mutex_t *mutex;

volatile int readcount, writecount = 0; // nombre de readers et de writers actif

void reader_writer(int n_reader, int n_writer, bool verbose, bool using_pthread_sync) {
    if (using_pthread_sync && verbose) {
        printf("Running the readers writers problem using pthread sync\n");
    }

    pthread_t readers[n_reader], writers[n_writer];

    mutex = mutex_init();
    mutex_readcount = mutex_init();
    mutex_writercount = mutex_init();
    readtry = mutex_init();

    semaphore_init(&db, 1);
    semaphore_init(&rsem, 1);
    semaphore_init(&wsem, 1);


    int err = pthread_mutex_init(&pthread_mutex, NULL);
    if (err != 0) {
        perror("Failed to init reader writer mutex");
        exit(EXIT_FAILURE);
    }
    err = pthread_mutex_init(&pthread_readtry, NULL);
    if (err != 0) {
        perror("Failed to init reader writer mutex");
        exit(EXIT_FAILURE);
    }
    err = pthread_mutex_init(&pthread_mutex_readcount, NULL);
    if (err != 0) {
        perror("Failed to init reader writer mutex");
        exit(EXIT_FAILURE);
    }
    err = pthread_mutex_init(&pthread_mutex_writercount, NULL);
    if (err != 0) {
        perror("Failed to init reader writer mutex");
        exit(EXIT_FAILURE);
    }

    sem_init(&pthread_db, 0, 1);
    sem_init(&pthread_rsem, 0, 1);
    sem_init(&pthread_wsem, 0, 1);

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
        writer_args[i]->number_of_threads = n_writer;
        writer_args[i]->using_pthread_sync = using_pthread_sync;

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
        reader_args[i]->number_of_threads = n_reader;
        reader_args[i]->using_pthread_sync = using_pthread_sync;


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
    mutex_destroy(mutex);
    mutex_destroy(readtry);
    mutex_destroy(mutex_readcount);
    mutex_destroy(mutex_writercount);

    err = pthread_mutex_destroy(&pthread_mutex);
    if (err != 0) {
        perror("Failed to destroy reader writer mutex");
        exit(EXIT_FAILURE);
    }
    err = pthread_mutex_destroy(&pthread_readtry);
    if (err != 0) {
        perror("Failed to destroy reader writer mutex");
        exit(EXIT_FAILURE);
    }
    err = pthread_mutex_destroy(&pthread_mutex_readcount);
    if (err != 0) {
        perror("Failed to destroy reader writer mutex");
        exit(EXIT_FAILURE);
    }
    err = pthread_mutex_destroy(&pthread_mutex_writercount);
    if (err != 0) {
        perror("Failed to destroy reader writer mutex");
        exit(EXIT_FAILURE);
    }

    semaphore_destroy(&db);
    semaphore_destroy(&wsem);
    semaphore_destroy(&rsem);
    sem_destroy(&pthread_db);
    sem_destroy(&pthread_wsem);
    sem_destroy(&pthread_rsem);

    if (verbose) {
        printf("Finished running the reader writer problem\n");
    }
}


void *reader(void *args) {
    reader_writer_args_t *arguments = (reader_writer_args_t *) args;

    for (int i = 0; i < WRITER_CYCLES / arguments->number_of_threads; ++i) {
        if (arguments->using_pthread_sync) {
            pthread_mutex_lock(&pthread_readtry);
            sem_wait(&pthread_rsem);
            pthread_mutex_lock(&pthread_mutex_readcount);
        } else {
            lock_test_and_test_and_set(readtry);
            semaphore_wait(&rsem);
            lock_test_and_test_and_set(mutex_readcount);
        }

            // section critique
            readcount++;
            if (readcount == 1) {
                if (arguments->verbose) {
                    printf("Reader #%d is the first reader, waiting on wsem\n", arguments->id);
                }
                if (arguments->using_pthread_sync) {
                    sem_wait(&pthread_wsem);
                } else {
                    semaphore_wait(&wsem);

                }
            }

        if (arguments->using_pthread_sync) {
            pthread_mutex_unlock(&pthread_mutex_readcount);
            sem_post(&pthread_rsem);
            pthread_mutex_unlock(&pthread_readtry);

        } else {
            unlock(mutex_readcount);
            semaphore_post(&rsem);
            unlock(readtry);
        }


        if (arguments->verbose) {
            printf("Reader #%d is reading\n", arguments->id);
        }

        // simulate busy work - read database
        for (int _ = 0; _ < BUSY_WORK_CYCLES; _++) {}

        if (arguments->using_pthread_sync) {
            pthread_mutex_lock(&pthread_mutex_readcount);
        } else {
            lock_test_and_test_and_set(mutex_readcount);
        }
            // section critique
            readcount--;
            if (readcount == 0) {
                if (arguments->verbose) {
                    printf("Reader #%d was the last reader\n", arguments->id);
                }
                if (arguments->using_pthread_sync) {
                    sem_post(&pthread_wsem);
                } else {
                    semaphore_post(&wsem);
                }
            }
        if (arguments->using_pthread_sync) {
            pthread_mutex_unlock(&pthread_mutex_readcount);
        } else {
            unlock(mutex_readcount);
        }

        if (arguments->verbose) {
            printf("Reader #%d is processing data read\n", arguments->id);
        }

    }

    pthread_exit(NULL);
}


void *writer(void *args) {
    reader_writer_args_t *arguments = (reader_writer_args_t *) args;

    for (int i = 0; i < READER_CYCLES / arguments->number_of_threads; ++i) {

        if (arguments->verbose) {
            printf("Writer #%d is preparing data\n", arguments->id);
        }


        if (arguments->using_pthread_sync) {
            pthread_mutex_lock(&pthread_mutex_writercount);
        } else {
            lock_test_and_test_and_set(mutex_writercount);
        }
            writecount++;
            if (writecount == 1) {
                if (arguments->verbose) {
                    printf("Writer #%d is the first writer\n", arguments->id);
                }
                if (arguments->using_pthread_sync) {
                    sem_wait(&pthread_rsem);
                } else {
                    semaphore_wait(&rsem);
                }
            }
        if (arguments->using_pthread_sync) {
            pthread_mutex_unlock(&pthread_mutex_writercount);
        } else {
            unlock(mutex_writercount);
        }

        if (arguments->using_pthread_sync) {
            sem_wait(&pthread_wsem);
        } else {
            semaphore_wait(&wsem);
        }
            // section critique, un seul writer à la fois

            if (arguments->verbose) {
                printf("Writer #%d is writing data\n", arguments->id);
            }
            // simulate busy work - write data
            for (int _ = 0; _ < BUSY_WORK_CYCLES; _++) {}

        if (arguments->using_pthread_sync) {
            sem_post(&pthread_wsem);
        } else {
            semaphore_post(&wsem);
        }

        if (arguments->using_pthread_sync) {
            pthread_mutex_lock(&pthread_mutex_writercount);
        } else {
            lock_test_and_test_and_set(mutex_writercount);
        }
            // section critique
            writecount--;
            if (writecount == 0) {
                if (arguments->verbose) {
                    printf("Writer #%d was the last writer\n", arguments->id);
                }
                if (arguments->using_pthread_sync) {
                    sem_post(&pthread_rsem);
                } else {
                    semaphore_post(&rsem);
                }
            }
        if (arguments->using_pthread_sync) {
            pthread_mutex_unlock(&pthread_mutex_writercount);
        } else {
            unlock(mutex_writercount);
        }
    }

    pthread_exit(NULL);
}
