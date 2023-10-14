#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H
#include <stdint.h>
#include <stdlib.h>

void initializeMemoryManager(void *initialAddress, uint64_t size);
void * memAlloc(uint32_t size);
void memFree(void * ptr);

#endif
