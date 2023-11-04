#include <stdint.h>
#include <process.h>

#define MAX_PRIORITY 8

void initScheduler();
void addProcess(process p);
void blockProcessPid(uint64_t pid);
int getRunningPid();
void yield();
void unlockProcessPid(uint64_t pid);
void blockProcessCurrent();
void unlockProcessCurrent();
void killProcessPid(uint64_t pid);
void changePriority(uint64_t pid, uint8_t priority);