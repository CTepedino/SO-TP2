#ifndef AUDIODRIVER_H
#define AUDIODRIVER_H
#include <stdint.h>

void beep(uint8_t frequency, uint64_t ticks);

void startSound(uint8_t frequency);
void endSound();
#endif
