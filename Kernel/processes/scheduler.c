#include <scheduler.h>

uint8_t usedPIDs[MAX_PROCESS_COUNT] = {0};
ProcessQueueADT queues[PRIORITY_LEVELS];
Process * currentProcess;
Process * dummyProcess;
int quantum;
int remainingQuantum;

void dummy(int argc, char ** argv);
Process * findProcess(uint64_t pid);

void initializeScheduler(){
    for(int i = 0; i < PRIORITY_LEVELS; i++){
        queues[i] = initializeProcessQueue();
    }
    dummyProcess = initializeProcess(DUMMY_PID, DUMMY_PID, "dummy", 0, NULL, &dummy);
    usedPIDs[DUMMY_PID] = 1;
}

void * contextSwitch(void * RSP){
    remainingQuantum--;
    if (remainingQuantum>0){
        return RSP;
    }
    if (currentProcess!=NULL){
        currentProcess->RSP = RSP;
        if (currentProcess->status == Running){
            currentProcess->status = Ready;
        }
    }
    int foundNext = 0;
    for(int i = 0; i < PRIORITY_LEVELS && !foundNext; i++){
        startIterator(queues[i]);
        while(hasNext(queues[i])){
            currentProcess = next(queues[i]);
            if (currentProcess->status == WaitingForChildren && currentProcess->childrenCount == 0){
                currentProcess->status = Ready;
            }
            if (currentProcess->status == Ready){
                foundNext = 1;
                remove(queues[i], currentProcess->pid);
                quantum = PRIORITY_LEVELS - i;
                remainingQuantum = quantum;
                enqueue(queues[i], currentProcess);
                break;
            }
        }
    }
    if (!foundNext){
        quantum = 1;
        remainingQuantum = quantum;
        currentProcess = dummyProcess;
    }
    currentProcess->status = Running;
    return currentProcess->RSP;
}

void addProcess(void (* program)(int argc, char ** argv), char *name, int argc, char ** argv, uint8_t priority){
    uint64_t pid = 0;
    for(int i = DUMMY_PID+1; i < MAX_PROCESS_COUNT; i++){
        if (usedPIDs[i]==0){
            pid = i;
            usedPIDs[i]=1;
            break;
        }
    }
    if (!pid){
        return;
    }
    uint64_t ppid = currentProcess == NULL ? DUMMY_PID : currentProcess->pid;
    Process * process = initializeProcess(pid, ppid, name, argc, argv, program);
    currentProcess->childrenCount++;
    if (priority >= PRIORITY_LEVELS){
        priority = PRIORITY_LEVELS-1;
    }
    enqueue(queues[priority], process);
}


void killProcess(uint64_t pid){
    Process * process;
    for(int i = 0; i < PRIORITY_LEVELS; i++){
        process = remove(queues[i], pid);
        if (process != NULL){
            break;
        }
    }
    if (process==NULL){
        return;
    }
    Process * parent = findProcess(process->ppid);
    if (parent != NULL){
        parent->childrenCount--;
    }
    if(process->pid == currentProcess->pid){
        yield();
    }

    usedPIDs[process->pid] = 0;
    freeProcess(process);
}

void killCurrentProcess(){
    killProcess(currentProcess->pid);
}

uint64_t getCurrentPid(){
    return currentProcess->pid;
}

void setProcessPriority(uint64_t pid, uint8_t priority){
    if (priority >= PRIORITY_LEVELS){
        priority = PRIORITY_LEVELS-1;
    }
    for(int i = 0; i < PRIORITY_LEVELS; i++){
        Process * process = remove(queues[i], pid);
        if (process!=NULL){
            enqueue(queues[priority], process);
            break;
        }
    }
}
void blockProcess(uint64_t pid){
    Process * process = findProcess(pid);
    if (process == NULL || process->status == Blocked){
        return;
    }
    enum status oldStatus = process->status;
    process->status = Blocked;
    if (oldStatus == Running){
        setProcessPriority(pid, PRIORITY_LEVELS - quantum + remainingQuantum);
        yield();
    }
}

void unblockProcess(uint64_t pid){
    Process * process = findProcess(pid);
    if (process == NULL){
        return;
    }
    process->status = Ready;

}

void waitForChildren(uint64_t pid){
    Process * process = findProcess(pid);
    if (process==NULL){
        return;
    }
    enum status oldStatus = process->status;
    process->status = WaitingForChildren;
    if (oldStatus==Running){
        yield();
    }
}

void yield(){
    remainingQuantum=0;
    forceTimerTick();
}

void schedulerInfo(){
    Process * process;
    for(int i = 0; i < PRIORITY_LEVELS; i++){
        startIterator(queues[i]);
        while(hasNext(queues[i])){
            process = next(queues[i]);
            printString("Nombre: ");
            printString(process->name);
            printString("; PID: ");
            printInt(process->pid);
            printString("; Prioridad: ");
            printInt(i);
            printString("; RSP: ");
            printInt((uint64_t)process->RSP);
            printString("; RBP: ");
            printInt((uint64_t)process->RBP);
            /*printString(";fg: ");
            printString(process->foreground ? "fg" : "bg");*/
            printString("\n");
        }
    }
}

Process * findProcess(uint64_t pid){
    Process * process = NULL;
    for(int i = 0; i<PRIORITY_LEVELS; i++){
        process = findProcessInQueue(queues[i], pid);
        if (process!=NULL){
            break;
        }
    }
    return process;
}



void dummy(int argc, char ** argv){
    while(1){
        _hlt();
    }
}
