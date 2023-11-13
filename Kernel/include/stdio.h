#ifndef STDIO_H
#define STDIO_H

#include <stdint.h>
#include <defs.h>
#include <videoDriver.h>
#include <keyboardDriver.h>
#include <scheduler.h>
#include <process.h>
#include <pipe.h>

uint64_t fRead(char * buffer, uint64_t length);
uint64_t fWrite(uint64_t color, const char * string, uint64_t count);

uint64_t read(char *buffer, uint64_t length);
uint64_t write(uint64_t color, const char * string, uint64_t count);

void screenInfo(uint32_t * width, uint32_t * height);

void putRectangle(uint32_t hexColor, uint32_t x, uint32_t y, uint32_t base, uint32_t height);
void putCircle(uint32_t hexColor, uint32_t x, uint32_t y, uint8_t radius);


#endif
