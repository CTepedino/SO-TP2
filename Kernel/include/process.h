#include <stdint.h>
#include <stddef.h>

#define READY 1
#define BLOCKED 2
#define KILLED 3
#define NAME 32
#define FD_READ 0
#define FD_WRITE 1
#define FD_ERROR 2

typedef struct process_t{
        char name[NAME];
        uint64_t pid;
        uint8_t state;
        uint8_t priority;
        uint64_t *rsp;
        uint64_t *rbp; 
        uint8_t foreground;
        int fd[3];
        uint64_t waitingPid;

}process_t;

typedef process_t * process;

process createProcess(char *name, uint64_t pid, int *fds, uint8_t priority, uint8_t foreground);