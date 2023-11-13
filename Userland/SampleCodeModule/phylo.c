#include <phylo.h>


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


void phylo() {
    end = 0;
    currentCount = 0;
    mutexId = semOpenNew(1);
    printId = semOpenNew(1);

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
        kill(pids[i]);
        semClose(s[i]);
        semClose(safe[i]);
    }

   semClose(mutexId);
   semClose(printId);

}

void addPhylo(){
    semWait(mutexId);
    if(currentCount == MAX_PHYLOS){
        print("MAX PHYLOS REACHED");
    } else{
        state[currentCount] = THINKING;
        s[currentCount] = semOpenNew(0);
        safe[currentCount] =semOpenNew(1);

        char string[12] = { "philosopher" };
        char ** philos = { 0 };

        char ** args = (char **) memAlloc(3 * sizeof(char *));
        char * buf = (char *) memAlloc(8);
        intToString(currentCount, buf,10);

        strcpy(args[0], string);
        args[1] = buf;
        philos = args;
        int fds[2] = {STDIN, STDOUT};
        pids[currentCount] = execve(philosopher, "philosopher", 2, philos, 0, fds);
        if( pids[currentCount] <= 0) {
            print("error creating philosopher.");
            return;
        }


        currentCount++;
    }
    semPost(mutexId);
}

void removePhylo(){
    if(currentCount == MIN_PHYLOS){
        print("MIN PHYLOS REACHED");
        return;
    }

    semWait(safe[currentCount-1]);
    semWait(mutexId);

    currentCount--;
    semClose(safe[currentCount]);
    kill(pids[currentCount]);
    semClose(s[currentCount]);
    
    semPost(mutexId);
}

void philosopher(int argc, char ** num) {
    int i = my_atoi(num[1]);
    while(!end) {
        semWait(safe[i]);
        think();
        takeForks(i);
        eat();
        putForks(i);
        semPost(safe[i]);
        sleep(50);
    }
}


void takeForks(int i) {
    semWait(mutexId);
    state[i] = HUNGRY;
    test(i);
    semPost(mutexId);
    semWait(s[i]);
}


void putForks(int i) {
    semWait(mutexId);
    state[i] = THINKING;
    test(LEFT);
    test(RIGHT);
    semPost(mutexId);
}


void test(int i) {
    if(state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
        state[i] = EATING;
        semPost(s[i]);
    }
}

void eat() {
    for(int i = 0; i < 5000000; i++)
        ;
    semWait(printId);
    for(int i = 0; i < currentCount; i++) {
        print(state[i] == EATING ? "E " : ". ");
        print(" "); 
    }
    print("\n");
    semPost(printId);
}

void think() {
     int i = 0;
    while (i < 10000000)
    {
        i++;
    }
}