#include <processQueue.h>

typedef struct Node{
    Process * process;
    struct Node * next;
    struct Node * prev;
}Node;

struct ProcessQueueCDT{
    Node * first;
    Node * last;
    Node * iterator;
};

ProcessQueueADT initializeProcessQueue(){
    ProcessQueueADT newQueue = memAlloc(sizeof(struct ProcessQueueCDT));
    newQueue->first = NULL;
    newQueue->last = NULL;
    return newQueue;
}

void freeQueue(ProcessQueueADT queue){
    Node * curr = queue->first;
    Node * next;
    while(curr!=NULL){
        next = curr->next;
        memFree(curr);
        curr = next;
    }
    memFree(queue);
}

void enqueue(ProcessQueueADT queue, Process * process){
    Node * newNode = memAlloc(sizeof(Node));
    newNode->process = process;
    if (queue->last == NULL){
        queue->first = newNode;
    } else {
        queue->last->next = newNode;
    }
    newNode->prev = queue->last;
    queue->last = newNode;
    newNode->next = NULL;
}

Process * dequeue(ProcessQueueADT queue){
    if (isEmpty(queue)){
        return NULL;
    }
    Node * first = queue->first;
    Process * toReturn = first->process;
    if (queue->first == queue->last){
        queue->first = NULL;
        queue->last = NULL;
    } else {
        queue->first = queue->first->next;
        queue->first->prev = NULL;
    }
    memFree(first);
    return toReturn;
}

Process * peek(ProcessQueueADT queue){
    return queue->first->process;
}

Process * remove(ProcessQueueADT queue, uint64_t pid){
    Node * curr = queue->first;
    while (curr != NULL){
        if (curr->process->pid == pid){
            break;
        }
        curr = curr->next;
    }
    if (curr==NULL){
        return NULL;
    }
    Process * toReturn = curr->process;
    if (queue->first==curr){
        queue->first = curr->next;
    } else {
        curr->prev->next = curr->next;
    }
    if (queue->last==curr){
        queue->last = curr->prev;
    } else {
        curr->next->prev = curr->prev;
    }
    memFree(curr);
    return toReturn;
}

Process * findProcessInQueue(ProcessQueueADT queue, uint64_t pid){
    Node * curr = queue->first;
    while (curr != NULL){
        if (curr->process->pid == pid){
            return curr->process;
        }
        curr = curr->next;
    }
    return NULL;
}

int isEmpty(ProcessQueueADT queue){
    return queue->first == NULL;
}

void startIterator(ProcessQueueADT queue){
    queue->iterator = queue->first;
}

int hasNext(ProcessQueueADT queue){
    return queue->iterator != NULL;
}

Process * next(ProcessQueueADT queue){
    Process * toReturn = queue->iterator->process;
    queue->iterator = queue->iterator->next;
    return toReturn;
}
