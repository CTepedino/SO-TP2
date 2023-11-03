#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <lib.h>

void initializeMemoryManager(void *initialAddress, uint64_t size);
void * memAlloc(uint64_t size);
void memFree(void * ptr);
void memoryInfo();



#endif
