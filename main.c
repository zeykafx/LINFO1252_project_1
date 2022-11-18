#include <stdio.h>
#include <stdlib.h>
#include "./headers/philosophes.h"
#include "./headers/producers_consumers.h"
#include "./headers/readers_writers.h"
#include "./headers/args.h"

int main(int argc, char *argv[]) {
    options_t options;

    options_parser(argc, argv, &options);
    if (options.verbose) {
        printf("Verbose mode enabled - the program will output debug information.\n");
    }

    if (options.number_philosophers > 0) {
        philosophers(options.number_philosophers, options.verbose);
    }
    if (options.number_producers > 0 && options.number_consumers > 0) {
        producer_consumer(options.number_producers, options.number_consumers, options.verbose);
    }
    if (options.number_readers > 0 && options.number_writers > 0) {
        reader_writer(options.number_readers, options.number_writers, options.verbose);
    }

    return EXIT_SUCCESS;
}

