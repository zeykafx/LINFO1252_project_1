#pragma once

#define WRITER_CYCLES 640
#define READER_CYCLES 2560

void *reader_writer();

void *writer(void *args);

void *reader(void *args);

