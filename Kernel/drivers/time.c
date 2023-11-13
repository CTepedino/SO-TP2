#include <time.h>

static uint64_t ticks = 0;

void timer_handler() {
    ticks++;
}

void sleep(uint64_t tickLength){
    uint64_t startTime = ticks;
    while(ticks<startTime+tickLength){}
}