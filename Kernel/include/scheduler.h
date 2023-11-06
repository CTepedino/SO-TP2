#include <stdint.h>
#include <process.h>
#include <queue.h>
#include <stddef.h>
#include <memoryManager.h>
#include <lib.h>
#include <interrupts.h>


#define STACK_SIZE 0x8000
#define REGISTER_AMOUNT 20
#define GENERAL_REGISTER_AMOUNT 15
#define START_STACK_SEGMENT 0x0
#define START_RFLAGS 0x202
#define START_CODE_SEGMENT 0x8
#define MAX_PRIORITY 8
#define MIN_PRIORITY 1
#define FOREGROUND 1
#define BACKGROUND 0

void initScheduler();
uint64_t createProcess(uint64_t *entryPoint, int foreground, uint64_t fdIn, uint64_t fdOut,
                           uint64_t first, uint64_t second, uint64_t third, char *name);
uint64_t * contextSwitch(uint64_t *rsp);
void blockProcessPid(uint64_t pid);
void yield();
void unlockProcessPid(uint64_t pid);
void blockProcessCurrent();
void unlockProcessCurrent();
void switchStates(unsigned int pid);
void killProcessPid(uint64_t pid);
void changePriority(uint64_t pid, uint8_t priority);
uint64_t getPid();
int addWaitingPid(uint64_t pid);
void exit();
void printProcesses();
void alter_process_state(uint64_t pid, uint8_t new_state);
void alter_from_state(uint8_t old_state, uint8_t new_state);