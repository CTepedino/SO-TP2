#ifndef	_QUEUE_H
#define _QUEUE_H

#include <stdint.h>
#include <memoryManager.h>
#include <stdbool.h>

typedef struct queueADT{
    uint64_t  * array;
    unsigned int readPos;
    unsigned int writePos;
    unsigned int amount; 
    unsigned int size;
}queueADT;

void init_queue(queueADT * q, unsigned int size);
unsigned int size_queue(queueADT * q);
uint64_t dequeue(queueADT * q);
void enqueue(queueADT * q, uint64_t elem);
uint8_t contais_queue(queueADT * q, uint64_t elem);
void destroy_queue(queueADT * q);


#endif