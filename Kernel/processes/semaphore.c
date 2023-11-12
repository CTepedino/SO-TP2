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
    char * name;
    uint64_t value;
    int mutex;
    pidList * waitingList;
    pidList * mutexList;
}Semaphore;

Semaphore * semaphores[MAX_SEMAPHORES] = {NULL};

int findSem(char * name);
void addToPidList(pidList * list, uint64_t pid);
void removeFromPidList(pidList * list, uint64_t pid);
void freePidList(pidList * list);
void lockSem(Semaphore * sem);
void unlockSem(Semaphore * sem);


int openSem(char * name, unsigned int value){
    if (findSem(name)!=-1){
        return 0;
    }
    for(int i = 0; i < MAX_SEMAPHORES; i++){
        if(semaphores[i]==NULL){
            semaphores[i] = memAlloc(sizeof(Semaphore));
            semaphores[i]->name = memAlloc(sizeof(name)+1);
            copyString(semaphores[i]->name, name);
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


void closeSem(char * name){
    int id = findSem(name);
    if (id==-1){
        return;
    }
    memFree(semaphores[id]->name);
    freePidList(semaphores[id]->waitingList);
    freePidList(semaphores[id]->mutexList);
    memFree(semaphores[id]);
    semaphores[id] = NULL;
}



void postSem(char * name){
    int id = findSem(name);
    if (id==-1){
        return;
    }
    Semaphore * sem = semaphores[id];
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

void waitSem(char * name){
    int id = findSem(name);
    if (id==-1){
        return;
    }
    Semaphore * sem = semaphores[id];
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

int findSem(char * name){
    for(int i = 0; i<MAX_SEMAPHORES;i++){
        if (semaphores[i] != NULL && stringCompare(name, semaphores[i]->name)==0){
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
