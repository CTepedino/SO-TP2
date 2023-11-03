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

void colorPrint(const char * string){
    sys_write(COLOR, string, strlen(string));
}

char getChar(){
    char c;
    sys_read(STDIN,&c,1);
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
    void * mem = NULL;
    sys_malloc(n, mem);
    return mem;
}

void memFree(void * ptr){
    sys_free(ptr);
}

void memoryInfo(){
    sys_memoryInfo();
}