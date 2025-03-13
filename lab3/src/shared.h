#ifndef SHARED_H
#define SHARED_H

#include <semaphore.h>

#define BUFFER_SIZE 4096
#define SHARED_FILENAME "shared_memory.dat"

typedef struct {
    char buffer[BUFFER_SIZE];
    int data_ready;
    sem_t sem_parent;
    sem_t sem_child;
} SharedMemory;

#endif
