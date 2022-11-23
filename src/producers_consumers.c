#include "../headers/producers_consumers.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../headers/test_and_set_lock.h"
#include "../headers/my_semaphore.h"

mutex_t *mutex;
semaphore_t empty;
semaphore_t full;

volatile int produced_elements = 0;
volatile int consumed_elements = 0;


void producer_consumer(int n_prods, int n_cons, bool verbose) {

    pthread_t prods[n_prods], cons[n_cons];

    mutex = mutex_init();

    semaphore_init(&empty, BUFFER_SIZE);
    semaphore_init(&full, 0);
//    int error = pthread_mutex_init(&mutex, NULL);
//    if (error != 0) {
//        perror("Failed to init prod cons mutex");
//        exit(EXIT_FAILURE);
//    }
//
//    sem_init(&empty, 0, BUFFER_SIZE);  // buffer vide
//    sem_init(&full, 0, 0);             // buffer vide


    // shared buffer, this will store the produced items
    buffer = malloc(BUFFER_SIZE * sizeof(int));
    if (buffer == NULL) {
        perror("Failed to malloc shared buffer");
        exit(EXIT_FAILURE);
    }

    // shared buffer used to indicate which index of the normal shared buffer has a produced
    // item ready to be consumed
    consumed_buffer = calloc(BUFFER_SIZE, sizeof(int));
    if (consumed_buffer == NULL) {
        perror("Failed to malloc consumed buffer");
        exit(EXIT_FAILURE);
    }

    // PRODUCERS initialization ------
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

    // CONSUMERS initialization ---
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

        cons_args[i]->id = i;
        cons_args[i]->verbose = verbose;

        pthread_create(&cons[i], NULL, consumer, (void *) cons_args[i]);
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

    if (verbose) {
        printf("Joined all producers\n");
    }

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

    if (verbose) {
        printf("Joined all consumers\n");
    }

    free(consumed_buffer);
    free(buffer);

    mutex_destroy(mutex);
//    int err = pthread_mutex_destroy(&mutex);
//    if (err != 0) {
//        perror("Failed to destroy prod cons mutex");
//        exit(EXIT_FAILURE);
//    }

    semaphore_destroy(&empty);
    semaphore_destroy(&full);
//    sem_destroy(&empty);
//    sem_destroy(&full);

    if (verbose) {
        printf("Finished running the Producer Consumers problem\n");
    }
}


// Producer: produces integers and places them in the shared buffer at the first possible index
void *producer(void *args) {
    prod_cons_args_t *arguments = (prod_cons_args_t *) args;

    while (true) {
        // produce elements as long as all the producers haven't produced 8192 elements combined
        if (produced_elements < CYCLES) {

//            sem_wait(&empty); // attente d'une place libre
//            pthread_mutex_lock(&mutex);
            semaphore_wait(&empty);
            lock_test_and_test_and_set(mutex);
            // section critique

                // search for the first free index where we can place our produced integer
                // (which is just the thread id)
                for (int j = 0; j < BUFFER_SIZE; j++) {
                    if (consumed_buffer[j] == 0) {
                        buffer[j] = arguments->id; // produce an integer
                        produced_elements++;

                        // this lets the consumer know that an element at index j is ready to be consumed
                        consumed_buffer[j] = 1;

                        if (arguments->verbose) {
                            printf("Producer #%d produced element index %d: %d\n", arguments->id, j, buffer[j]);
                            printf("Produced elements: %d\n", produced_elements);
                        }

                        // break after we produced an element,
                        // otherwise this will continue to produce while the mutex is locked
                        break;
                    }
                }

            unlock(mutex);
            semaphore_post(&full);
//            pthread_mutex_unlock(&mutex);
//            sem_post(&full); // il y a une place remplie en plus

        } else {
            // else if all the elements have been produced, exit the loop
//            sem_post(&full);
            semaphore_post(&full);
            break;
        }

        // simulate busy work
        for (int i = 0; i < BUSY_WORK_CYCLES; i++) {}

    }
    pthread_exit(NULL);
}

// Consumer: reads the integers from the shared buffer
void *consumer(void *args) {
    prod_cons_args_t *arguments = (prod_cons_args_t *) args;

    while (true) {
        // same thing as producers, run until all the 8192 elements have been consumed
        if (consumed_elements < CYCLES) {
            semaphore_wait(&full);
            lock_test_and_test_and_set(mutex);
//            sem_wait(&full); // attente d'une place remplie
//            pthread_mutex_lock(&mutex);
            // section critique

                // looping over consumed_buffer until an element is 1, meaning the element at the
                // same index in buffer is ready to be consumed
                for (int j = 0; j < BUFFER_SIZE; j++) {
                    if (consumed_buffer[j] == 1) {

                        // we have nothing to do with the produced element, so we kinda just ignore it,
                        // it will be overwritten by a producer
                        // int element = buffer[j]; // this is the element produced

                        // reset index j to 0 since we "consumed" the element
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

                        // break after we consumed an element, to avoid having this run too much
                        break;
                    }
                }

            unlock(mutex);
            semaphore_post(&empty);
//            pthread_mutex_unlock(&mutex);
//            sem_post(&empty); // il y a une place libre en plus

        } else {
            // if we consumed all the elements, then we exit
//            sem_post(&empty);
            semaphore_post(&empty);
            break;
        }

        // simulate busy work
        for (int i = 0; i < BUSY_WORK_CYCLES; i++) {}

    }
    pthread_exit(NULL);
}
