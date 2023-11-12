#ifndef PROCESSQUEUE_H
#define PROCESSQUEUE_H

#include <memoryManager.h>
#include <process.h>
#include <stdlib.h>

typedef struct ProcessQueueCDT * ProcessQueueADT;
typedef struct Process Process;


ProcessQueueADT initializeProcessQueue();
void freeQueue(ProcessQueueADT queue);

void enqueue(ProcessQueueADT queue, Process * process);
Process * dequeue(ProcessQueueADT queue);
Process * peek(ProcessQueueADT queue);
Process * remove(ProcessQueueADT queue, uint64_t pid);
Process * findProcessInQueue(ProcessQueueADT queue, uint64_t pid);

int isEmpty(ProcessQueueADT queue);
void startIterator(ProcessQueueADT queue);
int hasNext(ProcessQueueADT queue);
Process * next(ProcessQueueADT queue);

#endif
