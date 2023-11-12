#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <process.h>
#include <processQueue.h>
#include <memoryManager.h>
#include <lib.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_PROCESS_COUNT 100
#define PRIORITY_LEVELS 10
#define DUMMY_PID 0

void initializeScheduler();
void * contextSwitch(void * RSP);

void addProcess(void (* program)(int argc, char ** argv), char *name, int argc, char ** argv, uint8_t priority);
void killProcess(uint64_t pid);
void killCurrentProcess();

uint64_t getCurrentPid();
void setProcessPriority(uint64_t pid, uint8_t priority);
void blockProcess(uint64_t pid);
void unblockProcess(uint64_t pid);
void waitForChildren(uint64_t pid);
void yield();

void schedulerInfo();
#endif