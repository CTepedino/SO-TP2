#ifndef GENERALLIB_H
#define GENERALLIB_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define COLOR 3

typedef struct timeStruct{
    uint32_t year;
    uint32_t month;
    uint32_t dayOfMonth;
    uint32_t dayOfWeek;
    uint32_t hour;
    uint32_t minute;
    uint32_t second;
} timeStruct;

void putChar(char c);
void print(const char* string);
void colorPrint(const char * string);
void println(const char * string);
void printInt(uint64_t n);

void textPosition(uint32_t x, uint32_t y);
void screenInfo(uint32_t * width, uint32_t * height);

char getChar();
void scan(char * buf, uint64_t length);
void getCurrentKeyPress(char * keys);
void getCurrentReleasedKeys(char * rkeys);

void getTime(timeStruct * time);

void fillRegData(uint64_t regData[17]);

void getREGS(int * status,uint64_t * buffer);

void drawRectangle(uint32_t x, uint32_t y, uint32_t base, uint32_t height);
void drawCircle(uint32_t x, uint32_t y, uint8_t radius);
void clearRectangle(uint32_t x, uint32_t y, uint32_t base, uint32_t height);
void clearCircle(uint32_t x, uint32_t y, uint8_t radius);
void clearScreen();

void beep(uint8_t frequency, uint64_t ticks);

void sleep(uint64_t ticks);

void * memAlloc(size_t n);
void memFree(void * ptr);
void memoryInfo();
#endif