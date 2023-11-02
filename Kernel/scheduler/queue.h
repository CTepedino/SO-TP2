#include <scheduler/process.h>
#include <stdint.h>
typedef struct node_t{
    
    struct node_t *next;
    process pcb; 
} node_t;
typedef node_t*Node;

typedef struct queue_t{
    Node first;
    Node last;
   
} queue_t;
typedef queue_t*Queue;

Queue newQueue();
void enqueue(Queue queue,process p);
int dequeue(Queue queue,uint64_t pid);
process getProcessFromPid(Queue queue,uint64_t pid);