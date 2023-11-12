#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include <scheduler.h>
#include <lib.h>
#include <memoryManager.h>
#include <process.h>

#define MAX_SEMAPHORES 50


int openSem(char * name, unsigned int value);
void closeSem(char * name);

void postSem(char * name);
void waitSem(char * name);

void removeForAllSemaphores(uint64_t pid);

#endif