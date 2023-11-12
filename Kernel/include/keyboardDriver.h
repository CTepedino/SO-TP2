#ifndef KEYBOARDDRIVER_H
#define KEYBOARDDRIVER_H

#include <stdint.h>
#include <defs.h>
#include <scheduler.h>
#define F1_SCANCODE 0x3B

void keyboard_handler();
char getKey();
void cleanBuffer();
void getAllKeys(char * c);
void getReleasedKeys(char * c);
int isKeyPress(uint8_t code);

char readKey();
#endif
