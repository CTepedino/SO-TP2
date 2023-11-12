#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <lib.h>
#include <memoryManager.h>
#include <scheduler.h>
#include <interrupts.h>
#include <processQueue.h>

#define STACK_SIZE (1024*4)


enum status {Ready=0, Running, Blocked, WaitingForChildren};

typedef struct Process{
    uint64_t pid;
    uint64_t ppid;
    enum status status;
    char * name;
    char ** argv;
    void * RBP;
    void * RSP;
    uint64_t childrenCount;
} Process;

Process * initializeProcess(uint64_t pid, uint64_t ppid, char * name, int argc, char ** argv, void (*program)(int argc, char ** argv));
void freeProcess(Process * process);



#endif

