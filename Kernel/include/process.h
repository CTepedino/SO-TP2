#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <lib.h>
#include <pidList.h>
#include <memoryManager.h>
#include <scheduler.h>
#include <interrupts.h>
#include <processQueue.h>


#define STACK_SIZE (1024*4)
#define MAX_PIPE_COUNT 50

typedef struct FileDescriptors{
    int input;
    int output;
}FileDescriptors;

enum status {Ready=0, Running, Blocked, WaitingForChildren};

typedef struct Process{
    uint64_t pid;
    uint64_t ppid;
    enum status status;
    char * name;
    char ** argv;
    void * RBP;
    void * RSP;
    FileDescriptors fds;
    struct pidList * waitingList;
} Process;

Process * initializeProcess(uint64_t pid, uint64_t ppid, char * name, int argc, char ** argv, void (*program)(int argc, char ** argv), int fds[]);
void freeProcess(Process * process);



#endif

