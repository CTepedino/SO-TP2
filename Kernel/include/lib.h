#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include <stdio.h>

typedef struct timeStruct{
    uint32_t year;
    uint32_t month;
    uint32_t dayOfMonth;
    uint32_t dayOfWeek;
    uint32_t hour;
    uint32_t minute;
    uint32_t second;
} timeStruct;

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);


unsigned long strlen(const char* str);
void intToString(uint64_t n, char * buffer, uint8_t base, uint8_t intLength);
uint32_t uIntLen(uint64_t num, uint8_t base);
void copyString(char * dst, char * src);
void printString(const char * s);
void printInt(uint64_t n);

void saveREGS(uint64_t * RSP);
void getREGS(int * status, uint64_t * buffer);

char *cpuVendor(char *result);
uint32_t getTime(int id);
void setTimeFormat();
void getRTC(timeStruct * time);

void forceTimerTick();
void * initializeStack(void (*start)(void (*program)(int argc, char ** argv), int argc, char ** argv), void (* program)(int argc, char ** argv), void * rsp, int argc, char ** argv);

#endif