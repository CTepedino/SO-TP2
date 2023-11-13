#ifndef KEYBOARDDRIVER_H
#define KEYBOARDDRIVER_H

#include <stdint.h>
#include <defs.h>
#include <scheduler.h>
#include <semaphore.h>
#define BUFFER_LEN 64
#define KEYMAP_LEN 59


#define F1_SCANCODE 0x3B
#define ESC 27
#define CTRL 29
#define SHIFT 42

void initializeKeyboardHandler();
void keyboard_handler();
void writeKey(char key);
int8_t getAscii();
void cleanBuffer();
void getAllKeys(char * c);
void getReleasedKeys(char * c);
int isKeyPress(uint8_t code);

char readKey();
#endif
