#include <phylo.h>


#define MAX_PHYLOS 10
#define MIN_PHYLOS 5
#define LEFT    (i + currentCount - 1) % currentCount
#define RIGHT   (i + 1) % currentCount
#define THINKING    0
#define HUNGRY      1
#define EATING      2

int mutex;

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
    mutex = semOpenNew(1);

    for(int i = 0 ; i < MIN_PHYLOS; i++){
        addPhylo();
    }

    char c;
    while((c = getChar())!=EOF){
        switch (c){
            case 'a':
                addPhylo();
                break;
            case 'r':
                removePhylo();
                break;
        }
    }

    for(int i = 0 ; i < currentCount; i++){
        kill(pids[i]);
        semClose(s[i]);
        semClose(safe[i]);
    }

   semClose(mutex);

}

void addPhylo(){
    semWait(mutex);
    if(currentCount == MAX_PHYLOS){
        print("Maximo de filosofos alcanzado");
    } else{
        state[currentCount] = THINKING;
        s[currentCount] = semOpenNew(0);
        safe[currentCount] = semOpenNew(1);

        char ** args = memAlloc(sizeof(char *));
        char buf[10];
        intToString(currentCount, buf,10);
        args[0] = buf;
        int fds[2] = {STDIN, STDOUT};
        pids[currentCount] = execve(philosopher, "philosopher", 1, args, 0, fds);
        memFree(args);
        currentCount++;
    }
    semPost(mutex);
}

void removePhylo(){
    if(currentCount == MIN_PHYLOS){
        print("Minimo de filosofos alancazado");
        return;
    }

    semWait(safe[currentCount-1]);
    semWait(mutex);

    currentCount--;
    semClose(safe[currentCount]);
    kill(pids[currentCount]);
    semClose(s[currentCount]);
    
    semPost(mutex);
}

void philosopher(int argc, char ** argv) {
    int i = my_atoi(argv[0]);
    while(!end) {
        semWait(safe[i]);
        think();
        takeForks(i);
        eat();
        putForks(i);
        semPost(safe[i]);
        sleep(25);
    }
}


void takeForks(int i) {
    semWait(mutex);
    state[i] = HUNGRY;
    look(i);
    semPost(mutex);
    semWait(s[i]);
}


void putForks(int i) {
    semWait(mutex);
    state[i] = THINKING;
    look(LEFT);
    look(RIGHT);
    semPost(mutex);
}


void look(int i) {
    if(state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
        state[i] = EATING;
        semPost(s[i]);
    }
}

void eat() {
    for(int i = 0; i < currentCount; i++) {
        print(state[i] == EATING ? "E " : ". ");
        print(" "); 
    }
    print("\n");
}

void think() {
    sleep(10);
}