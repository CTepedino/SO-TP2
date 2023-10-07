#ifndef BIGNUMBERS_H
#define BIGNUMBERS_H

#include <generalLib.h>

#define CHAR_HEIGHT 7
#define CHAR_WIDTH 4

#define ____ 0x00
#define ___X 0x01
#define __X_ 0x02
#define __XX 0x03
#define _X__ 0x04
#define _X_X 0x05
#define _XX_ 0x06
#define _XXX 0x07
#define X___ 0x08
#define X__X 0x09
#define X_X_ 0x0A
#define X_XX 0x0B
#define XX__ 0x0C
#define XX_X 0x0D
#define XXX_ 0x0E
#define XXXX 0x0F

void drawBigNumber(uint8_t number, uint8_t size, uint32_t x, uint32_t y);

#endif
