#include <include/process.h>
#include <include/queue.h>
#include <memoryManager.h>
#include <lib.h>

process createProcess(char *name,uint64_t pid, int *fds, uint8_t priority, uint8_t foreground){
    process p = memAlloc(sizeof(process_t));
    strcpy(p->name,name);
    p->pid=pid;
    p->priority=priority;
    p->state = READY;
    //falta stack
    p->fd[FD_READ] = fds[0];
    p->fd[FD_WRITE] = fds[1];
    p->fd[FD_ERROR] = 2; 
    p->foreground = foreground;
    return p;
}