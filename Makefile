# Makefile template copied and modified from https://github.com/pantuza/c-project-template


# Source code directory structure
BINDIR := bin
SRCDIR := src
LOGDIR := log
LIBDIR := lib
TESTDIR := test


# Source code file extension
SRCEXT := c

CC := gcc

STD := -std=gnu99

# Protection for stack-smashing attack
STACK := -fstack-protector-all -Wstack-protector

# Specifies to GCC the required warnings
WARNS := -Wall -Wextra -pedantic # -pedantic warns on language standards

# Flags for compiling
CFLAGS := -O3 $(STD) $(STACK) $(WARNS)

# Debug options
DEBUG := -g3 -DDEBUG=1

# Dependency libraries
LIBS := # -lm  -I some/path/to/library

# Test libraries
TEST_LIBS := -lcunit

# Tests binary file
TEST_BINARY := binary_test_runner


# %.o file names
NAMES := $(notdir $(basename $(wildcard $(SRCDIR)/*.$(SRCEXT))))
OBJECTS :=$(patsubst %,$(LIBDIR)/%.o,$(NAMES))


#
# COMPILATION RULES
#

default: all

# Help message
help:
	@echo "LINFO1252 Makefile help"
	@echo
	@echo "Target rules:"
	@echo "    all      - Compiles and generates binary file"
	@echo "    tests    - Compiles with cmocka and run tests binary file"
	@echo "    valgrind - Runs binary file using valgrind tool"
	@echo "    clean    - Clean the project by removing binaries"
	@echo "    help     - Prints a help message with target rules"


# Rule for link and generate the binary file
all: $(OBJECTS)
	@echo -en "LD ";
	$(CC) -o $(BINDIR)/binary $+ $(DEBUG) $(CFLAGS) $(LIBS)
	@echo -en "\n--\nBinary file placed at" \
			  "$(BINDIR)/binary\n";


# Rule for object binaries compilation
$(LIBDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo -en "CC ";
	$(CC) -c $^ -o $@ $(DEBUG) $(CFLAGS) $(LIBS)


# Rule for run valgrind tool
valgrind:
	valgrind \
		--track-origins=yes \
		--leak-check=full \
		--leak-resolution=high \
		--log-file=$(LOGDIR)/$@.log \
		$(BINDIR)/binary
	@echo -en "\nCheck the log file: $(LOGDIR)/$@.log\n"


# Compile tests and run the test binary
tests:
	@echo -en "CC";
	$(CC) $(TESTDIR)/main.c -o $(BINDIR)/$(TEST_BINARY) $(DEBUG) $(CFLAGS) $(LIBS) $(TEST_LIBS)
	@which ldconfig && ldconfig -C /tmp/ld.so.cache || true # caching the library linking
	@echo -en " Running tests: ";
	./$(BINDIR)/$(TEST_BINARY)


# Rule for cleaning the project
clean:
	@rm -rvf $(BINDIR)/* $(LIBDIR)/* $(LOGDIR)/*;
