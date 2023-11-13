#ifndef PIDLIST_H
#define PIDLIST_H

#include <memoryManager.h>

typedef struct pNode{
    uint64_t pid;
    struct pNode * next;
    struct pNode * prev;
} pNode;

typedef struct pidList{
    pNode * first;
    pNode * last;
    pNode * iterator;
} pidList;


pidList * initializePidList();
void addToPidList(pidList * list, uint64_t pid);
void removeFromPidList(pidList * list, uint64_t pid);
void freePidList(pidList * list);

uint8_t isEmptyPidList(pidList * list);
void startPidIterator(pidList * list);
uint8_t hasNextPid(pidList * list);
uint64_t nextPid(pidList * list);

#endif
