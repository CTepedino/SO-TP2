#include <audioDriver.h>
#include <time.h>

void beep(uint8_t frequency, uint64_t ticks){
    startSound(frequency);
    sleep(ticks);
    endSound();
}