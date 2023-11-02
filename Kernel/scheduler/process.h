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
        uint64_t pid;
        uint64_t ppid;
        uint8_t status;
        uint8_t priority;
        void *stackPointer; 
        char name[NAME];
        uint8_t foreground;
        int fd[3];

}process_t;
typedef process_t * process;

process createProcess(char *name,uint64_t pid, uint64_t ppid, char **args, int *fds, uint8_t priority, uint8_t foreground);