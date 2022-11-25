#include <stdio.h>
#include <stdlib.h>
#include "./headers/philosophes.h"
#include "./headers/producers_consumers.h"
#include "./headers/readers_writers.h"
#include "./headers/args.h"
#include "headers/test_and_set_lock.h"
#include "./headers/my_semaphore.h"

int main(int argc, char *argv[]) {
    options_t options;

    options_parser(argc, argv, &options);
    if (options.verbose) {
        printf("Verbose mode enabled - the program will output debug information.\n");
    }

    if (options.number_philosophers > 0) {
        philosophers(options.number_philosophers, options.verbose, options.running_problems_with_pthread_sync);
    }
    if (options.number_producers > 0 && options.number_consumers > 0) {
        producer_consumer(options.number_producers, options.number_consumers, options.verbose,
                          options.running_problems_with_pthread_sync);
    }
    if (options.number_readers > 0 && options.number_writers > 0) {
        reader_writer(options.number_readers, options.number_writers, options.verbose,
                      options.running_problems_with_pthread_sync);
    }
    if (options.number_test_and_set_lock_threads > 0 || options.number_test_and_test_and_set_threads > 0) {
        bool is_simple_test_and_set = options.number_test_and_set_lock_threads > 0;
        test_and_set_lock(options.verbose, options.number_test_and_set_lock_threads,
                          options.number_test_and_test_and_set_threads, is_simple_test_and_set);
    }
    if (options.number_sem_test_threads > 0) {
        test_semaphore(options.number_sem_test_threads, options.verbose);
    }

    return EXIT_SUCCESS;
}

