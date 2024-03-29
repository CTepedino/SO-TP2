#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <generalLib.h>

uint32_t GetUint();
uint32_t GetUniform(uint32_t max);
uint8_t memcheck(void *start, uint8_t value, uint32_t size);
int64_t satoi(char *str);
void bussy_wait(uint64_t n);
void endless_loop();
void endless_loop_print();
void * memset2(void * destination, int32_t c, uint64_t length);

#endif