#ifndef QUEUE_H
#define QUEUE_H

#include <process.h>
#include <stdint.h>
#include <stddef.h>
#include <memoryManager.h>
#include <lib.h>

typedef struct node_t{
    struct node_t *next;
    process pcb; 
} node_t;

typedef node_t*Node;

typedef struct queue_t{
    Node first;
    Node last;
    uint64_t size;
} queue_t;

typedef queue_t*Queue;

Queue newQueue();
void enqueue(Queue queue,process p);
process dequeue(Queue queue);
process dequeueReady(Queue queue);
int remove(Queue queue,uint64_t pid);
process getProcessFromPid(Queue queue,uint64_t pid);
void destroyQueue(Queue queue);
int isEmpty(Queue queue);
void printQueue(Queue queue);
void alter_state_if(Queue queue, uint8_t old_state, uint8_t new_state);

#endif