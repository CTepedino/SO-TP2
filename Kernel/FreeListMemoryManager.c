#ifdef FREE_LIST

#include <memoryManager.h>

#define FREE 0
#define TAKEN 1

typedef struct FreeListNode{
    uint64_t blockSize;
    uint8_t status;
    struct FreeListNode * next;
    struct FreeListNode * prev;
} FreeListNode;


uint64_t totalMem;
uint64_t usedMem;
FreeListNode * freeList;

void split(FreeListNode * node, uint64_t offset);
void merge(FreeListNode * node);

void initializeMemoryManager(void *initialAddress, uint64_t size) {
    totalMem = size;
    freeList = (FreeListNode *) initialAddress;
    freeList->blockSize = size - sizeof(FreeListNode);
    freeList->next = NULL;
    freeList->prev = NULL;
    freeList->status = FREE;
}

void * memAlloc(uint64_t size){

    if (size==0){
        return NULL;
    }
    FreeListNode * node = freeList;
    while (node != NULL){
        if (node->status==FREE) {
            if (node->blockSize > size + sizeof(FreeListNode)) {
                split(node, size);
            }
            if (node->blockSize >= size) {
                node->status = TAKEN;
                usedMem += size;
                return (void *) (node + sizeof(FreeListNode));
            }
        }
        node = node->next;
    }
    return NULL;
}

void memFree(void * ptr){

    if (ptr==NULL){
        return;
    }
    FreeListNode * node = freeList;
    while (node != NULL){
        if (ptr == node + sizeof(FreeListNode)){
            usedMem -= node->blockSize;
            merge(ptr);
            return;
        }
        node = node->next;
    }
}

void memoryInfo(){
    char buffer[400];
    printString("Free List Memory Manager\nTotal Memory: ");
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

void split(FreeListNode * node, uint64_t offset){
    FreeListNode * newNode = (FreeListNode *) (node + offset);
    newNode->blockSize = node->blockSize - offset - sizeof(FreeListNode);
    node->blockSize = offset;
    newNode->status = FREE;
    newNode->prev = node;
    newNode->next = node->next;
    node->next = newNode;
}

void merge(FreeListNode * node){
    FreeListNode * right = node->next;
    if (right!=NULL && right->status==FREE){
        node->next = right->next;
        node->blockSize += sizeof(FreeListNode) + right->blockSize;
    }
    FreeListNode * left = node->prev;
    if (left!=NULL && left->status==FREE){
        left->next = node->next;
        left->blockSize += sizeof(FreeListNode) + node->blockSize;
    }
}

#endif