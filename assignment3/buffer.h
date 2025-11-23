#ifndef BUFFER_H
#define BUFFER_H

// Standard includes used by both producer and consumer
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>

// The original assignment uses POSIX shared memory and semaphores.
// These headers are not available on native Windows builds. If
// you are compiling on Windows, build under WSL/Cygwin/MSYS2 or
// otherwise use a POSIX-compatible environment. To make the
// intent explicit, we conditionally include POSIX headers here.
#if defined(_WIN32) || defined(_WIN64)
#warning "This code requires POSIX APIs (shm, semaphores). Build on WSL or Linux."
#else
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#endif

// Constants for shared memory and semaphores
#define BUFFER_SIZE 10
#define SHM_KEY 0x1234
#define SEM_MUTEX "/sem_mutex"
#define SEM_EMPTY "/sem_empty"
#define SEM_FULL "/sem_full"


// Basic item definition used by producer/consumer
typedef struct {
	int value;         // produced value
	int producer_id;   // which producer created i
} item_t;

// Shared buffer layout placed in shared memory
typedef struct {
	item_t buffer[BUFFER_SIZE];
	int head;    // next write index
	int tail;   // next read index
	int count; // number of items currently in buffer
} shared_buffer_t;

#endif
