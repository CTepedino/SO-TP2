#ifndef APPLICATIONS_H
#define APPLICATIONS_H

#include <stdint.h>
#include <string.h>
#include <generalLib.h>
#include <syscalls.h>

void killProcess(int argc, char** argv);
void niceProcess(int argc, char** argv);
void blockProcess(int argc, char** argv);
void ps();
void loopProcess(int argc, char** argv);

#endif