#include <semaphore.h>

typedef struct Node{
    uint64_t pid;
    struct Node * next;
    struct Node * prev;
} Node;

typedef struct pidList{
    Node * first;
    Node * last;
} pidList;

typedef struct Semaphore{
    uint64_t id;
    uint64_t value;
    int mutex;
    pidList * waitingList;
    pidList * mutexList;
}Semaphore;

Semaphore * semaphores[MAX_SEMAPHORES] = {NULL};

int findSem(int id);
void addToPidList(pidList * list, uint64_t pid);
void removeFromPidList(pidList * list, uint64_t pid);
void freePidList(pidList * list);
void lockSem(Semaphore * sem);
void unlockSem(Semaphore * sem);


int openSem(int id, unsigned int value){
    if (findSem(id)!=-1){
        return 0;
    }
    for(int i = 0; i < MAX_SEMAPHORES; i++){
        if(semaphores[i]==NULL){
            semaphores[i] = memAlloc(sizeof(Semaphore));
            semaphores[i]->id = id;
            semaphores[i]->value = value;
            semaphores[i]->mutex = 0;
            semaphores[i]->waitingList = memAlloc(sizeof(pidList));
            semaphores[i]->waitingList->first = NULL;
            semaphores[i]->waitingList->last = NULL;
            semaphores[i]->mutexList = memAlloc(sizeof(pidList));
            semaphores[i]->mutexList->first = NULL;
            semaphores[i]->mutexList->last = NULL;
            return 0;
        }
    }
    return -1;
}

int openNewSem(unsigned int value){
    for(int i = 0; i< MAX_SEMAPHORES; i++){
        if (semaphores[i]==NULL){
            int id = 1;
            while(1){
                if (findSem(id)==-1){
                    openSem(id, value);
                }
                id++;
            }

        }
    }
    return -1;
}


void closeSem(int id){
    int idx = findSem(id);
    if (idx==-1){
        return;
    }
    freePidList(semaphores[idx]->waitingList);
    freePidList(semaphores[idx]->mutexList);
    memFree(semaphores[idx]);
    semaphores[idx] = NULL;
}



void postSem(int id){
    int idx = findSem(id);
    if (idx==-1){
        return;
    }
    Semaphore * sem = semaphores[idx];
    lockSem(sem);
    sem->value++;
    if (sem->value != 0) {
        if (sem->waitingList->first != NULL){
            int pid = sem->waitingList->first->pid;
            removeFromPidList(sem->waitingList, pid);
            unblockProcess(pid);
        }
    }
    unlockSem(sem);
}

void waitSem(int id){
    int idx = findSem(id);
    if (idx==-1){
        return;
    }
    Semaphore * sem = semaphores[idx];
    lockSem(sem);
    while (sem->value == 0) {
        uint16_t pid = getCurrentPid();
        addToPidList(sem->waitingList, pid);
        unlockSem(sem);
        blockProcess(pid);
        lockSem(sem);
    }
    sem->value--;
    unlockSem(sem);
}

int findSem(int id){
    for(int i = 0; i<MAX_SEMAPHORES;i++){
        if (semaphores[i] != NULL && semaphores[i]->id==id){
            return i;
        }
    }
    return -1;
}

void addToPidList(pidList * list, uint64_t pid){
    Node * n = memAlloc(sizeof(Node));
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
    Node * curr = list->first;
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
    Node * curr = list->first;
    Node * next;
    while(curr!=NULL){
        next = curr->next;
        memFree(curr);
        curr = next;
    }
}

void lockSem(Semaphore *sem) {
    while (_xchg(&(sem->mutex), 1)) {
        uint16_t pid = getCurrentPid();
        addToPidList(sem->mutexList, pid);
        blockProcess(pid);
    }
}

void unlockSem(Semaphore *sem) {
    if (sem->mutexList->first != NULL){
        int pid = sem->mutexList->first->pid;
        removeFromPidList(sem->mutexList, pid);
        unblockProcess(pid);
    }
    sem->mutex = 0;
}


void removeForAllSemaphores(uint64_t pid){
    for(int i = 0; i<MAX_SEMAPHORES; i++){
        if (semaphores[i]!=NULL){
            removeFromPidList(semaphores[i]->waitingList, pid);
            removeFromPidList(semaphores[i]->mutexList, pid);
        }
    }
}
