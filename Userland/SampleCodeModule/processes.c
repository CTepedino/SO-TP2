#include <processes.h>

void killProcess(int argc, char** argv){
    if(argc < 2){
        printf("Usage: kill <pid>\n");
        return;
    }
    int pid = atoi(argv[1]); //hacer atoi
    if(pid == 0){
        printf("Invalid pid\n");
        return;
    }
    sys_kill(pid);
}
void niceProcess(int argc, char** argv){
    if(argc < 3){
        printf("Usage: nice <pid> <priority>\n");
        return;
    }
    int pid = atoi(argv[1]); //hacer atoi
    if(pid <=0){
        printf("Invalid pid\n");
        return;
    }
    int priority = atoi(argv[2]); //hacer atoi
    if(priority<1||priority>8){
        printf("Invalid priority\n");
        return;
    }
    sys_nice(pid,priority);
}
void blockProcess(int argc, char** argv){
    if(argc < 2){
        printf("Usage: block <pid>\n");
        return;
    }
    int pid = atoi(argv[1]); //hacer atoi
    if(pid <= 0){
        printf("Invalid pid\n");
        return;
    }
    sys_block(pid);
}
void ps(){
    sys_ps();
}
void loopProcess(int argc, char** argv){
   if(argc < 2){
        printf("Usage: loop <number>\n");
        return;
    }
    int ms = atoi(argv[1]); //hacer atoi
    if(ms <0){
        printf("Invalid sleep time\n");
        return;
    }
    
    while(1){
        printf("Hello! My pid is: %d\n", sys_getpid());
    sleep(ms); //hacer sleep
    }
}