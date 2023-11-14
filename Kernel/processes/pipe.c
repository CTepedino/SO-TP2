#include <pipe.h>

typedef struct Pipe {
    char buffer[PIPE_BUFFER_SIZE];
    uint16_t rIndex;
    uint16_t size;
    uint64_t inputPid;
    uint64_t outputPid;
    uint8_t mutex;
} Pipe;

Pipe * pipes[MAX_PIPE_COUNT];

void sendEOF(Pipe * p, int idx){
    if (p->size >= PIPE_BUFFER_SIZE){
        p->mutex = 1;
        blockProcess(p->inputPid);
    }
    if (pipes[idx]==NULL){
        return;
    }
    p->buffer[(p->rIndex+p->size)%PIPE_BUFFER_SIZE] = EOF;
    pipes[idx]->size++;
    if (p->mutex){
        unblockProcess(p->outputPid);
        p->mutex = 0;
    }
}

int findPipe(int id){
    int idx = id - DEFAULT_FD_COUNT;
    if (idx < 0 || idx >= MAX_PIPE_COUNT){
        return -1;
    }
    return idx;
}

int createNewPipe(){
    for(int i = 0; i < MAX_PIPE_COUNT; i++){
        if (pipes[i]==NULL){
            pipes[i] = memAlloc(sizeof(Pipe));
            pipes[i]->rIndex = 0;
            pipes[i]->rIndex = 0;
            pipes[i]->inputPid = 0;
            pipes[i]->outputPid = 0;
            pipes[i]->mutex = 0;
            for(int j = 0; j < MAX_PIPE_COUNT; j++){
                pipes[i]->buffer[j]=0;
            }
            return i + DEFAULT_FD_COUNT;
        }
    }
    return -1;
}

int openPipe(int64_t pid, int id, uint8_t mode){
    int idx = findPipe(id);
    if (idx==-1){
        return -1;
    }
    if (pipes[idx]==NULL){
        pipes[idx] = memAlloc(sizeof(Pipe));
        pipes[idx]->rIndex = 0;
        pipes[idx]->size = 0;
        pipes[idx]->inputPid = 0;
        pipes[idx]->outputPid = 0;
        pipes[idx]->mutex = 0;
        for(int i = 0; i < MAX_PIPE_COUNT; i++){
            pipes[idx]->buffer[i]=0;
        }
    }
    if (mode==MODE_READ && pipes[idx]->outputPid==0){
        pipes[idx]->outputPid = pid;
    } else if (mode==MODE_WRITE && pipes[idx]->inputPid==0) {
        pipes[idx]->inputPid = pid;
    } else {
        return -1;
    }
    return 0;
}

void closePipe(int64_t pid, int id){
    int idx = findPipe(id);
    if (idx==-1 || pipes[idx]==NULL) {
        return;
    }
    if (pid == pipes[idx]->inputPid){
        sendEOF(pipes[idx], idx);
    } else if (pid == pipes[idx]->outputPid){
        memFree(pipes[idx]);
        pipes[idx]=NULL;
    }
}

int readPipe(int id, char * buffer, uint64_t length){
    int idx = findPipe(id);
    if (length==0 || idx==-1 || pipes[idx]==NULL || pipes[idx]->outputPid != getCurrentPid()) {
        return 0;
    }
    uint8_t eofFound = 0;
    uint64_t readCount = 0;
    Pipe * p = pipes[idx];
    while(readCount < length && !eofFound){
        if (p->size == 0 && p->buffer[p->rIndex] != EOF){
            p->mutex = 1;
            blockProcess(p->outputPid);
        }
        while(readCount < length && (pipes[idx]->size > 0 || (int) pipes[idx]->buffer[pipes[idx]->rIndex]==EOF)){
            buffer[readCount] = p->buffer[p->rIndex];
            if ((int) buffer[readCount++]==EOF){
                eofFound=1;
                break;
            }
            p->size--;
            p->rIndex = (p->rIndex+1)%PIPE_BUFFER_SIZE;
        }
        if (p->mutex){
            unblockProcess(p->inputPid);
            p->mutex = 0;
        }

    }
    return readCount;
}

int writePipe(int id, const char * string, uint64_t count){
    int idx = findPipe(id);
    if (count == 0 || idx == -1 || pipes[idx]==NULL || pipes[idx]->inputPid != getCurrentPid()){
        return 0;
    }
    uint64_t writeCount = 0;
    Pipe * p = pipes[idx];
    while(writeCount < count && (int)p->buffer[(p->rIndex+p->size)%PIPE_BUFFER_SIZE]!=EOF){
        if (p->size >= PIPE_BUFFER_SIZE){
            p->mutex = 1;
            blockProcess(p->inputPid);
        }
        if (pipes[idx]==NULL){
            return writeCount;
        }
        while(p->size < PIPE_BUFFER_SIZE && writeCount < count){
            p->buffer[(p->rIndex+p->size)%PIPE_BUFFER_SIZE] = string[writeCount];
            if ((int)string[writeCount++]==EOF){
                break;
            }
            pipes[idx]->size++;
        }
        if (p->mutex){
            unblockProcess(p->outputPid);
            p->mutex = 0;
        }

    }
    return writeCount;
}