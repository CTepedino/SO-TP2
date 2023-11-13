#ifndef SHELL_H
#define SHELL_H

#include <generalLib.h>
#include <stdint.h>
#include <string.h>
#include <pong.h>
#include <exceptionTesters.h>
#include <tests.h>
#include <phylo.h>

#define READBUF_LENGTH 50
#define COMMANDS_LENGTH 7
#define TIME_LENGTH 9

#define MAX_NAME_LENGTH 20
#define MAX_DESC_LENGTH 100
#define MAX_ARG_COUNT 10

typedef struct Command{
    char name[MAX_NAME_LENGTH];
    char desc[MAX_DESC_LENGTH];
    void (*function)(int argc, char ** argv);
} Command;

void shellStart();

void executeCommand(Command command, int argc, char ** argv, int foreground);

void help();
void time();
void regStatus();

void argTest(int argc, char ** argv);

void killShell(int argc, char ** argv);
void nice(int argc, char ** argv);
void block(int argc, char ** argv);

#endif