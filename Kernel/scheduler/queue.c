#include  <include/process.h>
#include <stddef.h>
#include <stdint.h>
#include <include/queue.h>
Queue newQueue(){
    Queue queue = somalloc(sizeof(queue_t));
        if(queue == NULL){
            return NULL;
        }
    queue->first = NULL;
    queue->last = NULL;
    return queue;
}

void enqueue(Queue queue,process p){
    if(queue == NULL|| p==NULL){
        return;
    }
    Node node = somalloc(sizeof(node_t));
    if(node == NULL){
        return;
    }
    node->pcb = p;
    node->next = NULL;
    if(queue->first == NULL){
        queue->first = node;
        queue->last = node;
    }
    else{
        queue->last->next = node;
        queue->last = node;
    }

}

int dequeue(Queue queue,uint64_t pid){
    if(queue == NULL){
        return -1;
    }
    Node aux = queue->first;
    Node prev = NULL;
    while(aux != NULL){
        if(aux->pcb->pid == pid){
            if(prev == NULL){
                queue->first = aux->next;
            }
            else{
                prev->next = aux->next;
            }
            if(aux->next == NULL){
                queue->last = prev;
            }
            free(aux);
            return 1;
        }
        prev = aux;
        aux = aux->next;
    }
    return -1;

}
process getProcessFromPid(Queue queue,uint64_t pid){
    if(queue == NULL||queue->first==NULL){
        return NULL;
    }
    Node aux = queue->first;
    while(aux != NULL){
        if(aux->pcb->pid == pid){
            return aux->pcb;
        }
        aux = aux->next;
    }
    return NULL;
}