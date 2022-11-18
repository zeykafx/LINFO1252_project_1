#include "../headers/producers_consumers.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
pthread_mutex_t mutex;
sem_t empty;
sem_t full;
int produced_elements = 0;
int consumed_elements = 0;


void producer_consumer(int n_prods, int n_cons, bool verbose) {

    pthread_t prods[n_prods], cons[n_cons];


    int error = pthread_mutex_init(&mutex, NULL);
    if (error != 0) {
        perror("Failed to init prod cons mutex");
        exit(EXIT_FAILURE);
    }

    sem_init(&empty, 0, BUFFER_SIZE);  // buffer vide
    sem_init(&full, 0, 0);             // buffer vide

    buffer = malloc(BUFFER_SIZE * sizeof(int));
    if (buffer == NULL) {
        perror("Failed to malloc shared buffer");
        exit(EXIT_FAILURE);
    }

    consumed_buffer = calloc(BUFFER_SIZE, BUFFER_SIZE * sizeof(short int));
    if (consumed_buffer == NULL) {
        perror("Failed to malloc consumed buffer");
        exit(EXIT_FAILURE);
    }

    // PRODS ------
    prod_cons_args_t **prod_args = malloc(n_prods * sizeof(prod_cons_args_t *));
    if (prod_args == NULL) {
        perror("Failed to malloc prod args array");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n_prods; ++i) {
        prod_args[i] = malloc(sizeof(prod_cons_args_t));
        if (prod_args[i] == NULL) {
            perror("Failed to malloc prod args");
            exit(EXIT_FAILURE);
        }

        prod_args[i]->id = i;
        prod_args[i]->verbose = verbose;

        pthread_create(&prods[i], NULL, producer, (void *) prod_args[i]);
    }

    // CONS ---
    prod_cons_args_t **cons_args = malloc(n_cons * sizeof(prod_cons_args_t *));
    if (cons_args == NULL) {
        perror("Failed to malloc cons args array");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n_cons; ++i) {
        cons_args[i] = malloc(sizeof(prod_cons_args_t));
        if (cons_args[i] == NULL) {
            perror("Failed to malloc cons args");
            exit(EXIT_FAILURE);
        }

        cons_args[i]->id = n_prods + i;
        cons_args[i]->verbose = verbose;

        pthread_create(&cons[i], NULL, consumer, (void *) prod_args[i]);
    }


    // producer: join and clean up
    for (int i = 0; i < n_prods; ++i) {
        int err = pthread_join(prods[i], NULL);
        if (err != 0) {
            perror("Failed to join producer thread");
            exit(EXIT_FAILURE);
        }
        free(prod_args[i]);
    }
    free(prod_args);

    // consumer: join and clean up
    for (int i = 0; i < n_cons; ++i) {
        int err = pthread_join(cons[i], NULL);
        if (err != 0) {
            perror("Failed to join consumer thread");
            exit(EXIT_FAILURE);
        }
        free(cons_args[i]);
    }
    free(cons_args);

    for (int i = 0; i < n_prods; ++i) {
        int err = pthread_mutex_destroy(&mutex);
        if (err != 0) {
            perror("Failed to destroy prod cons mutex");
            exit(EXIT_FAILURE);
        }
    }
}

// Producteur
void *producer(void *args) {
    prod_cons_args_t *arguments = (prod_cons_args_t *) args;

    while (true) {
        sem_wait(&empty); // attente d'une place libre
        pthread_mutex_lock(&mutex);
        if (produced_elements < CYCLES) {
            // section critique
            for (int j = 0; j < BUFFER_SIZE; j++) {
                if (consumed_buffer[j] == 0) {
                    buffer[j] = arguments->id;
                    produced_elements++;
                    consumed_buffer[j] = 1;

                    if (arguments->verbose) {
                        printf("Prod %d produced element index %d: %d\n", arguments->id, j, buffer[j]);
                        printf("Produced elements: %d\n", produced_elements);
                    }
                }
            }
        } else {
            break;
        }
        pthread_mutex_unlock(&mutex);
        sem_post(&full); // il y a une place remplie en plus

        // simulate busy work
        for (int i = 0; i < 10000; i++) {
            usleep(5);
        }
    }
    return (NULL);
}


// Consommateur
void *consumer(void *args) {
    prod_cons_args_t *arguments = (prod_cons_args_t *) args;

    while (true) {
        sem_wait(&full); // attente d'une place remplie
        pthread_mutex_lock(&mutex);
        if (consumed_elements < CYCLES) {
            // section critique
            for (int j = 0; j < BUFFER_SIZE; j++) {
                if (consumed_buffer[j] == 1) {
                    consumed_buffer[j] = 0;
                    consumed_elements++;
                    if (arguments->verbose) {
                        printf("Consumer #%d consumed index %d : %d\n", arguments->id, j, buffer[j]);
                        printf("consumed_buffer : [");
                        for (int loop = 0; loop < BUFFER_SIZE; loop++) {
                            printf("%d ", consumed_buffer[loop]);
                        }
                        printf("]\n");
                        printf("Consumed elements: %d\n", consumed_elements);
                    }
                }
            }
        } else {
            break;
        }

        pthread_mutex_unlock(&mutex);
        sem_post(&empty); // il y a une place libre en plus

        // faut bien travailler
        for (int i = 0; i < 10000; i++) {
            usleep(5);
        }
    }
    return (NULL);
}
