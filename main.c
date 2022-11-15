#include <stdio.h>
#include <stdlib.h>
#include "./headers/philosophes.h"
#include "./headers/args.h"

int main(int argc, char *argv[]) {

    options_t options;

    options_parser(argc, argv, &options);
    if (options.verbose) {
        printf("Verbose mode enabled - the program will output debug information.\n");
    }
    philosophers(options.number_philosophers, options.verbose);

    return EXIT_SUCCESS;
}

