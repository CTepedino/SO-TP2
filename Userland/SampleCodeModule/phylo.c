#include <syscalls.h>
#include <generalLib.h>
#include <string.h>


#define MAX_PHYLOS 15
#define MIN_PHYLOS 5
#define LEFT    (i + currentCount - 1) % currentCount
#define RIGHT   (i + 1) % currentCount
#define THINKING    0
#define HUNGRY      1
#define EATING      2

int mutexId;
int printId;

typedef int sem_t;
int end = 0;

int state[MAX_PHYLOS] = { 0 };
sem_t s[MAX_PHYLOS] = { 0 };
sem_t safe[MAX_PHYLOS] = { 0 };
int pids[MAX_PHYLOS] = { 0 };
int currentCount = 0;

void philosopher(char ** num);
void takeForks(int i);
void addPhylo();
void removePhylo();
void putForks(int i);
void test(int i);
void eat();
void think();

void phylo() {
    end = 0;
    currentCount = 0;
      mutexId = sys_open_new_sem(1);
    printId = sys_open_new_sem(1);
    sys_openSem("mutex",1);
    sys_openSem("print",1);

    for(int i = 0 ; i < MIN_PHYLOS; i++){
        addPhylo();
    }

    char c;
    while(!end){
       c = getChar();
        switch (c){
            case 'a':
                addPhylo();
                break;
            case 'r':
                removePhylo();
                break;
            case 'q':
                end = 1;
                break;
        }
    }

    for(int i = 0 ; i < currentCount; i++){
    sys_killProcess(pids[i]);
      sys_closeSem(s[i]);
      sys_closeSem(safe[i]);
    }

   sys_closeSem(mutexId);
   sys_closeSem(printId);

}

void addPhylo(){
    sys_waitSem(mutexId);
    if(currentCount == MAX_PHYLOS){
        print("MAX PHYLOS REACHED");
    } else{
        state[currentCount] = THINKING;
        s[currentCount] = sys_open_new_sem(0);
        safe[currentCount] =sys_open_new_sem(1);

        char string[12] = { "philosopher" };
        char ** philos = { 0 };

        char ** args = (char **) sys_malloc(3 * sizeof(char *));
        char * buf = (char *) sys_malloc(8);
       intToString(currentCount, buf,10);

        args[0] = (char *)(intptr_t) strcpy(args[0], string);
        args[1] = buf;
        philos = args;
        pids[currentCount] = sys_addProcess((uint64_t)&philosopher, "philosopher", 2, philos, 1);
        if( pids[currentCount] <= 0) {
            print("error creating philosopher.");
            return;
        }


        currentCount++;
    }
    sys_postSem(mutexId);
}

void removePhylo(){
    if(currentCount == MIN_PHYLOS){
        print("MIN PHYLOS REACHED");
        return;
    }

    sys_waitSem(safe[currentCount-1]);
    sys_waitSem(mutexId);

    currentCount--;
  sys_closeSem(safe[currentCount]);
    sys_killProcess(pids[currentCount]);
   sys_closeSem(s[currentCount]);
    
   sys_postSem(mutexId);
}

void philosopher(char ** num) {
    int i = atoi(num[1]);
    while(!end) {
        sys_waitSem(safe[i]);
        think();
        takeForks(i);
        eat();
        putForks(i);
       sys_postSem(safe[i]);
    }
}


void takeForks(int i) {
    sys_waitSem(mutexId);
    state[i] = HUNGRY;
    test(i);
    sys_postSem(mutexId);
    sys_waitSem(s[i]);
}


void putForks(int i) {
    sys_waitSem(mutexId);
    state[i] = THINKING;
    test(LEFT);
    test(RIGHT);
   sys_postSem(mutexId);
}


void test(int i) {
    if(state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
        state[i] = EATING;
        sys_postSem(s[i]);
    }
}

void eat() {
    for(int i = 0; i < 5000000; i++)
        ;
    sys_waitSem(printId);
    for(int i = 0; i < currentCount; i++) {
        print(state[i] == EATING? "E " : ". ", 2);
    }
    print("");
   sys_postSem(printId);
}

void think() {
     int i = 0;
    while (i < 10000000)
    {
        i++;
    }
}