#include <stdint.h>
#include <stdlib.h>
#include <generalLib.h>
#include <syscalls.h>
#include <string.h>

#define WHITE 0xFFFFFF
#define BLACK 0

void putChar(char c){
    sys_write(STDOUT, &c, 1);
}

void print(const char* string){
    sys_write(STDOUT, string, strlen(string));
}

void println(const char * string){
    print(string);
    putChar('\n');
}


void colorPrint(const char * string){
    sys_write(COLOR, string, strlen(string));
}

char getChar(){
    char c;
    sys_read(&c,1);
    return c;
}

void textPosition(uint32_t x, uint32_t y){
    sys_textPosition(x,y);
}

void screenInfo(uint32_t * width, uint32_t * height){
    sys_screenInfo(width, height);
}

void scan(char * buf, uint64_t length){
    if (length == 0)
        return;
    
    uint64_t count = 0;
    while (1){
        char c = getChar();
        if (c == '\n'){
            buf[count < (length - 1) ? count : length - 1] = '\0';
            putChar(c);
            return;
        }
        if (c == '\b'){
            if (count > 0){
                count--;
                putChar(c);
            }
        }
        else{
            putChar(c);
            if(count < length - 1)
                buf[count] = c;
            count++;
        }
    }
}

void printInt(uint64_t n){
    uint32_t len = uIntLen(n, 10);
    char buffer[len+1];
    intToStringL(n, buffer, 10, len);
    print(buffer);
}





void getREGS(int * status,uint64_t * buffer){
    sys_getREGS(status, buffer);
}

void getTime(timeStruct * time){
    sys_getRTC(time);
}

void clearScreen(){
    uint32_t width, height;
    screenInfo(&width, &height);
    sys_putRectangle(BLACK, 0, 0, width, height);
}

void drawRectangle(uint32_t x, uint32_t y, uint32_t base, uint32_t height){
    sys_putRectangle(WHITE,x,y,base,height);
}

void drawCircle(uint32_t x, uint32_t y, uint8_t radius){
    sys_putCircle(WHITE,x,y,radius);
}

void clearRectangle(uint32_t x, uint32_t y, uint32_t base, uint32_t height){
    sys_putRectangle(BLACK,x,y,base,height);
}

void clearCircle(uint32_t x, uint32_t y, uint8_t radius){
    sys_putCircle(BLACK,x,y,radius);
}

void beep(uint8_t frequency, uint64_t ticks){
    sys_beep(frequency, ticks);
}

void getCurrentKeyPress(char * keys){
    sys_getCurrentKeyPress(keys);
}

void getCurrentReleasedKeys(char * rkeys){
    sys_getCurrentReleasedKeys(rkeys);
}

void sleep(uint64_t ticks){
    sys_sleep(ticks);
}

void * memAlloc(size_t n){
    return sys_malloc(n);
}

void memFree(void * ptr){
    sys_free(ptr);
}

void memoryInfo(){
    sys_memoryInfo();
    killCurrent();
}

uint64_t execve(void (* program)(int argc, char ** argv), char *name, int argc, char ** argv, uint8_t priority, int fds[]){
    return sys_addProcess(program, name, argc, argv, priority, fds);
}

void kill(uint64_t pid){
    sys_killProcess(pid);
}

void killCurrent(){
    sys_killCurrentProcess();
}

uint64_t getPid(){
    return sys_getCurrentPid();
}

void setPriority(uint64_t pid, uint64_t priority){
    sys_setProcessPriority(pid, priority);
}

void blockProcess(uint64_t pid){
    sys_blockProcess(pid);
}

void unblockProcess(uint64_t pid){
    sys_unblockProcess(pid);
}

void waitForChildren(uint64_t childPid){
    sys_waitForChildren(childPid);
}

void yield(){
    sys_yield();
}

void schedulerInfo(){
    sys_schedulerInfo();
    killCurrent();
}


int semOpen(int id, unsigned int value){
    return sys_openSem(id, value);
}

void semClose(int id){
    sys_closeSem(id);
}

void semPost(int id){
    sys_postSem(id);
}

void semWait(int id){
    sys_waitSem(id);
}

int openPipe(int id, uint8_t mode){
    return sys_openPipe(id, mode);
}

int createNewPipe(){
    return sys_createNewPipe();
}

void closePipe(int id){
    sys_closePipe(id);
}

int readPipe(int id, char * buffer, uint64_t length){
    return sys_readPipe(id, buffer, length);
}

int writePipe(int id, const char * string, uint64_t count){
    return sys_writePipe(id, string, count);
}

void wc(int argc, char *argv[]) {
  int lines = 0;
  char c;
  while ((c = getChar()) != 0) {
    if (c == '\n') {
      lines++;
    }
  }
  print("Line count: ");
  printInt(lines);
  print("\n");
  killCurrent();
}

void cat(int argc, char *argv[]) {
  char c;
  while ((c = getChar()) != 0)
    putChar(c);
  putChar('\n');
  killCurrent();
}

void filter(int argc, char *argv[]) {
  char c;
  while ((c = getChar()) != 0) {
    if (!isVowel(c)) {
      putChar(c);
    }
  }
  putChar('\n');
  killCurrent();
}

void loop(int argc,char** argv){
    while(1){
        print("Hello! My pid is: ");
        printInt(getPid());
        putChar('\n');
        putChar('\n');
        sleep(10); 
    }
    killCurrent();
}