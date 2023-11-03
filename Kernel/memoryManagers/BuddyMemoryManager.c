#ifdef BUDDY

#include <memoryManager.h>

#define MIN 5
#define LEVELS 32

#define FREE 0
#define TAKEN 1

#define BLOCK_SIZE(i) (1<<(i))
#define BUDDY_OF(b, i)  ((Head *)((uint64_t) base +  (((uint64_t)((void *)(b) - base)) ^ (1 << (i)))))
#define HIDE_HEAD(b) ((void  *)(b) + sizeof(Head))
#define SHOW_HEAD(m) ((Head *) ((m) - sizeof(Head)))

typedef struct Head{
    uint8_t level;
    uint8_t status;
    struct Head * next;
    struct Head * prev;
} Head;

void * base;
uint8_t max;
Head * freeLists[LEVELS];

uint64_t usedMem;
uint64_t totalMem;


uint8_t level(uint64_t size);
Head * newHead(void * address, uint8_t level, Head *next);
void freeListsRemove(Head * block);
void split(uint8_t idx);
Head * merge(Head * block, Head * buddy);

void initializeMemoryManager(void *initialAddress, uint64_t size) {
    base = initialAddress;
    max = level(size);
    if (max < MIN){
        return;
    }
    totalMem = size;
    freeLists[max-1] = newHead(base, max, NULL);
}

void * memAlloc(uint64_t size){
    if (size==0){
        return NULL;
    }
    uint8_t targetLevel = level(size+sizeof(Head));
    targetLevel = targetLevel < MIN-1 ? MIN - 1 : targetLevel;
    if (targetLevel > max){
        return NULL;
    }
    if (freeLists[targetLevel]==NULL){
        uint8_t toSplit = targetLevel;
        uint8_t found = 0;
        while (!found){
            toSplit++;
            if (toSplit > max){
                return NULL;
            }
            if (freeLists[toSplit] != NULL){
                found=1;
            }
        }
        while (toSplit>targetLevel) {
            split(toSplit);
            toSplit--;
        }
    }
    Head * block = freeLists[targetLevel];
    freeListsRemove(block);
    block->status = TAKEN;
    usedMem += BLOCK_SIZE(block->level);
    return HIDE_HEAD(block);
}

void memFree(void * ptr){
    if (ptr == NULL){
        return;
    }
    Head * block = SHOW_HEAD(ptr);
    if (block->status == FREE){
        return;
    }
    usedMem -= BLOCK_SIZE(block->level);
    Head * buddy = BUDDY_OF(block,block->level);
    while (buddy->status == FREE && block->level < max){
        block = merge(block, buddy);
        buddy = BUDDY_OF(block, block->level);
    }
    freeLists[block->level-1] = newHead((void *) block, block->level, freeLists[block->level-1]);
}

void memoryInfo(){
    char buffer[400];
    printString("Buddy Memory Manager\nTotal Memory: ");
    intToString(totalMem, buffer, 10, uIntLen(totalMem, 10));
    printString(buffer);
    printString("\nAvailable: ");
    intToString(totalMem-usedMem, buffer, 10, uIntLen(totalMem-usedMem, 10));
    printString(buffer);
    printString("\nUsed: ");
    intToString(usedMem, buffer, 10, uIntLen(usedMem, 10));
    printString(buffer);
    printString("\n");
}


uint8_t level(uint64_t size){
    uint8_t i = 0;
    while (size >>= 1){
        i++;
    }
    return i;
}


Head * newHead(void * address, uint8_t level, Head *next){
    Head * block = (Head * ) address;
    block->level = level;
    block->status = FREE;
    block->next = next;
    block->prev = NULL;
    if (next){
        next->prev = block;
    }
    return block;
}

void freeListsRemove(Head * block){
    if (block->prev != NULL){
        block->prev->next = block->next;
    } else {
        freeLists[block->level-1] = block->next;
    }
    if (block->next != NULL){
        block->next->prev = block->prev;
    }
}

void split(uint8_t idx){
    Head * block = freeLists[idx];
    Head * buddy = BUDDY_OF(block, idx);
    freeListsRemove(block);
    newHead((void *) buddy, idx, freeLists[idx-1]);
    freeLists[idx-1] = newHead((void *) block, idx, buddy);
}

Head * merge(Head * block, Head * buddy){
    Head * primary,* secondary;
    if (block < buddy){
        primary = block;
        secondary = buddy;
    } else {
        primary = buddy;
        secondary = block;
    }
    freeListsRemove(secondary);
    primary->level++;
    return primary;
}

#endif