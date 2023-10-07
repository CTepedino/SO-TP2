#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <stdlib.h>

typedef struct MemoryManagerCDT * MemoryManagerADT;

MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager, void * const restrict managedMemory);

void * allocMemory(MemoryManagerADT const restrict memoryManager, const size_t memoryToAllocate);

extern MemoryManagerADT memoryManager;

#endif

