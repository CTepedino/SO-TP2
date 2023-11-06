#include <applications.h>

void killProcess(int argc, char** argv){
    if(argc < 2){
        print("Usage: kill <pid>\n");
        return;
    }
    uint64_t pid = (uint64_t) atoi(argv[1]);
    if(pid == 0){
        print("Invalid pid\n");
        return;
    }
    sys_kill(pid);
}
void niceProcess(int argc, char** argv){
    if(argc < 3){
        print("Usage: nice <pid> <priority>\n");
        return;
    }
    uint64_t pid = (uint64_t) atoi(argv[1]);
    if(pid <=0){
        print("Invalid pid\n");
        return;
    }
    uint8_t priority = (uint8_t) atoi(argv[2]);
    if(priority<1||priority>8){
        print("Invalid priority\n");
        return;
    }
    sys_nice(pid,priority);
}
void blockProcess(int argc, char** argv){
    if(argc < 2){
        print("Usage: block <pid>\n");
        return;
    }
    uint64_t pid = (uint64_t) atoi(argv[1]);
    if(pid <= 0){
        print("Invalid pid\n");
        return;
    }
    sys_block(pid);
}
void ps(){
    sys_ps();
}
void loopProcess(int argc, char** argv){
   if(argc < 2){
        print("Usage: loop <number>\n");
        return;
    }
    int ms = atoi(argv[1]);
    if(ms <0){
        print("Invalid sleep time\n");
        return;
    }

    uint64_t myPid;
    char pidString[5];
    
    while(1){
        myPid = sys_getpid();
        intToString(myPid, pidString, 10);
        print("Hello! My pid is: ");
        print(pidString);
        print("\n");
        sleep(ms); 
    }
}