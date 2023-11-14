#include <pipe.h>


typedef struct Pipe {
    char buffer[PIPE_BUFFER_SIZE];
    uint16_t rIndex;
    uint16_t wIndex;
    uint64_t inputPid;
    uint64_t outputPid;
    int block;
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
            pipes[i] = memAlloc(sizeof(Pipe));
            pipes[i]->rIndex = 0;
            pipes[i]->wIndex = 0;
            pipes[i]->inputPid = 0;
            pipes[i]->outputPid = 0;
            pipes[i]->block=0;
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
        pipes[idx]->wIndex = 0;
        pipes[idx]->inputPid = 0;
        pipes[idx]->outputPid = 0;
        pipes[idx]->block=0;
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
        char eof[] = {EOF};
        writePipe(id, eof, 1);
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
    uint64_t readCount = 0;
    uint8_t end = 0;
    while(!end && readCount < length){
       if (pipes[idx]->rIndex==pipes[idx]->wIndex && (int)pipes[idx]->buffer[pipes[idx]->rIndex] != EOF){
           pipes[idx]->block = 1;
           blockProcess(pipes[idx]->outputPid);
       }
        while (readCount < length && (pipes[idx]->wIndex != pipes[idx]->rIndex || (int) pipes[idx]->buffer[pipes[idx]->rIndex] == EOF)) {
            buffer[readCount] = pipes[idx]->buffer[pipes[idx]->rIndex];
            if ((int) buffer[readCount] == EOF) {
                end = 1;
                break;
            }
            readCount++;
            pipes[idx]->rIndex++;
            pipes[idx]->rIndex = pipes[idx]->rIndex % PIPE_BUFFER_SIZE;
        }
        if (pipes[idx]->block){
            unblockProcess(pipes[idx]->inputPid);
            pipes[idx]->block=0;
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
    while(writeCount < count && pipes[idx]->buffer[pipes[idx]->wIndex]!=EOF){
        if(pipes[idx]->rIndex == pipes[idx]->wIndex){
            pipes[idx]->block=1;
            blockProcess(pipes[idx]->inputPid);
        }
        if(pipes[idx]==NULL){
            break;
        }
        while(pipes[idx]->wIndex != pipes[idx]->rIndex && writeCount < count){
            pipes[idx]->buffer[pipes[idx]->wIndex] = string[writeCount];
            if ((int)string[writeCount]==EOF){
                break;
            }
            writeCount++;
            pipes[idx]->wIndex++;
            pipes[idx]->wIndex = pipes[idx]->wIndex % PIPE_BUFFER_SIZE;
        }
        if (pipes[idx]->block){
            unblockProcess(pipes[idx]->outputPid);
            pipes[idx]->block=0;
        }

    }
    return writeCount;

}