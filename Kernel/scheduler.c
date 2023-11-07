#include <scheduler.h> 

Queue queue = NULL;
uint64_t pidCounter = 1;
process currentProcess = NULL;
uint64_t idleTaskPid;

static void fillPCB(process pcb, uint64_t pid, uint64_t *base, int foreground,
                    uint64_t fdIn, uint64_t fdOut, char *name);

static void idleTask(){
	while(1)
		_hlt();
}

void initScheduler(){

    idleTaskPid = createProcess((uint64_t)&idleTask, BACKGROUND, FD_READ, FD_WRITE, "idle", NULL, NULL, "idle");

    blockProcessPid(idleTaskPid);
    
    queue = newQueue();
}

uint64_t createProcess(uint64_t *entryPoint, uint8_t foreground, uint64_t fdIn, uint64_t fdOut,
                           uint64_t first, uint64_t second, uint64_t third, char *name) {
    uint64_t *base;
    if ((base = memAlloc(STACK_SIZE)) == NULL) {
        return 0;
    }
    base[STACK_SIZE - 1] = START_STACK_SEGMENT;
    base[STACK_SIZE - 2] = (uint64_t)(base + STACK_SIZE); //rbp
    base[STACK_SIZE - 3] = START_RFLAGS;
    base[STACK_SIZE - 4] = START_CODE_SEGMENT;
    base[STACK_SIZE - 5] = (uint64_t) entryPoint;      //rip
    for (int i = 0; i < GENERAL_REGISTER_AMOUNT; i++) {
        base[STACK_SIZE - i - 6] = GENERAL_REGISTER_AMOUNT - i;
    }

    base[STACK_SIZE - 11] = first; //rdi;
    base[STACK_SIZE - 12] = second; //rsi;
    base[STACK_SIZE - 9] = third; //rdx;

    process pcb;
    if ((pcb = memAlloc(sizeof(process_t))) == NULL) {
        return 0;
    }
    fillPCB(pcb, pidCounter, base, foreground, fdIn, fdOut, name);
    enqueue(queue, pcb);
    return pidCounter++;
}

static void fillPCB(process pcb, uint64_t pid, uint64_t *base, int foreground,
                    uint64_t fdIn, uint64_t fdOut, char *name) {
    pcb->pid = pid;
    pcb->state = READY;
    pcb->rsp = base + STACK_SIZE - REGISTER_AMOUNT;
    pcb->rbp = base + STACK_SIZE;
    pcb->priority = 1;
    pcb->tickets = 1;
    pcb->foreground = foreground;
    pcb->fd[FD_READ] = fdIn;
    pcb->fd[FD_WRITE] = fdOut;
    pcb->waitingPid = 0;
    pcb->name[0] = 0;
    strcpy(pcb->name, name);
}

uint64_t * contextSwitch(uint64_t *rsp) {
    if (currentProcess != NULL) {
        currentProcess->rsp = rsp;

        if (currentProcess->state == READY && currentProcess->tickets > 0) {
            currentProcess->tickets--;
            return currentProcess->rsp;
        }
        
        currentProcess->tickets = currentProcess->priority;
    }

    process aux = dequeueReady(queue);

    if(aux == NULL){
        currentProcess = getProcessFromPid(queue, idleTaskPid);
        unlockProcessPid(idleTaskPid);
    }
    else if(aux->pid != idleTaskPid){
        currentProcess = aux;
        blockProcessPid(idleTaskPid);
    }

    currentProcess->tickets--;
    return currentProcess->rsp;
}


void blockProcessPid(uint64_t pid){
    if(queue=NULL || queue->first==NULL){
        return ;
    } 
    process p = getProcessFromPid(queue,pid);
    if(currentProcess == NULL){
     return ;
    }
    p->state=BLOCKED;
    if(pid==currentProcess->pid){
        _forceTimerInt();
    }
}

void yield(){
    _forceTimerInt();
}

void unlockProcessPid(uint64_t pid){
    if(queue==NULL|| queue->first==NULL){
        return ;
    }
    process p = getProcessFromPid(queue,pid);
    if(p == NULL || p->state == KILLED || p->state == READY){
        return ;
    }
    p->state = READY;
    return ;
}

void blockProcessCurrent(){
    if(currentProcess == NULL){
        return ;
    }
    blockProcessPid(currentProcess->pid);
}

void unlockProcessCurrent(){
    if(currentProcess == NULL){
        return ;
    }
    unlockProcessPid(currentProcess->pid);
}

void switchStates(unsigned int pid){
    process aux;
    if ((aux = getProcessFromPid(queue, pid)) != NULL) {
        aux->state = aux->state == BLOCKED ? READY : BLOCKED;
    }
}

void killProcessPid(uint64_t pid){
    if(queue==NULL|| queue->first==NULL){
        return ;
    }
    process p=getProcessFromPid(queue,pid);
    if(p==NULL){
        return ;
    }
    p->state=KILLED;
    remove(queue, pid);
    // removeWaitingPid(p->pid);
    // pipeClose(p->fd[FD_READ]);
    // pipeClose(p->fd[FD_WRITE]);
    memFree(p->rbp - STACK_SIZE);
    memFree(p);
    if(pid == currentProcess->pid){
        _forceTimerInt();
    }
}

void changePriority(uint64_t pid, uint8_t priority){
    if(queue==NULL|| queue->first==NULL){
        return ;
    }
    process p=getProcessFromPid(queue,pid);
    if(p==NULL){
        return ;
    }
    if(priority>MAX_PRIORITY || priority < MIN_PRIORITY){
        return ;
    }
    p->priority = priority;
    p->tickets = priority;
}

uint64_t getPid() {
    return currentProcess->pid;
}

void addWaitingPid(uint64_t pid) {
    process aux;
    if ((aux = getProcessFromPid(queue, pid)) != NULL) {
        aux->waitingPid = currentProcess->pid;
        blockProcessPid(currentProcess->pid);
    }
}

void exitCurrentProcess() {
    int waiting = currentProcess->waitingPid;
    if (waiting != 0) {
        unlockProcessPid(waiting);
    }

    // if (getFdOut() != STDOUT) {
    //     pipeClose(getFdOut());
    // }
    // if (getFdIn() != STDIN) {
    //     pipeClose(getFdIn());
    // }

    killProcessPid(getPid());
    currentProcess = NULL;

    _forceTimerInt();
}

void printProcesses() {
    printQueue(queue);
}

void alter_process_state(uint64_t pid, uint8_t new_state){
	process aux = getProcessFromPid(queue, pid);
    if (aux != NULL){
        aux->state = new_state;
    }
}

void alter_from_state(uint8_t old_state, uint8_t new_state){
    alter_state_if(queue, old_state, new_state);
}



