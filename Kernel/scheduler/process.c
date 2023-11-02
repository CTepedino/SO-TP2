#include <scheduler/process.h>
#include <scheduler/queue.h>
process createProcess(char *name,uint64_t pid, uint64_t ppid, char **args, int *fds, uint8_t priority, uint8_t foreground){
    process p=somalloc(sizeof(process_t));
    strcpy(p->name,name);
    p->pid=pid;
    p->ppid=ppid;
    p->priority=priority;
    p->status = READY;
    //falta stack
    p->fd[FD_READ] = fds[0];
    p->fd[FD_WRITE] = fds[1];
    p->fd[FD_ERROR] = 0; 
    p->foreground = foreground;
    return p;
}