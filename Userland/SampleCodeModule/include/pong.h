#ifndef PONG_H
#define PONG_H
#include <stdint.h>
#include <generalLib.h>
#include <string.h>
#include <bigNumbers.h>
#define ESC 27
#define PADDLE_WIDTH 10
#define PADDLE_HEIGHT 45
#define PADDLE_SPEED 10
#define BALL_SIZE 11
#define BALL_INITIAL_SPEED_X 4
#define BALL_INITIAL_SPEED_Y 4
#define MID_LINE_HEIGHT 10
#define MID_LINE_WIDTH 5
#define NUMBER_SIZE 10

void pong();

#endif
