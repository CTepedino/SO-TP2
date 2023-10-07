#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>
#include <stdlib.h>

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

#endif
