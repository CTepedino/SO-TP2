#include <pipe.h>


typedef struct Pipe {
    char buffer[PIPE_BUFFER_SIZE];
    uint16_t first;
    uint16_t size;
    uint64_t inputPid;
    uint64_t outputPid;
    int semWrite;
    int semRead;
} Pipe;

Pipe * pipes[MAX_PIPE_COUNT];

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
            int semWrite = openNewSem(PIPE_BUFFER_SIZE);
            int semRead = openNewSem(0);
            if (semWrite==-1 || semRead==-1){
                return -1;
            }
            pipes[i] = memAlloc(sizeof(Pipe));
            pipes[i]->first = 0;
            pipes[i]->size = 0;
            pipes[i]->inputPid = 0;
            pipes[i]->outputPid = 0;
            pipes[i]->semWrite = semWrite;
            pipes[i]->semRead = semRead;
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
        int semWrite = openNewSem(PIPE_BUFFER_SIZE);
        int semRead = openNewSem(0);
        if (semWrite==-1 || semRead==-1){
            return -1;
        }
        pipes[idx] = memAlloc(sizeof(Pipe));
        pipes[idx]->first = 0;
        pipes[idx]->size = 0;
        pipes[idx]->inputPid = 0;
        pipes[idx]->outputPid = 0;
        pipes[idx]->semWrite = semWrite;
        pipes[idx]->semRead = semRead;
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
        pipes[idx]->inputPid = 0;
    } else if (pid == pipes[idx]->outputPid){
        pipes[idx]->outputPid = 0;
    }
    if (pipes[idx]->outputPid == 0 && pipes[idx]->inputPid == 0){
        closeSem(pipes[idx]->semWrite);
        closeSem(pipes[idx]->semRead);
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
    while(readCount < length && !eofFound){
        waitSem(pipes[idx]->semRead);
        while(readCount < length && (pipes[idx]->size > 0 || (int) pipes[idx]->buffer[pipes[idx]->first]==EOF)){
            buffer[readCount] = pipes[idx]->buffer[pipes[idx]->first];
            if ((int) buffer[readCount++]==EOF){
                eofFound=1;
                break;
            }
            pipes[idx]->size--;
            pipes[idx]->first = (pipes[idx]->first+1)%PIPE_BUFFER_SIZE;
        }
        postSem(pipes[idx]->semWrite);
    }
    return readCount;
}

int writePipe(int id, const char * string, uint64_t count){
    int idx = findPipe(id);
    if (count == 0 || idx == -1 || pipes[idx]==NULL || pipes[idx]->inputPid != getCurrentPid()){
        return 0;
    }
    uint64_t writeCount = 0;
    while(pipes[idx]!=NULL && writeCount < count && (int)pipes[idx]->buffer[(pipes[idx]->first+pipes[idx]->size)%PIPE_BUFFER_SIZE]!=EOF){
        waitSem(pipes[idx]->semWrite);
        while(pipes[idx]->size < PIPE_BUFFER_SIZE && writeCount < count){
            pipes[idx]->buffer[(pipes[idx]->first+pipes[idx]->size)%PIPE_BUFFER_SIZE] = string[writeCount];
            if ((int)string[writeCount++]==EOF){
                break;
            }
            pipes[idx]->size++;
        }
        postSem(pipes[idx]->semRead);
    }
    return writeCount;
}