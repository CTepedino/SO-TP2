#include <scheduler/scheduler.h> 
#include <scheduler/queue.h>
#include <stddef.h>
#include <stdint.h>
#include <scheduler/process.h>

Queue queue=NULL;

void initScheduler(){
queue = newQueue();

}
void addProcess(process p){
enqueue(queue,p);
}

void blockProcessPid(uint64_t pid){
   if(queue=NULL || queue->first==NULL){
    return ;
   } 
   process p=getProcessFromPid(queue,pid);
    if(p==NULL){
     return ;
    }
    p->status=BLOCKED;
    if(pid==getRunningPid()){
        forceTimerTick();//ver
    }
}
int getRunningPid(){
    if(queue==NULL || queue->first==NULL){
        return -1;
    }
    
    return queue->first->pcb->pid;
}

void yield(){

forceTimerTick();
}

void unlockProcessPid(uint64_t pid){
    if(queue==NULL|| queue->first==NULL){
        return ;
    }
    process p=getProcessFromPid(queue,pid);
    if(p==NULL||p->status==KILLED||p->status==READY){
        return ;
    }
    p->status=READY;
   return ;
}

void blockProcessCurrent(){
  uint64_t pid=getRunningPid();
    if(pid==-1){
        return ;
    }
    blockProcessPid(pid);
}

void unlockProcessCurrent(){
    uint64_t pid=getRunningPid();
    if(pid==-1){
        return ;
    }
    unlockProcessPid(pid);
}

void killProcessPid(uint64_t pid){
    if(queue==NULL|| queue->first==NULL){
        return ;
    }
    process p=getProcessFromPid(queue,pid);
    if(p==NULL){
        return ;
    }
    p->status=KILLED;
    if(pid==getRunningPid()){
        forceTimerTick();
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
    if(priority>MAX_PRIORITY){
        return ;
    }
    p->priority=priority;
}




