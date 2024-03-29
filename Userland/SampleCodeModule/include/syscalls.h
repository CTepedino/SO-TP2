#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>
#include <stdlib.h>

void sys_read(char *buf, uint64_t count);
void sys_write(uint64_t color, const char *buf, uint64_t count);
void sys_screenInfo(uint32_t * width, uint32_t * height);
void sys_textPosition(uint32_t x, uint32_t y);
void sys_getRTC(timeStruct * time);
void sys_getREGS(int * status, uint64_t * buffer);
void sys_beep(uint8_t frequency, uint64_t ticks);
void sys_putRectangle(uint32_t hexColor, uint32_t x, uint32_t y, uint32_t base, uint32_t height);
void sys_putCircle(uint32_t hexColor, uint32_t x, uint32_t y, uint8_t radius);
void sys_getCurrentKeyPress(char * keys);
void sys_getCurrentReleasedKeys(char * rkeys);
void sys_sleep(uint64_t ticks);
void * sys_malloc(size_t n);
void sys_free(void * ptr);
void sys_memoryInfo();
uint64_t sys_addProcess(void (* program)(int argc, char ** argv), char *name, int argc, char ** argv, uint8_t priority, int fds[]);
void sys_killProcess(uint64_t pid);
void sys_killCurrentProcess();
uint64_t sys_getCurrentPid();
void sys_setProcessPriority(uint64_t pid, uint64_t priority);
void sys_blockProcess(uint64_t pid);
void sys_unblockProcess(uint64_t pid);
void sys_waitForChildren(uint64_t childPid);
void sys_yield();
void sys_schedulerInfo();
int sys_openSem(int id, unsigned int value);
void sys_closeSem(int id);
void sys_postSem(int id);
void sys_waitSem(int id);
int sys_openNewSem(unsigned int value);
int sys_openPipe(int id, uint8_t mode);
int sys_createNewPipe();
void sys_closePipe(int id);
int sys_readPipe(int id, char * buffer, uint64_t length);
int sys_writePipe(int id, const char * string,uint64_t count);
int sys_getStatus(uint64_t pid);

#endif
