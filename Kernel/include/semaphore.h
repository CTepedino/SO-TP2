#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#define SUCCESS 0
#define ERROR_SEM_ID_EXISTS -1
#define ERROR_NO_MORE_SPACE -3
#define INVALID_SEM_ID -2

#define SEM_CONTINUE 0
#define SEM_BLOCK 1

#include <scheduler.h>
#include <queue.h>

int _xchg(unsigned int * lock, int value);


int open_sem_available(unsigned int value);

int open_sem(unsigned int sem_id, unsigned int value);
int close_sem(unsigned int sem_id);

int post_sem(unsigned int sem_id);
int wait_sem(unsigned int sem_id);

#endif