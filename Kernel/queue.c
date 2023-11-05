#include <queue.h>


Queue newQueue(){
    Queue queue = memAlloc(sizeof(queue_t));
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
    Node node = memAlloc(sizeof(node_t));
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

process dequeue(Queue queue){
    if(queue == NULL || queue->first == NULL){
        return -1;
    }
    Node aux = queue->first;
    queue->first = aux->next;
    process toReturn = aux->pcb;
    memFree(aux->pcb);
    return toReturn;
}

process dequeueReady(Queue queue){
    if (queue == NULL || queue->first == NULL) {
        return NULL;
    }
    while (queue->first->pcb->state != READY) {
        queue->last = queue->first;
        queue->first = queue->first->next;
    }

    process toReturn = queue->first->pcb;
    queue->last = queue->first;
    queue->first = queue->first->next;

    return toReturn;
}

int remove(Queue queue,uint64_t pid){
    if(queue == NULL){
        return -1;
    }
    Node aux = queue->first;
    Node prev = NULL;
    while(aux->pcb != NULL){
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
            memFree(aux->pcb);
            return 1;
        }
        prev = aux->pcb;
        aux->pcb = aux->next;
    }
    return -1;
}

process getProcessFromPid(Queue queue,uint64_t pid){
    if(queue == NULL||queue->first==NULL){
        return NULL;
    }
    Node aux = queue->first;
    while(aux->pcb != NULL){
        if(aux->pcb->pid == pid){
            return aux->pcb;
        }
        aux->pcb = aux->next;
    }
    return NULL;
}

static void freeQueueRec(Node first){
    if (first == NULL){
        return;
    }
    freeQueueRec(first->next);
    memFree(first);
}

void destroyQueue(Queue queue){
    if (queue == NULL) {
        return;
    }
    freeQueueRec(queue->first);
    free(queue);
}

int isEmpty(Queue queue){
    return queue->first == NULL;
}

void printQueue(Queue queue){
    char toPrintString[20] = {0};
    printString("PID         State    Prior    RSP                      RBP                      FG    Name\n");
    Node aux = queue->first;
    while (aux != NULL) {
        //PID
        intToString(aux->pcb->pid, toPrintString, 10, uIntLen(aux->pcb->pid, 10));
        printString(toPrintString);
        printString("    ");
        for (int i = 0; i < 8 - uIntLen(aux->pcb->pid, 10); i++) printString(" ");
        //State
        printString(aux->pcb->state == READY ? "Ready" : aux->pcb->state == BLOCKED ? "Blocked" : "Killed ");
        printString("    ");
        //Priority
        intToString(aux->pcb->priority, toPrintString, 10, uIntLen(aux->pcb->priority, 10));
        printString(toPrintString);
        printString("        ");
        //RSP
        intToString(aux->pcb->rsp, toPrintString, 16, uIntLen(aux->pcb->rsp, 16));
        printString("0x");
        printString(toPrintString);
        printString("    ");
        //RBP
        printString("0x");
        intToString(aux->pcb->rbp, toPrintString, 16, uIntLen(aux->pcb->rbp, 16));
        printString(toPrintString);
        printString("    ");
        //FG
        printString(aux->pcb->foreground == 0 ? "BG" : "FG");
        printString("    ");
        //Name
        printString(aux->pcb->name);
        printString("\n");
    }
}