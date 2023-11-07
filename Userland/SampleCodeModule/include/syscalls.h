#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>
#include <stdlib.h>
#include <generalLib.h>

void sys_read(uint64_t fd, char *buf, uint64_t count);
void sys_write(uint64_t fd, const char *buf, uint64_t count);
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
void sys_malloc(size_t n, void * mem);
void sys_free(void * ptr);
void sys_memoryInfo();
void sys_kill(uint64_t pid);
void sys_nice(uint64_t pid, uint8_t priority);
void sys_block(uint64_t pid);
void sys_ps();
uint64_t sys_getpid();
uint64_t sys_createProcess(uint64_t *entryPoint, uint8_t foreground, uint64_t first, uint64_t second, uint64_t third, char *name);
void sys_unblock(uint64_t pid);
void sys_yield();
void sys_waitPid(uint64_t pid);
void sys_exit();
#endif
