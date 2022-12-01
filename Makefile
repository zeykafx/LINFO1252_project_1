# Makefile template copied and modified from https://github.com/pantuza/c-project-template

# Source code directory structure
BINDIR := bin
SRCDIR := src
LOGDIR := log
LIBDIR := lib
TESTDIR := test

CC := gcc

STD := -std=gnu99

# Specifies to GCC the required warnings
WARNS := -Wall -Wextra -pedantic # -pedantic warns on language standards

# Flags for compiling
CFLAGS := -I headers/ $(STD) $(WARNS)

# Debug options
DEBUG := -g3 -DDEBUG=1

# Dependency libraries
LIBS := -lpthread

# %.o file names
NAMES := $(notdir $(basename $(wildcard $(SRCDIR)/*.c)))
OBJECTS :=$(patsubst %,$(LIBDIR)/%.o,$(NAMES))


default: all

# Rule for object binaries compilation
$(LIBDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $^ -o $@ $(DEBUG) $(CFLAGS) $(LIBS)

# Clean, compile, and run binary
run: clean all
	./$(BINDIR)/binary -s 8

# link and generate the binary file
all: main.c $(OBJECTS)
	$(CC) -o $(BINDIR)/binary $+ $(DEBUG) $(CFLAGS) $(LIBS)
	@echo "\nBinary file placed at" \
			  "$(BINDIR)/binary\n";


# Rule for cleaning the project
clean:
	@rm -rvf $(BINDIR)/* $(LIBDIR)/* $(LOGDIR)/*;

.PHONY: clean tests

valgrind: clean all
	valgrind \
		--track-origins=yes \
		--leak-check=full \
		--show-leak-kinds=all \
		--leak-resolution=high \
		--log-file=$(LOGDIR)/$@.log \
		$(BINDIR)/binary -p 4 -c 4 -o
	@echo "\nCheck the log file: $(LOGDIR)/$@.log\n"

philosophers: clean all
	./$(BINDIR)/binary -v -N 8

producer_consumer: clean all
	./$(BINDIR)/binary -v -p 4 -c 4

reader_writer: clean all
	./$(BINDIR)/binary -v -r 4 -w 4

test_and_set: clean all
	./$(BINDIR)/binary -v -l 8

test_and_test_and_set: clean all
	./$(BINDIR)/binary -v -t 8

semaphore: clean all
	./$(BINDIR)/binary -v -s 8

graphs:
	./experiments.sh false
	./experiments.sh true
	./analysis.py
	@echo "done"
