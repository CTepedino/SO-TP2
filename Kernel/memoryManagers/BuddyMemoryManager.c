#ifdef BUDDY
#include <memoryManager.h>

#define MIN 5
#define LEVELS 32

#define FREE 0
#define TAKEN 1

#define BLOCK_SIZE(l) (1<<(l))
#define BUDDY_OF(b)  ((Header *)((uint64_t) base +  (((uint64_t)((void *)(b) - base)) ^ (1 << (b->level)))))
#define HIDE_HEADER(b) ((void  *)(b) + sizeof(Header))
#define SHOW_HEADER(m) ((Header *) ((m) - sizeof(Header)))

typedef struct Header {
	uint8_t level;
	uint8_t status;
	struct Header * prev;
	struct Header * next;
} Header;

void * base;
uint8_t max;
Header * freeLists[LEVELS];
uint64_t totalMem;
uint64_t usedMem;

uint8_t getLevel(uint64_t size);
Header *newHeader(void * address, uint8_t level, Header * next);
void freeListsRemove(Header * header);
void split(uint8_t idx);
Header *merge(Header * block, Header * buddy);


void initializeMemoryManager(void *initialAddress, uint64_t size){
	base = initialAddress;
	max = getLevel(size);
    totalMem = size;
	freeLists[max - 1] = newHeader(base, max, NULL);
}

void * memAlloc(uint64_t size) {
    uint8_t level = getLevel(size+sizeof(Header));
    if (level < MIN-1){
        level = MIN-1;
    }
    if (level >= max){
        return NULL;
    }
    uint8_t toSplit = level;
    while(1){
        if (toSplit>=max){
            return NULL;
        }
        if (freeLists[toSplit] != NULL){
            break;
        }
        toSplit++;
    }
    while(toSplit>level){
        split(toSplit);
        toSplit--;
    }
    Header *block = freeLists[level];
	freeListsRemove(block);
	block->status = TAKEN;
    usedMem += BLOCK_SIZE(block->level);
    return HIDE_HEADER(block);
}

void memFree(void * ptr) {
    if (ptr==NULL){
        return;
    }
	Header * block = SHOW_HEADER(ptr);
    block->status = FREE;
    usedMem -= BLOCK_SIZE(block->level);
    for (Header * buddy = BUDDY_OF(block); buddy->status==FREE && buddy->level==block->level && block->level < max; buddy = BUDDY_OF(block)){
        block = merge(block, buddy);
    }
	freeLists[block->level - 1] = newHeader(block, block->level, freeLists[block->level - 1]);
}

void memoryInfo(){
    printString("Buddy Memory Manager\n");
    printString("Total: ");
    printInt(totalMem);
    printString("\nDisponible: ");
    printInt(totalMem - usedMem);
    printString("\nEn uso: ");
    printInt(usedMem);
    printString("\n");
}



uint8_t getLevel(uint64_t size){
    uint8_t i = 0;
    while (size >>= 1){
        i++;
    }
    return i;
}

Header * newHeader(void * address, uint8_t level, Header *next) {
	Header * header = (Header *) address;
	header->level = level;
	header->status = FREE;
	header->prev = NULL;
	header->next = next;
	if (next != NULL) {
		next->prev = header;
	}
	return header;
}

void freeListsRemove(Header * header) {
	if (header->prev == NULL){
        freeLists[header->level - 1] = header->next;
    } else{
		header->prev->next = header->next;
    }
	if (header->next != NULL){
		header->next->prev = header->prev;
    }
}

void split(uint8_t idx) {
	Header *block = freeLists[idx];
	freeListsRemove(block);
	Header *buddy = (Header *) ((void*)block + (1 << idx));
	newHeader(buddy, idx, freeLists[idx - 1]);
	freeLists[idx - 1] = newHeader(block, idx, buddy);
}

Header * merge(Header *block, Header *buddy) {
	freeListsRemove(buddy);
	Header *blockStart = block < buddy ? block : buddy;
	blockStart->level++;
	return blockStart;
}

#endif