#ifndef STDIO_H
#define STDIO_H

#include <stdint.h>
#include <defs.h>
#include <videoDriver.h>
#include <keyboardDriver.h>

void read(uint64_t fd, char *buffer, uint64_t length);
void write(uint64_t fd, const char * string, uint64_t count);

void screenInfo(uint32_t * width, uint32_t * height);

void putRectangle(uint32_t hexColor, uint32_t x, uint32_t y, uint32_t base, uint32_t height);
void putCircle(uint32_t hexColor, uint32_t x, uint32_t y, uint8_t radius);


#endif
