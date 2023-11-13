#include <pidList.h>

pidList * initializePidList(){
    pidList * list = memAlloc(sizeof(pidList));
    list->first = NULL;
    list->last = NULL;
    list->iterator = NULL;
    return list;
}

void addToPidList(pidList * list, uint64_t pid){
    pNode * n = memAlloc(sizeof(pNode));
    n->pid = pid;
    n->next = NULL;
    if (list->first==NULL){
        n->prev = NULL;
        list->first = n;
        list->last = n;
    } else {
        n->prev = list->last;
        list->last->next = n;
        list->last = n;
    }
}

void removeFromPidList(pidList * list, uint64_t pid){
    pNode * curr = list->first;
    while (curr != NULL) {
        if (curr->pid == pid) {
            break;
        }
        curr = curr->next;
    }
    if (curr!= NULL){
        if (list->first==curr){
            list->first = curr->next;
        } else {
            curr->prev->next = curr->next;
        }
        if (list->last==curr){
            list->last = curr->prev;
        } else {
            curr->next->prev = curr->prev;
        }
        memFree(curr);
    }

}

void freePidList(pidList * list){
    pNode * curr = list->first;
    pNode * next;
    while(curr!=NULL){
        next = curr->next;
        memFree(curr);
        curr = next;
    }
}

uint8_t isEmptyPidList(pidList * list){
    return list->first==NULL;
}

void startPidIterator(pidList * list){
    list->iterator = list->first;
}

uint8_t hasNextPid(pidList * list){
    return list->iterator != NULL;
}

uint64_t nextPid(pidList * list){
    uint64_t toReturn = list->iterator->pid;
    list->iterator = list->iterator->next;
    return toReturn;
}
