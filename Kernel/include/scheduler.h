#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <process.h>
#include <processQueue.h>
#include <memoryManager.h>
#include <lib.h>
#include <stdint.h>
#include <stdlib.h>
#include <semaphore.h>

#define MAX_PROCESS_COUNT 100
#define PRIORITY_LEVELS 10
#define DUMMY_PID 0

void initializeScheduler();
void * contextSwitch(void * RSP);

uint64_t addProcess(void (* program)(int argc, char ** argv), char *name, int argc, char ** argv, uint8_t priority, int fds[]);
void killProcess(uint64_t pid);
void killCurrentProcess();
void killForegroundProcess();

uint64_t getCurrentPid();
Process * getCurrentProcess();
void setProcessPriority(uint64_t pid, uint8_t priority);
void blockProcess(uint64_t pid);
void unblockProcess(uint64_t pid);
void waitForChildren(uint64_t pid);
void yield();

void schedulerInfo();
#endif