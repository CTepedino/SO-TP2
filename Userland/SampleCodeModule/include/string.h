#ifndef STRING_H
#define STRING_H
#include <stdint.h>
#include <stdlib.h>


int strcmp(const char* str1, const char* str2);

uint64_t strlen(const char* s);

void intToStringL(uint64_t num, char *string, uint8_t base, uint32_t intLength);

void intToString(uint64_t num, char * string, uint8_t base);

uint32_t uIntLen(uint64_t num, uint8_t base);

void toUpper(char * buffer, const char * string);

char * strtok(char * str, char delim);

void strcpy(const char * src, char * dst);
int isVowel(char letter);
uint64_t atoi(const char * string);

#endif