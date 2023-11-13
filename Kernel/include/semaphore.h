#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include <scheduler.h>
#include <lib.h>
#include <memoryManager.h>
#include <process.h>

#define MAX_SEMAPHORES 100


int openSem(int id, unsigned int value);
int openNewSem(unsigned int value);
void closeSem(int id);

void postSem(int id);
void waitSem(int id);

void removeForAllSemaphores(uint64_t pid);

#endif