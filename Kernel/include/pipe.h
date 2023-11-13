#ifndef _PIPES_H_
#define _PIPES_H_

#include <semaphore.h>
#include <memoryManager.h>
#include <scheduler.h>
#include <stdint.h>
#include <lib.h>

#define MODE_READ 0
#define MODE_WRITE 1
#define PIPE_BUFFER_SIZE (1024*4)

int createNewPipe();
int openPipe(int64_t pid, int id, uint8_t mode);
void closePipe(int64_t pid, int id);

int readPipe(int id, char * buffer, uint64_t length);
int writePipe(int id, const char * string, uint64_t count);




#endif