#include <process.h>

void startProcess(void (*program)(int argc, char ** argv), int argc, char ** argv);

Process * initializeProcess(uint64_t pid, uint64_t ppid, char * name, int argc, char ** argv, void (*program)(int argc, char ** argv), int fds[]){
    Process * newProcess = memAlloc(sizeof(Process));
    newProcess->pid = pid;
    newProcess->ppid = ppid;
    newProcess->status = Ready;
    newProcess->name = memAlloc(strlen(name)+1);
    copyString(newProcess->name, name);
    newProcess->argv = memAlloc(sizeof(char*) * (argc+1));
    for(int i = 0; i < argc; i++){
        newProcess->argv[i] = memAlloc(sizeof(char *) * strlen(argv[i]));
        copyString(newProcess->argv[i], argv[i]);
    }
    newProcess->argv[argc] = NULL;
    newProcess->RBP = memAlloc(STACK_SIZE);
    newProcess->RSP = initializeStack(&startProcess, program,(void *) ((uint64_t)newProcess->RBP+STACK_SIZE), argc, newProcess->argv);
    newProcess->childrenCount = 0;
    newProcess->fds.input = fds[0];
    newProcess->fds.output = fds[1];
    if (fds[0] >= DEFAULT_FD_COUNT){
        openPipe(pid, fds[0], MODE_READ);
    }
    if (fds[1]  >= DEFAULT_FD_COUNT){
        openPipe(pid, fds[1], MODE_WRITE);
    }
    return newProcess;
}

void freeProcess(Process * process){
    memFree(process->name);
    for(int i = 0; process->argv[i]!=NULL; i++){
        memFree(process->argv[i]);
    }
    if (process->fds.input >= DEFAULT_FD_COUNT){
        closePipe(process->pid, process->fds.input);
    }
    if (process->fds.output >= DEFAULT_FD_COUNT){
        closePipe(process->pid, process->fds.output);
    }
    memFree(process->argv);
    memFree(process->RBP);
    memFree(process);
}



void startProcess(void (*program)(int argc, char ** argv), int argc, char ** argv){
    program(argc, argv);
    killCurrentProcess();
}
